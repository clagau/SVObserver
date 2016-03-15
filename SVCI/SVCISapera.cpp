// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCISapera
// * .File Name       : $Workfile:   SVCISapera.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   13 Jun 2014 07:36:56  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVCISapera.h"

#include "ObjectInterfaces/SVImageEnums.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"

#include "SVCICallbackStruct.h"
#include "SVCIServerStruct.h"
#include "SVSaperaManager.h"

long g_lRefCount = 0;

SVCIServerStructCArray g_caServers;
SVCIAcqStructVector g_caAcqs;
SVCIBufferStructCArray g_caBuffers;

SVQueueObject g_FreeImageHandles;

// Local functions
static HRESULT g_LoadCCAFile(CORSERVER p_hCorServer, unsigned long p_ulIndex, LPCTSTR p_csFileName);
static HRESULT g_LoadCVIFile(CORSERVER p_hCorServer, unsigned long p_ulIndex, LPCTSTR p_csFileName);
static HRESULT g_LoadCCFFile(CORSERVER p_hCorServer, unsigned long p_ulIndex, LPCTSTR p_csFileName);

CORSTATUS __stdcall g_SVCITriggerCallback( void *p_pvContext, unsigned int p_uiEventType, unsigned int p_uiEventCount)
{
	::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

	try
	{
		unsigned long l_ulIndex = (unsigned long)p_pvContext;

		if ( 0 <= l_ulIndex && l_ulIndex < (unsigned long)g_caAcqs.size() )
		{
			if ( g_caAcqs[ l_ulIndex ].m_lIsStarted == 1 )
			{
				SVCallbackListClass::iterator l_pos = g_caAcqs[ l_ulIndex ].m_svTriggerCallbacks.GetHeadPosition();

				while ( l_pos != g_caAcqs[ l_ulIndex ].m_svTriggerCallbacks.end() )
				{
					const SVCallbackStruct &l_Data = g_caAcqs[ l_ulIndex ].m_svTriggerCallbacks.GetAt( l_pos );

					try
					{
						if ( l_Data.m_pCallback != NULL )
						{
							l_Data.m_pCallback( l_Data.m_pOwner, l_Data.m_pData );							
						}
					}
					catch ( ... )
					{
					}

					g_caAcqs[ l_ulIndex ].m_svTriggerCallbacks.GetNext( l_pos );
				}
			}
		}
	}
	catch ( ... )
	{
	}

	return CORSTATUS_OK;
}

VOID CALLBACK SVSaperaEndFrameAPCProc( DWORD dwParam )
{
}

HRESULT SVSaperaCameraProcessEndFrame( long p_CameraIndex, bool& p_rProcess )
{
	HRESULT l_Status = S_OK;

	p_rProcess = ( 0 < g_caAcqs[ p_CameraIndex ].m_BufferIndexQueue.size() );

	if( p_rProcess )
	{
		long l_BufferIndex = -1;

		if( g_caAcqs[ p_CameraIndex ].m_BufferIndexQueue.PopHead( l_BufferIndex ) == S_OK )
		{
			if( 0 <= l_BufferIndex && l_BufferIndex < g_caBuffers.GetSize() )
			{
				if( g_caAcqs[ p_CameraIndex ].m_lIsStarted == 1 && 
					g_caAcqs[ p_CameraIndex ].m_pBufferInterface != NULL )
				{
					SVImageBufferInterface l_Buffer;
					
					if( g_caAcqs[ p_CameraIndex ].m_pBufferInterface->GetNextBuffer( l_Buffer ) == S_OK )
					{
						l_Buffer.SetStartFrameTimeStamp( g_caBuffers[ l_BufferIndex ].m_StartTimeStamp );
						l_Buffer.SetEndFrameTimeStamp( g_caBuffers[ l_BufferIndex ].m_EndTimeStamp );

						unsigned char *l_pBuffer = l_Buffer.GetBufferAddress();

						if( l_pBuffer != NULL )
						{
							size_t l_Size = g_caBuffers[ l_BufferIndex ].m_Pitch * g_caBuffers[ l_BufferIndex ].m_ulHeight;

							memcpy( l_pBuffer, g_caBuffers[ l_BufferIndex ].m_pAddress, l_Size );

							g_caAcqs[ p_CameraIndex ].m_pBufferInterface->UpdateWithCompletedBuffer( l_Buffer );
						}
					}
				}

				g_caBuffers[ l_BufferIndex ].m_StartTimeStamp = 0;
				g_caBuffers[ l_BufferIndex ].m_EndTimeStamp = 0;

				SVSaperaManager::Instance().BufferSetPrm( g_caBuffers[ l_BufferIndex ].m_hCorBuffer, CORBUFFER_PRM_STATE, CORBUFFER_VAL_STATE_EMPTY );
			}
		}
	}

	return l_Status;
}

struct SVSaperaEndFrameThreadParam
{
	SVSaperaEndFrameThreadParam( long p_CameraIndex )
	:	m_CameraIndex( p_CameraIndex ) {}

	long m_CameraIndex;

};

DWORD WINAPI SVSaperaEndFrameThreadProc( LPVOID p_lpParam )
{
	// Perform Queue Servicing
	HRESULT hrOk = S_OK;

	SVSaperaEndFrameThreadParam *pItem = NULL;

	pItem = reinterpret_cast< SVSaperaEndFrameThreadParam* >( p_lpParam );

	if( pItem != NULL )
	{
		HANDLE hShutdown = NULL;

		if( DuplicateHandle( GetCurrentProcess(), g_caAcqs[ pItem->m_CameraIndex ].mhShutdown,
													GetCurrentProcess(), &hShutdown,
													0, FALSE,	DUPLICATE_SAME_ACCESS ) )
		{
			do
			{
				unsigned long ulReturn = WaitForSingleObjectEx( hShutdown, INFINITE, TRUE );

				if ( ulReturn == WAIT_IO_COMPLETION )
				{
					bool l_Process = true;

					while( hrOk == S_OK && l_Process )
					{
						ulReturn = WaitForSingleObjectEx( hShutdown, 0, FALSE );

						if ( ulReturn == WAIT_TIMEOUT )
						{
							hrOk = SVSaperaCameraProcessEndFrame( pItem->m_CameraIndex, l_Process );
						}
						else
						{
							hrOk = S_FALSE;
						}
					}
				}
				else
				{
					hrOk = S_FALSE;
				}
			}
			while( hrOk == S_OK );

			CloseHandle( hShutdown );

			hShutdown = NULL;
		}
		else
		{
			hrOk = E_FAIL;
		}

		delete pItem;
	}
	else
	{
		hrOk = E_FAIL;
	}

	return hrOk;
}

HRESULT SVSaperaCameraAPCProcessEndFrame( unsigned long p_CameraIndex, unsigned long p_BufferIndex, bool p_IsTrash )
{
	HRESULT l_hrOk = S_OK;

	if( 0 <= p_BufferIndex && p_BufferIndex < static_cast< size_t >( g_caBuffers.GetSize() ) )
	{
		bool l_StartIsTrash = g_caAcqs[ p_CameraIndex ].m_StartIsTrash;
		long l_StartIndex = g_caAcqs[ p_CameraIndex ].m_StartIndex;
		SVClock::SVTimeStamp l_StartFrameTimeStamp = g_caAcqs[ p_CameraIndex ].m_StartFrameTimeStamp;

		g_caAcqs[ p_CameraIndex ].m_StartIsTrash = true;
		g_caAcqs[ p_CameraIndex ].m_StartIndex = -1;
		g_caAcqs[ p_CameraIndex ].m_StartFrameTimeStamp = 0;

		if( !l_StartIsTrash && !p_IsTrash && l_StartIndex == p_BufferIndex && g_caAcqs[ p_CameraIndex ].m_pBufferInterface != NULL )
		{
			/*  JMS - Use with Sapera 7.0
			__int64 l_CorStartFrameTimeStamp = 0;

			SVSaperaManager::Instance().BufferGetPrm( g_caBuffers[ p_BufferIndex ].m_hCorBuffer, CORBUFFER_PRM_HOST_COUNTER_STAMP, &l_CorStartFrameTimeStamp );

			if( l_CorStartFrameTimeStamp != 0 )
			{
				l_StartFrameTimeStamp = l_CorStartFrameTimeStamp;
			}
			*/

			g_caBuffers[ p_BufferIndex ].m_StartTimeStamp = l_StartFrameTimeStamp;
			g_caBuffers[ p_BufferIndex ].m_EndTimeStamp = g_caAcqs[ p_CameraIndex ].m_pBufferInterface->GetTimeStamp();

			g_caAcqs[ p_CameraIndex ].m_BufferIndexQueue.PushTail( p_BufferIndex );

			QueueUserAPC( SVSaperaEndFrameAPCProc, g_caAcqs[ p_CameraIndex ].mhThread, NULL );
		}
		else
		{
			g_caBuffers[ p_BufferIndex ].m_StartTimeStamp = 0;
			g_caBuffers[ p_BufferIndex ].m_EndTimeStamp = 0;

			SVSaperaManager::Instance().BufferSetPrm( g_caBuffers[ p_BufferIndex ].m_hCorBuffer, CORBUFFER_PRM_STATE, CORBUFFER_VAL_STATE_EMPTY );
		}
	}

	return l_hrOk;
}

CORSTATUS __stdcall g_SVCIDigitizerCallback( void *p_pvContext, unsigned int p_uiEventType, unsigned int p_uiEventCount)
{
	::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

	try
	{
		SVCICallbackStruct *l_pData = (SVCICallbackStruct *)p_pvContext;

	if ( l_pData != NULL && l_pData->m_ulDigitizerIndex < (unsigned long)g_caAcqs.size() )
		{
			if ( g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_lIsStarted == 1 )
			{
				if( ( p_uiEventType & CORXFER_VAL_EVENT_TYPE_START_OF_FRAME ) == CORXFER_VAL_EVENT_TYPE_START_OF_FRAME )
				{
					if( g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_pBufferInterface != NULL )
					{
						g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_StartIsTrash = l_pData->m_IsTrash;
						g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_StartIndex = l_pData->m_ulBufferIndex;
						if( l_pData->m_IsTrash )
						{
							g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_StartFrameTimeStamp = 0;
						}
						else
						{
							g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_StartFrameTimeStamp = g_caAcqs[ l_pData->m_ulDigitizerIndex ].m_pBufferInterface->GetTimeStamp();
						}
					}
				}

				if( ( p_uiEventType & CORXFER_VAL_EVENT_TYPE_END_OF_FRAME ) == CORXFER_VAL_EVENT_TYPE_END_OF_FRAME )
				{
					SVSaperaCameraAPCProcessEndFrame( l_pData->m_ulDigitizerIndex, l_pData->m_ulBufferIndex, l_pData->m_IsTrash );
				}
			}
		}
	}
	catch ( ... )
	{
	}

	return CORSTATUS_OK;
}

HRESULT g_CICreate()
{
	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &g_lRefCount ) == 1 )
	{
		SVSaperaManager::Instance().Startup();

		if ( g_FreeImageHandles.Create() )
		{
			unsigned int l_uiServerCount = 0;

			HRESULT l_hrOk = SVSaperaManager::Instance().ManGetServerCount( &l_uiServerCount );

			for ( unsigned long i = 0; l_hrOk == S_OK && i < l_uiServerCount; i++ )
			{
				CORSERVER l_hCorBoard;
				char l_szName[40];

				l_hrOk = SVSaperaManager::Instance().ManGetServerByIndex( i, l_szName, &l_hCorBoard );
				
				if ( l_hrOk == S_OK )
				{
					unsigned int l_uiDigCount = 0;

					HRESULT l_Temp = SVSaperaManager::Instance().AcqGetCount( l_hCorBoard, &l_uiDigCount );

					if ( l_Temp == S_OK )
					{
						SVCIServerStruct l_Server;

						l_Server.m_csName = l_szName;
						l_Server.m_csName.Replace("X64-AN_","Viper_Quad_");
						l_Server.m_csName.Replace("Xcelera-AN_LX1_","Viper_Quad_");
						l_Server.m_hCorServer = l_hCorBoard;

						l_hrOk = SVSaperaManager::Instance().ManResetServer( l_hCorBoard );

						int l_iIndex = g_caServers.Add( l_Server );

						if ( l_iIndex < 0 )
						{
							l_hrOk = S_FALSE;
						}

						if ( l_hrOk == S_OK )
						{
							for ( unsigned long j = 0; j < l_uiDigCount; j++ )
							{
								unsigned long l_ulAcqIndex = 0;

								g_CIAcqCreate( l_iIndex, j, &l_ulAcqIndex );
							}
						}
					}
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		if ( l_hrOk != S_OK )
		{
			g_CIDestroy( true );
		}
	}

	return l_hrOk;
}

HRESULT g_CIDestroy( bool p_bClose )
{
	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &g_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &g_lRefCount, 0 );

		int i = 0;
		int l_iSize = 0;
		
		l_iSize = g_caAcqs.size();

		for ( i = l_iSize - 1; 0 <= i; i-- )
		{
			if ( g_CIAcqDestroy( i ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}

		g_caAcqs.clear();

		l_iSize = g_caBuffers.GetSize();

		for ( i = l_iSize - 1; 0 <= i; i-- )
		{
			if ( g_CIBufferDestroy( i ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			g_caBuffers.RemoveAt( i );
		}

		if ( ! g_FreeImageHandles.Destroy() )
		{
			l_hrOk = S_FALSE;
		}

		l_iSize = g_caServers.GetSize();

		for ( i = l_iSize - 1; 0 <= i; i-- )
		{
			l_hrOk = SVSaperaManager::Instance().ManReleaseServer( g_caServers[ i ].m_hCorServer );

			if( l_hrOk != S_OK )
			{
				l_hrOk = S_FALSE;
			}


			g_caServers.RemoveAt( i );
		}

		SVSaperaManager::Instance().Shutdown();
	}

	return l_hrOk;
}

HRESULT g_CIBufferCreate( unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, unsigned long p_ulType, unsigned long *p_pulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulIndex != NULL )
	{
		CORSERVER l_hCorServer = CORHANDLE_NULL;

		HRESULT l_Temp = SVSaperaManager::Instance().ManGetLocalServer( &l_hCorServer );

		if ( l_Temp == S_OK )
		{
			SVCIBufferStruct l_Buffer;

			l_Buffer.m_hCorServer = l_hCorServer;
			l_Buffer.m_ulWidth = p_ulWidth;
			l_Buffer.m_ulHeight = p_ulHeight;
			l_Buffer.m_ulFormat = p_ulFormat;
			l_Buffer.m_ulType = p_ulType;

			l_hrOk = SVSaperaManager::Instance().BufferNew( l_Buffer.m_hCorServer, 
																 l_Buffer.m_ulWidth,
																 l_Buffer.m_ulHeight,
																 l_Buffer.m_ulFormat,
																 l_Buffer.m_ulType,
																 &(l_Buffer.m_hCorBuffer) );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = SVSaperaManager::Instance().BufferGetPrm( l_Buffer.m_hCorBuffer, 
				                              CORBUFFER_PRM_ADDRESS, 
																			&(l_Buffer.m_pAddress) );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = SVSaperaManager::Instance().BufferGetPrm( l_Buffer.m_hCorBuffer, 
																				CORBUFFER_PRM_PITCH, 
																				&(l_Buffer.m_Pitch) );
				}

				if ( l_hrOk == S_OK )
				{
					if ( *p_pulIndex < (unsigned long)g_caBuffers.GetSize() )
					{
						l_hrOk = g_CIBufferDestroy( *p_pulIndex );
					}
					else
					{
						if ( g_FreeImageHandles.IsCreated() )
						{
							long l_lSize = 0;

							if ( g_FreeImageHandles.GetSize( l_lSize ) && 0 < l_lSize )
							{
								unsigned long l_ulTemp = 0;

								if ( g_FreeImageHandles.RemoveHead( (void **)&l_ulTemp ) )
								{
									*p_pulIndex = l_ulTemp;
								}
							}
						}
					}

					if ( l_hrOk == S_OK )
					{
						if ( *p_pulIndex < (unsigned long)g_caBuffers.GetSize() )
						{
							g_caBuffers[ *p_pulIndex ] = l_Buffer;
						}
						else
						{
							*p_pulIndex = (unsigned long)g_caBuffers.Add( l_Buffer );

							if ( *p_pulIndex >= (unsigned long)g_caBuffers.GetSize() )
							{
								l_hrOk = S_FALSE;
							}
						}
					}
				}

				if ( l_hrOk != S_OK )
				{
					SVSaperaManager::Instance().BufferFree( l_Buffer.m_hCorBuffer );
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIBufferCreate( BSTR p_bstrFileName, unsigned long *p_pulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulIndex != NULL )
	{
		CORSERVER l_hCorServer = CORHANDLE_NULL;

		HRESULT l_Temp = SVSaperaManager::Instance().ManGetLocalServer( &l_hCorServer );

		if ( l_Temp == S_OK )
		{
			CString l_csFileName = p_bstrFileName;

			if ( ! l_csFileName.IsEmpty() )
			{
				SVCIBufferStruct l_Buffer;

				char l_szDrive[_MAX_DRIVE];
				char l_szDir[_MAX_DIR];
				char l_szFileName[_MAX_FNAME];
				char l_szExt[_MAX_EXT];

				_splitpath( l_csFileName, l_szDrive, l_szDir, l_szFileName, l_szExt );

				if ( CString( l_szExt ).CompareNoCase( ".cvi" ) == 0 )
				{
					CORVIC l_hCorVic = CORHANDLE_NULL;

					l_hrOk = SVSaperaManager::Instance().VicNew( l_hCorServer, &l_hCorVic );

					if ( l_hrOk == S_OK )
					{
						l_hrOk = SVSaperaManager::Instance().VicLoad( l_hCorVic, l_csFileName );

						if ( l_hrOk == S_OK )
						{
							SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_CROP_WIDTH, &l_Buffer.m_ulWidth );
							SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_CROP_HEIGHT, &l_Buffer.m_ulHeight );
							SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_OUTPUT_FORMAT, &l_Buffer.m_ulFormat );
							
							switch ( l_Buffer.m_ulFormat )
							{
								case CORACQ_VAL_OUTPUT_FORMAT_RGB5551:
								case CORACQ_VAL_OUTPUT_FORMAT_RGB565:
								case CORACQ_VAL_OUTPUT_FORMAT_RGB888:
								case CORACQ_VAL_OUTPUT_FORMAT_RGB8888:
								case CORACQ_VAL_OUTPUT_FORMAT_RGB101010:
								case CORACQ_VAL_OUTPUT_FORMAT_RGB161616:
								case CORACQ_VAL_OUTPUT_FORMAT_UYVY:
								case CORACQ_VAL_OUTPUT_FORMAT_YUY2:
								case CORACQ_VAL_OUTPUT_FORMAT_YVYU:
								case CORACQ_VAL_OUTPUT_FORMAT_YUYV:
								case CORACQ_VAL_OUTPUT_FORMAT_Y411:
								case CORACQ_VAL_OUTPUT_FORMAT_Y211:
								{
									l_Buffer.m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_RGB8888;

									break;
								}
								case CORACQ_VAL_OUTPUT_FORMAT_MONO8:
								case CORACQ_VAL_OUTPUT_FORMAT_MONO16:
								case CORACQ_VAL_OUTPUT_FORMAT_MONO32:
								default:
								{
									l_Buffer.m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_MONO8;

									break;
								}
							}
						}

						l_Temp = SVSaperaManager::Instance().VicFree( l_hCorVic );

						if ( l_Temp != S_OK )
						{
							l_hrOk = S_FALSE;
						}
					}
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = g_CIBufferCreate( l_Buffer.m_ulWidth,
																     l_Buffer.m_ulHeight,
																     l_Buffer.m_ulFormat,
																     l_Buffer.m_ulType,
																     p_pulIndex );
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIBufferDestroy( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caBuffers.GetSize();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caBuffers[ p_ulIndex ].m_hCorBuffer ) )
		{
			HRESULT l_Temp = SVSaperaManager::Instance().BufferFree( g_caBuffers[ p_ulIndex ].m_hCorBuffer );

			if ( l_Temp != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			g_caBuffers[ p_ulIndex ].m_hCorBuffer = CORHANDLE_NULL;
			g_caBuffers[ p_ulIndex ].m_hCorServer = CORHANDLE_NULL;

			if ( g_FreeImageHandles.IsCreated() )
			{
				g_FreeImageHandles.AddTail( (void *)p_ulIndex );
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqCreate( unsigned long p_ulServerIndex, unsigned long p_ulAcqIndex, unsigned long *p_pulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iServerSize = g_caServers.GetSize();

	if ( p_ulServerIndex < (unsigned long)l_iServerSize )
	{
		CORACQ l_hCorAcq = CORHANDLE_NULL;

		l_hrOk = SVSaperaManager::Instance().AcqGetHandle( g_caServers[ p_ulServerIndex ].m_hCorServer, 
		                              p_ulAcqIndex, &l_hCorAcq );

		if ( l_hrOk == S_OK )
		{
			unsigned long l_ulPixelDepth = 0;
			CORSERVER l_hCorServer = CORHANDLE_NULL;

			SVCIAcqStruct l_Acq;

			l_Acq.m_lServerIndex = p_ulServerIndex;

			l_Acq.m_csName.Format( "Dig_%d", p_ulAcqIndex );
			l_Acq.m_csFullName.Format( "%s.%s", 
																 g_caServers[ l_Acq.m_lServerIndex ].m_csName,
																 l_Acq.m_csName );

			l_Acq.m_hCorServer = g_caServers[ l_Acq.m_lServerIndex ].m_hCorServer;

			l_Acq.m_hCorAcq = l_hCorAcq;

			SVSaperaManager::Instance().AcqSetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_EXT_TRIGGER_ENABLE, CORACQ_VAL_EXT_TRIGGER_ON );
			SVSaperaManager::Instance().AcqSetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_EXT_TRIGGER_DETECTION, CORACQ_VAL_FALLING_EDGE );

			SVSaperaManager::Instance().AcqGetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_CROP_WIDTH, &l_Acq.m_ulWidth );
			SVSaperaManager::Instance().AcqGetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_CROP_HEIGHT, &l_Acq.m_ulHeight );
			SVSaperaManager::Instance().AcqGetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_OUTPUT_FORMAT, &l_Acq.m_ulFormat );

			switch ( l_Acq.m_ulFormat )
			{
				case CORACQ_VAL_OUTPUT_FORMAT_RGB5551:
				case CORACQ_VAL_OUTPUT_FORMAT_RGB565:
				case CORACQ_VAL_OUTPUT_FORMAT_RGB888:
				case CORACQ_VAL_OUTPUT_FORMAT_RGB8888:
				case CORACQ_VAL_OUTPUT_FORMAT_RGB101010:
				case CORACQ_VAL_OUTPUT_FORMAT_RGB161616:
				case CORACQ_VAL_OUTPUT_FORMAT_UYVY:
				case CORACQ_VAL_OUTPUT_FORMAT_YUY2:
				case CORACQ_VAL_OUTPUT_FORMAT_YVYU:
				case CORACQ_VAL_OUTPUT_FORMAT_YUYV:
				case CORACQ_VAL_OUTPUT_FORMAT_Y411:
				case CORACQ_VAL_OUTPUT_FORMAT_Y211:
				{
					l_Acq.m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_RGB8888;

					break;
				}
				case CORACQ_VAL_OUTPUT_FORMAT_MONO8:
				case CORACQ_VAL_OUTPUT_FORMAT_MONO16:
				case CORACQ_VAL_OUTPUT_FORMAT_MONO32:
				default:
				{
					l_Acq.m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_MONO8;

					break;
				}
			}

			SVSaperaManager::Instance().AcqGetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_LUT_FORMAT, &l_Acq.m_ulLutFormat );
			SVSaperaManager::Instance().AcqGetPrm( l_Acq.m_hCorAcq, CORACQ_PRM_PIXEL_DEPTH, &l_ulPixelDepth );

			l_Acq.m_ulLutEntries = 1 << l_ulPixelDepth;

			l_hrOk = SVSaperaManager::Instance().ManGetLocalServer( &l_hCorServer );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = SVSaperaManager::Instance().LutNew( l_hCorServer, l_Acq.m_ulLutEntries, 
				                        l_Acq.m_ulLutFormat, &(l_Acq.m_hCorLut) );
			}

			if ( l_hrOk == S_OK )
			{
				SVCIAcqStructVector::iterator l_Iter = g_caAcqs.insert( g_caAcqs.end(), l_Acq );

				*p_pulIndex = std::distance( g_caAcqs.begin(), l_Iter );
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqLoadCameraFiles( unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			if ( g_CIAcqUnloadCameraFiles( p_ulIndex ) == S_OK )
			{
				CORSERVER l_hCorServer = CORHANDLE_NULL;

				HRESULT l_Temp = SVSaperaManager::Instance().ManGetLocalServer( &l_hCorServer );

				if ( l_Temp == S_OK )
				{
					// check safearray
					long lLBound, lUBound;
					SafeArrayGetLBound(p_psaFileNames, 1, &lLBound);
					SafeArrayGetUBound(p_psaFileNames, 1, &lUBound);

					// iterate safearray
					for ( long i=lLBound; i <= lUBound; i++)
					{
						BSTR bstrFileName = NULL;
						SafeArrayGetElementNoCopy(p_psaFileNames, &i, &bstrFileName);
						CString l_csFileName = bstrFileName;

						if ( ! l_csFileName.IsEmpty() )
						{
							SVCIBufferStruct l_Buffer;

							char l_szDrive[_MAX_DRIVE];
							char l_szDir[_MAX_DIR];
							char l_szFileName[_MAX_FNAME];
							char l_szExt[_MAX_EXT];

							_splitpath( l_csFileName, l_szDrive, l_szDir, l_szFileName, l_szExt );

							if ( CString( l_szExt ).CompareNoCase( ".cca" ) == 0 )
							{
								l_hrOk = g_LoadCCAFile(l_hCorServer, p_ulIndex, l_csFileName);
							}
							else if ( CString( l_szExt ).CompareNoCase( ".cvi" ) == 0 )
							{
								l_hrOk = g_LoadCVIFile(l_hCorServer, p_ulIndex, l_csFileName);
							}
							else if ( CString( l_szExt ).CompareNoCase( ".ccf" ) == 0 )
							{
								l_hrOk = g_LoadCCFFile(l_hCorServer, p_ulIndex, l_csFileName);
							}
						}// end if ( ! l_csFileName.IsEmpty() )
					}// end for each file in safearray
				}// end if ( SVSaperaManager::Instance().ManGetLocalServer( &l_hCorServer ) == S_OK )
			}// end if
		}// end if
	}// end if ( p_ulIndex < (unsigned long)l_iSize )

	if ( l_hrOk == S_OK )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) &&
		     ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorCam ) &&
			 ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorVic ) )
		{
			l_hrOk = SVSaperaManager::Instance().AcqReset( g_caAcqs[ p_ulIndex ].m_hCorAcq );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = SVSaperaManager::Instance().AcqSetPrms( g_caAcqs[ p_ulIndex ].m_hCorAcq, 
				                            g_caAcqs[ p_ulIndex ].m_hCorVic, 
										    g_caAcqs[ p_ulIndex ].m_hCorCam, 
										    TRUE );
			}

			if ( l_hrOk == S_OK )
			{
				HRESULT l_Temp = SVSaperaManager::Instance().AcqSetPrm( g_caAcqs[ p_ulIndex ].m_hCorAcq, 
				                  CORACQ_PRM_OUTPUT_FORMAT, 
									g_caAcqs[ p_ulIndex ].m_ulFormat );
			}

			if ( l_hrOk != S_OK )
			{
				SVSaperaManager::Instance().AcqReset( g_caAcqs[ p_ulIndex ].m_hCorAcq );
			}
		}
	}

	return l_hrOk;	
}

HRESULT g_CIAcqBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_Count = 18;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			l_hrOk = g_CIAcqBufferDestroyAll( p_ulIndex );

			if ( l_hrOk == S_OK )
			{
				CORSERVER l_hCorServer = CORHANDLE_NULL;

				l_hrOk = SVSaperaManager::Instance().ManGetLocalServer( &l_hCorServer );

				for ( unsigned long i = 0; l_hrOk == S_OK && i < l_Count; i++ )
				{
					unsigned long l_ulBufferIndex = (unsigned long) -1;

					l_hrOk = g_CIBufferCreate( g_caAcqs[ p_ulIndex ].m_ulWidth,
																		 g_caAcqs[ p_ulIndex ].m_ulHeight,
																		 g_caAcqs[ p_ulIndex ].m_ulFormat,
																		 g_caAcqs[ p_ulIndex ].m_ulType,
																		 &l_ulBufferIndex );

					if ( l_hrOk == S_OK )
					{
						if ( CORHANDLE_IS_NULL( g_caBuffers[ l_ulBufferIndex ].m_hCorBuffer ) )
						{
							l_hrOk = S_FALSE;
						}
						else
						{
							if ( CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorXfer ) )
							{
								l_hrOk = SVSaperaManager::Instance().XferNew( g_caAcqs[ p_ulIndex ].m_hCorServer, 
																				 g_caAcqs[ p_ulIndex ].m_hCorAcq, 
																				 g_caBuffers[ l_ulBufferIndex ].m_hCorBuffer, 
																				 NULL, 
																				 &(g_caAcqs[ p_ulIndex ].m_hCorXfer) );

								if ( l_hrOk == S_OK )

								{
									l_hrOk = SVSaperaManager::Instance().XferSetPrm( g_caAcqs[ p_ulIndex ].m_hCorXfer, 
																							CORXFER_PRM_CYCLE_MODE,
																							g_caAcqs[ p_ulIndex ].m_ulCycleMode );
								}
								/*  JMS - Use with Sapera 7.0
								if ( l_hrOk == S_OK )

								{
									l_hrOk = SVSaperaManager::Instance().XferSetPrm( g_caAcqs[ p_ulIndex ].m_hCorXfer, 
										CORXFER_PRM_BUFFER_TIMESTAMP_MODULE,
										CORXFER_VAL_BUFFER_TIMESTAMP_MODULE_XFER );
								}

								if ( l_hrOk == S_OK )

								{
									l_hrOk = SVSaperaManager::Instance().XferSetPrm( g_caAcqs[ p_ulIndex ].m_hCorXfer, 
										CORXFER_PRM_BUFFER_TIMESTAMP_EVENT,
										CORXFER_VAL_EVENT_TYPE_START_OF_FRAME );
								}
								*/
							}
							else
							{
								l_hrOk = SVSaperaManager::Instance().XferAppend( g_caAcqs[ p_ulIndex ].m_hCorXfer, 
																						g_caAcqs[ p_ulIndex ].m_hCorAcq, 
																						g_caBuffers[ l_ulBufferIndex ].m_hCorBuffer, 
																						NULL );
							}
						}

						if ( l_hrOk == S_OK )
						{
							SVCICallbackStruct *l_pCallback = new SVCICallbackStruct;
							if ( l_pCallback != NULL )
							{
								l_pCallback->m_ulDigitizerIndex = p_ulIndex;
								l_pCallback->m_ulBufferIndex = l_ulBufferIndex;
								l_pCallback->m_IsTrash = ( g_caAcqs[ p_ulIndex ].m_UsesTrashBuffer && ( ( l_Count - 1 ) == i ) );

								if ( (unsigned long)g_caAcqs[ p_ulIndex ].m_caBuffers.Add( l_pCallback ) != i )
								{
									l_hrOk = S_FALSE;
								}
							}
							else
							{
								l_hrOk = S_FALSE;
							}
						}

						if ( l_hrOk == S_OK )
						{
							l_hrOk = SVSaperaManager::Instance().XferRegisterCallback( g_caAcqs[ p_ulIndex ].m_hCorXfer, 
										                                CORXFER_VAL_EVENT_TYPE_START_OF_FRAME | 
										                                CORXFER_VAL_EVENT_TYPE_END_OF_FRAME,
										                                g_SVCIDigitizerCallback,
										                                g_caAcqs[ p_ulIndex ].m_caBuffers[ i ] );

						}
					}
				}

				if ( l_hrOk != S_OK )
				{
					g_CIAcqBufferDestroyAll( p_ulIndex );
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqTriggerRegister( unsigned long p_ulIndex, SVCallbackStruct &p_rCallback )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		SVCallbackListClass::iterator l_pos = g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.Find( p_rCallback );
		if ( l_pos != g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.end() )
		{
			l_hrOk = S_OK;
		}
		else
		{
			if ( ::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsTriggerRegistered), 1 ) == 0 )
			{
				if( !CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
				{
					l_hrOk = SVSaperaManager::Instance().AcqRegisterCallback( g_caAcqs[ p_ulIndex ].m_hCorAcq, 
																							 CORACQ_VAL_EVENT_TYPE_EXTERNAL_TRIGGER, 
																							 g_SVCITriggerCallback, 
																							 (void *)p_ulIndex );
				}

				if ( l_hrOk != S_OK )
				{
					::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsTriggerRegistered), 0 );
				}
			}
			else
			{
				l_hrOk = S_OK;
			}

			if ( l_hrOk == S_OK )
			{
				g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.AddTail( p_rCallback );
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long) l_iSize )
	{
		g_caAcqs[ p_ulIndex ].m_pBufferInterface = p_pInterface;
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT g_CIAcqStart( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long) l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorXfer ) )
			{
				if ( ::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsStarted), 1 ) == 0 )
				{
					unsigned long l_ThreadID;

					g_caAcqs[ p_ulIndex ].m_BufferIndexQueue.clear();

					if( g_caAcqs[ p_ulIndex ].mhShutdown != NULL )
					{
						::ResetEvent( g_caAcqs[ p_ulIndex ].mhShutdown );
					}
					else
					{
						g_caAcqs[ p_ulIndex ].mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );
					}

					g_caAcqs[ p_ulIndex ].mhThread = CreateThread( NULL, 0, SVSaperaEndFrameThreadProc, new SVSaperaEndFrameThreadParam( p_ulIndex ), 0, &l_ThreadID );

					::SetThreadPriority( g_caAcqs[ p_ulIndex ].mhThread, THREAD_PRIORITY_NORMAL );

					l_hrOk = SVSaperaManager::Instance().XferConnect( g_caAcqs[ p_ulIndex ].m_hCorXfer );

					if ( l_hrOk == S_OK )
					{
						l_hrOk = SVSaperaManager::Instance().XferStart( g_caAcqs[ p_ulIndex ].m_hCorXfer,
																			 CORXFER_CONTINUOUS );
					}

					if ( l_hrOk != S_OK )
					{
						::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsStarted), 0 );
					}
				}
				else
				{
					l_hrOk = S_OK;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqStop( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			if ( ::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsStarted), 0 ) == 1 )
			{
				if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorXfer ) )
				{
					HRESULT l_Temp = SVSaperaManager::Instance().XferStop( g_caAcqs[ p_ulIndex ].m_hCorXfer );

					if ( l_Temp != S_OK )
					{
						l_Temp = SVSaperaManager::Instance().XferWait( g_caAcqs[ p_ulIndex ].m_hCorXfer, 1000 );

						if ( l_Temp != S_OK )
						{
							l_hrOk = S_FALSE;
						}
					}
					
					if ( l_hrOk != S_OK )
					{
						l_hrOk = SVSaperaManager::Instance().XferAbort( g_caAcqs[ p_ulIndex ].m_hCorXfer );
					}

					l_Temp = SVSaperaManager::Instance().XferDisconnect( g_caAcqs[ p_ulIndex ].m_hCorXfer );

					if ( l_Temp != S_OK )
					{
						l_hrOk = S_FALSE;
					}
				}

				if( g_caAcqs[ p_ulIndex ].mhShutdown != NULL )
				{
					::SetEvent( g_caAcqs[ p_ulIndex ].mhShutdown );
				}

				if( g_caAcqs[ p_ulIndex ].mhThread != NULL )
				{
					::CloseHandle( g_caAcqs[ p_ulIndex ].mhThread );
					g_caAcqs[ p_ulIndex ].mhThread = NULL;
				}

				g_caAcqs[ p_ulIndex ].m_BufferIndexQueue.clear();
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqUnregisterBufferInterface( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long) l_iSize )
	{
		g_caAcqs[ p_ulIndex ].m_pBufferInterface = NULL;
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT g_CIAcqTriggerUnregister( unsigned long p_ulIndex, SVCallbackStruct &p_rCallback )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		SVCallbackListClass::iterator l_pos = g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.Find( p_rCallback );
		if ( l_pos != g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.end() )
		{
			g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.RemoveAt( l_pos );

			if ( 0 == g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.GetCount() )
			{
				if ( ::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsTriggerRegistered), 0 ) == 1 )
				{
					if( !CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
					{
						HRESULT l_Temp = SVSaperaManager::Instance().AcqUnregisterCallback( g_caAcqs[ p_ulIndex ].m_hCorAcq, g_SVCITriggerCallback );

						if ( l_Temp != S_OK )
						{
							l_hrOk = S_FALSE;
						}
					}
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqTriggerUnregisterAll( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		g_caAcqs[ p_ulIndex ].m_svTriggerCallbacks.RemoveAll();

		if ( ::InterlockedExchange( &(g_caAcqs[ p_ulIndex ].m_lIsTriggerRegistered), 0 ) == 1 )
		{
			if( !CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
			{
				HRESULT l_Temp = SVSaperaManager::Instance().AcqUnregisterCallback( g_caAcqs[ p_ulIndex ].m_hCorAcq, g_SVCITriggerCallback );

				if ( l_Temp != S_OK )
				{
					l_hrOk = S_FALSE;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT g_CIAcqTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long* p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT g_CIAcqTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR* p_bstrName )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT g_CIAcqTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT g_CIAcqTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT g_CIAcqBufferDestroyAll( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorXfer ) )
			{
				if ( g_CIAcqStop( p_ulIndex ) != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				HRESULT l_Temp = SVSaperaManager::Instance().XferUnregisterCallback( g_caAcqs[ p_ulIndex ].m_hCorXfer, g_SVCIDigitizerCallback );

				if ( l_Temp != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				l_Temp = SVSaperaManager::Instance().XferReset( g_caAcqs[ p_ulIndex ].m_hCorXfer );

				if ( l_Temp != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				l_Temp = SVSaperaManager::Instance().XferFree( g_caAcqs[ p_ulIndex ].m_hCorXfer );

				if ( l_Temp != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				g_caAcqs[ p_ulIndex ].m_hCorXfer = CORHANDLE_NULL;
			}

			int l_iBufferSize = g_caAcqs[ p_ulIndex ].m_caBuffers.GetSize();

			for ( int i = l_iBufferSize - 1; 0 <= i; i--)
			{
				SVCICallbackStruct *l_pCallback = g_caAcqs[ p_ulIndex ].m_caBuffers[ i ];

				g_caAcqs[ p_ulIndex ].m_caBuffers.RemoveAt( i );

				if ( l_pCallback != NULL )
				{
					if ( g_CIBufferDestroy( l_pCallback->m_ulBufferIndex ) != S_OK )
					{
						l_hrOk = S_FALSE;
					}
					delete l_pCallback;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT g_CIAcqUnloadCameraFiles( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorCam ) )
		{
			HRESULT l_Temp = SVSaperaManager::Instance().CamFree( g_caAcqs[ p_ulIndex ].m_hCorCam );

			if ( l_Temp != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			g_caAcqs[ p_ulIndex ].m_hCorCam = CORHANDLE_NULL;
		}

		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorVic ) )
		{
			HRESULT l_Temp = SVSaperaManager::Instance().VicFree( g_caAcqs[ p_ulIndex ].m_hCorVic );

			if ( l_Temp != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			g_caAcqs[ p_ulIndex ].m_hCorVic = CORHANDLE_NULL;
		}
	}
	return l_hrOk;	
}

HRESULT g_CIAcqDestroy( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorLut ) )
			{
				HRESULT l_Temp = SVSaperaManager::Instance().LutFree( g_caAcqs[ p_ulIndex ].m_hCorLut );

				if ( l_Temp != S_OK )
				{
					l_hrOk = S_FALSE;
				}

				g_caAcqs[ p_ulIndex ].m_hCorLut = CORHANDLE_NULL;
			}

			if ( g_CIAcqTriggerUnregisterAll( p_ulIndex ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			if ( g_CIAcqBufferDestroyAll( p_ulIndex ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			if ( g_CIAcqUnloadCameraFiles( p_ulIndex ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			HRESULT l_Temp = SVSaperaManager::Instance().AcqRelease( g_caAcqs[ p_ulIndex ].m_hCorAcq );

			if ( l_Temp != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			g_caAcqs[ p_ulIndex ].m_hCorAcq = CORHANDLE_NULL;
			g_caAcqs[ p_ulIndex ].m_hCorServer = CORHANDLE_NULL;
			g_caAcqs[ p_ulIndex ].m_csFullName.Empty();
			g_caAcqs[ p_ulIndex ].m_csName.Empty();
			g_caAcqs[ p_ulIndex ].m_lServerIndex = -1;
		}

		if( g_caAcqs[ p_ulIndex ].mhShutdown != NULL )
		{
			::SetEvent( g_caAcqs[ p_ulIndex ].mhShutdown );
			::CloseHandle( g_caAcqs[ p_ulIndex ].mhShutdown );
			g_caAcqs[ p_ulIndex ].mhShutdown = NULL;
		}

		if( g_caAcqs[ p_ulIndex ].mhThread != NULL )
		{
			::CloseHandle( g_caAcqs[ p_ulIndex ].mhThread );
			g_caAcqs[ p_ulIndex ].mhThread = NULL;
		}
	}
	return l_hrOk;
}

HRESULT g_CIAcqGetLightReference( unsigned long p_ulIndex, unsigned long p_ulType, long &p_rlValue )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	p_rlValue = 0;

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			UINT32 l_ui32Type = (UINT32) SVLightReferenceTypeUnknown;

			switch( p_ulType )
			{
				case SVLightReferenceTypeBrightness:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightness;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeBrightnessRed:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessRed;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeBrightnessGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessGreen;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeBrightnessBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessBlue;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrast:
				{
					l_ui32Type = SVCorLightReferenceTypeContrast;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrastRed:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastRed;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrastGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastGreen;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrastBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastBlue;

					l_hrOk = S_OK;

					break;
				}
				case SVCorLightReferenceTypeBrightness:
				case SVCorLightReferenceTypeBrightnessRed:
				case SVCorLightReferenceTypeBrightnessGreen:
				case SVCorLightReferenceTypeBrightnessBlue:
				case SVCorLightReferenceTypeContrast:
				case SVCorLightReferenceTypeContrastRed:
				case SVCorLightReferenceTypeContrastGreen:
				case SVCorLightReferenceTypeContrastBlue:
				{
					l_ui32Type = (UINT32) p_ulType;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					l_hrOk = S_FALSE;

					break;
				}
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = SVSaperaManager::Instance().AcqGetPrm( g_caAcqs[ p_ulIndex ].m_hCorAcq, l_ui32Type, &p_rlValue );

				if ( l_hrOk == S_OK )
				{
					p_rlValue /= 10L;
				}// end else
			}
		}
	}
	return l_hrOk;
}

HRESULT g_CIAcqSetLightReference( unsigned long p_ulIndex, unsigned long p_ulType, long p_lValue )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) )
		{
			UINT32 l_ui32Type = (UINT32) SVLightReferenceTypeUnknown;

			switch( p_ulType )
			{
				case SVLightReferenceTypeBrightness:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightness;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeBrightnessRed:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessRed;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeBrightnessGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessGreen;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeBrightnessBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessBlue;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrast:
				{
					l_ui32Type = SVCorLightReferenceTypeContrast;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrastRed:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastRed;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrastGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastGreen;

					l_hrOk = S_OK;

					break;
				}
				case SVLightReferenceTypeContrastBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastBlue;

					l_hrOk = S_OK;

					break;
				}
				case SVCorLightReferenceTypeBrightness:
				case SVCorLightReferenceTypeBrightnessRed:
				case SVCorLightReferenceTypeBrightnessGreen:
				case SVCorLightReferenceTypeBrightnessBlue:
				case SVCorLightReferenceTypeContrast:
				case SVCorLightReferenceTypeContrastRed:
				case SVCorLightReferenceTypeContrastGreen:
				case SVCorLightReferenceTypeContrastBlue:
				{
					l_ui32Type = (UINT32) p_ulType;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					l_hrOk = S_FALSE;

					break;
				}
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = SVSaperaManager::Instance().AcqSetPrm( g_caAcqs[ p_ulIndex ].m_hCorAcq, l_ui32Type, p_lValue * 10 );
			}
		}
	}
	return l_hrOk;
}

HRESULT g_CIAcqGetLightReference( unsigned long p_ulIndex, SVLightReference& lr )
{
	HRESULT l_hrOk = S_OK;
	CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
	CORACQ hcorLut = g_caAcqs[ p_ulIndex ].m_hCorLut;

	// RGB  Brightness    -2462 ... 2371
	// RGB  Contrast       8024 ... 15305
	// Quad Brightness    -6700 ... 800
	// Quad Contrast       8400 ... 10900

	ULONG ulBands;
	HRESULT l_Temp = SVSaperaManager::Instance().LutGetPrm( hcorLut, CORLUT_PRM_NPAGES, &ulBands );

	lr.Create( ulBands );
	if( ulBands == 1 )
	{
		// Create space for brightness and constrast
		lr.Band( 0 ).Create( 2 );
		lr.Band( 0 ).Attribute( 0 ).dwType = SVLightReferenceTypeBrightness;
		lr.Band( 0 ).Attribute( 0 ).strName = _T( "Brightness" );
		lr.Band( 0 ).Attribute( 0 ).iIDCaption = 10054; // IDS_BRIGHTNESS_REF
		lr.Band( 0 ).Attribute( 0 ).lMin = -6700;
		lr.Band( 0 ).Attribute( 0 ).lMax = 800;
		lr.Band( 0 ).Attribute( 0 ).lStep = 1;

		lr.Band( 0 ).Attribute( 1 ).dwType = SVLightReferenceTypeContrast;
		lr.Band( 0 ).Attribute( 1 ).strName = _T( "Contrast" );
		lr.Band( 0 ).Attribute( 1 ).iIDCaption = 10055; // IDS_CONTRAST_REF
		lr.Band( 0 ).Attribute( 1 ).lMin = 8400;
		lr.Band( 0 ).Attribute( 1 ).lMax = 10900;
		lr.Band( 0 ).Attribute( 1 ).lStep = 1;
	}// end if
	else if( ulBands == 3 )
	{
		// Create space for brightness and constrast
		lr.Band( 0 ).Create( 2 );
		lr.Band( 0 ).Attribute( 0 ).dwType = SVLightReferenceTypeBrightnessRed;
		lr.Band( 0 ).Attribute( 0 ).strName = _T( "Brightness" );
		lr.Band( 0 ).Attribute( 0 ).iIDCaption = 10054; // IDS_BRIGHTNESS_REF
		lr.Band( 0 ).Attribute( 0 ).lMin = -2462;
		lr.Band( 0 ).Attribute( 0 ).lMax = 2371;
		lr.Band( 0 ).Attribute( 0 ).lStep = 1;

		//lr.Band( 0 ).Attribute( 1 ).dwType = SVCorLightReferenceTypeContrastRed;
		lr.Band( 0 ).Attribute( 1 ).dwType = SVLightReferenceTypeContrastRed;
		lr.Band( 0 ).Attribute( 1 ).strName = _T( "Contrast" );
		lr.Band( 0 ).Attribute( 1 ).iIDCaption = 10055; // IDS_CONTRAST_REF
		lr.Band( 0 ).Attribute( 1 ).lMin = 8024;
		lr.Band( 0 ).Attribute( 1 ).lMax = 15305;
		lr.Band( 0 ).Attribute( 1 ).lStep = 1;

		lr.Band( 1 ).Create( 2 );
		lr.Band( 1 ).Attribute( 0 ).dwType = SVLightReferenceTypeBrightnessGreen;
		lr.Band( 1 ).Attribute( 0 ).strName = _T( "Brightness" );
		lr.Band( 1 ).Attribute( 0 ).iIDCaption = 10054; // IDS_BRIGHTNESS_REF
		lr.Band( 1 ).Attribute( 0 ).lMin = -2462;
		lr.Band( 1 ).Attribute( 0 ).lMax = 2371;
		lr.Band( 1 ).Attribute( 0 ).lStep = 1;

		//lr.Band( 1 ).Attribute( 1 ).dwType = SVCorLightReferenceTypeContrastGreen;
		lr.Band( 1 ).Attribute( 1 ).dwType = SVLightReferenceTypeContrastGreen;
		lr.Band( 1 ).Attribute( 1 ).strName = _T( "Contrast" );
		lr.Band( 1 ).Attribute( 1 ).iIDCaption = 10055; // IDS_CONTRAST_REF
		lr.Band( 1 ).Attribute( 1 ).lMin = 8024;
		lr.Band( 1 ).Attribute( 1 ).lMax = 15305;
		lr.Band( 1 ).Attribute( 1 ).lStep = 1;

		lr.Band( 2 ).Create( 2 );
		lr.Band( 2 ).Attribute( 0 ).dwType = SVLightReferenceTypeBrightnessBlue;
		lr.Band( 2 ).Attribute( 0 ).strName = _T( "Brightness" );
		lr.Band( 2 ).Attribute( 0 ).iIDCaption = 10054; // IDS_BRIGHTNESS_REF
		lr.Band( 2 ).Attribute( 0 ).lMin = -2462;
		lr.Band( 2 ).Attribute( 0 ).lMax = 2371;
		lr.Band( 2 ).Attribute( 0 ).lStep = 1;

		//lr.Band( 2 ).Attribute( 1 ).dwType = SVCorLightReferenceTypeContrastBlue;
		lr.Band( 2 ).Attribute( 1 ).dwType = SVLightReferenceTypeContrastBlue;
		lr.Band( 2 ).Attribute( 1 ).strName = _T( "Contrast" );
		lr.Band( 2 ).Attribute( 1 ).iIDCaption = 10055; // IDS_CONTRAST_REF
		lr.Band( 2 ).Attribute( 1 ).lMin = 8024;
		lr.Band( 2 ).Attribute( 1 ).lMax = 15305;
		lr.Band( 2 ).Attribute( 1 ).lStep = 1;
	}// end else

	for( int iBand = 0; iBand < lr.NumBands(); iBand++)
	{
		for( int iAttr = 0; iAttr < lr.Band(iBand).NumAttributes(); iAttr++)
		{
			SVLightReferenceAttributeInfo& attr = lr.Band(iBand).Attribute(iAttr);

			UINT32 l_ui32Type = (UINT32) SVLightReferenceTypeUnknown;

			switch( attr.dwType )
			{
				case SVLightReferenceTypeBrightness:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightness;
					break;
				}
				case SVLightReferenceTypeBrightnessRed:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessRed;
					break;
				}
				case SVLightReferenceTypeBrightnessGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessGreen;
					break;
				}
				case SVLightReferenceTypeBrightnessBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessBlue;
					break;
				}
				case SVLightReferenceTypeContrast:
				{
					l_ui32Type = SVCorLightReferenceTypeContrast;
					break;
				}
				case SVLightReferenceTypeContrastRed:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastRed;
					break;
				}
				case SVLightReferenceTypeContrastGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastGreen;
					break;
				}
				case SVLightReferenceTypeContrastBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastBlue;
					break;
				}
				case SVCorLightReferenceTypeBrightness:
				case SVCorLightReferenceTypeBrightnessRed:
				case SVCorLightReferenceTypeBrightnessGreen:
				case SVCorLightReferenceTypeBrightnessBlue:
				case SVCorLightReferenceTypeContrast:
				case SVCorLightReferenceTypeContrastRed:
				case SVCorLightReferenceTypeContrastGreen:
				case SVCorLightReferenceTypeContrastBlue:
				{
					l_ui32Type = (UINT32) attr.dwType;
					break;
				}
			}

			l_Temp = SVSaperaManager::Instance().AcqGetPrm( hcorAcq, l_ui32Type, &attr.lValue );

			if ( l_Temp != S_OK )
			{				
				l_hrOk = S_FALSE;
			}
			else
			{
				attr.lValue = attr.lValue / 10L;
			}// end else
		}// end for
	}// end for
	return l_hrOk;
}

HRESULT g_CIAcqSetLightReference( unsigned long p_ulIndex, const SVLightReference& lr )
{
	HRESULT l_hrOk = S_OK;

	CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
	for ( int iBand = 0; iBand < lr.NumBands(); iBand++)
	{
		for ( int iAttr = 0; iAttr < lr.Band(iBand).NumAttributes(); iAttr++)
		{
			const SVLightReferenceAttributeInfo& attr = lr.Band(iBand).Attribute(iAttr);

			UINT32 l_ui32Type = ( UINT32 )SVLightReferenceTypeUnknown;

			switch ( attr.dwType )
			{
				case SVLightReferenceTypeBrightness:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightness;
					break;
				}
				case SVLightReferenceTypeBrightnessRed:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessRed;
					break;
				}
				case SVLightReferenceTypeBrightnessGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessGreen;
					break;
				}
				case SVLightReferenceTypeBrightnessBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeBrightnessBlue;
					break;
				}
				case SVLightReferenceTypeContrast:
				{
					l_ui32Type = SVCorLightReferenceTypeContrast;
					break;
				}
				case SVLightReferenceTypeContrastRed:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastRed;
					break;
				}
				case SVLightReferenceTypeContrastGreen:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastGreen;
					break;
				}
				case SVLightReferenceTypeContrastBlue:
				{
					l_ui32Type = SVCorLightReferenceTypeContrastBlue;
					break;
				}
				case SVCorLightReferenceTypeBrightness:
				case SVCorLightReferenceTypeBrightnessRed:
				case SVCorLightReferenceTypeBrightnessGreen:
				case SVCorLightReferenceTypeBrightnessBlue:
				case SVCorLightReferenceTypeContrast:
				case SVCorLightReferenceTypeContrastRed:
				case SVCorLightReferenceTypeContrastGreen:
				case SVCorLightReferenceTypeContrastBlue:
				{
					l_ui32Type = ( UINT32 ) attr.dwType;
					break;
				}
			}

			HRESULT l_Temp = SVSaperaManager::Instance().AcqSetPrm( hcorAcq, l_ui32Type, attr.lValue * 10 );

			if ( l_Temp != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqGetLut( unsigned long p_ulIndex, VARIANT &p_rvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) &&
		     ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorLut ) )
		{
			CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
			CORHANDLE hLut = g_caAcqs[ p_ulIndex ].m_hCorLut;

			ULONG ulElementSize;
			ULONG ulBands;
			ULONG ulSizeOfBuffer;
			ULONG ulFormat;

			l_hrOk = ::VariantClear( &p_rvarValue );

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().AcqGetPrm(hcorAcq, CORACQ_PRM_LUT_FORMAT, &ulFormat);
			}

			if (l_hrOk == S_OK)
			{
				switch ( ulFormat )
				{
					case CORLUT_VAL_FORMAT_MONO8:
					case CORLUT_VAL_FORMAT_COLORNI10:
						{
							break;
						}
					default:
						{
							l_hrOk = S_FALSE;
							break;
						}
				}
			}

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_SIZE, &ulSizeOfBuffer);
			}

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_NPAGES, &ulBands);
			}

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_DATASIZE, &ulElementSize);
			}

			if (l_hrOk == S_OK)
			{
				UINT uiBandSize = ulSizeOfBuffer / ulElementSize / ulBands;

				CByteArray aRawBytes;
				aRawBytes.SetSize(ulSizeOfBuffer);

				l_hrOk = SVSaperaManager::Instance().LutRead(hLut, 0, aRawBytes.GetData(), ulSizeOfBuffer);

				if (l_hrOk == S_OK)
				{
					SAFEARRAYBOUND l_psabData[2];
					long l_plIndex[2];

					l_psabData[0].cElements = ulBands;
					l_psabData[0].lLbound = 0;
					l_psabData[1].cElements = uiBandSize;
					l_psabData[1].lLbound = 0;

					p_rvarValue.vt = VT_ARRAY | VT_I4;
					p_rvarValue.parray = SafeArrayCreate( VT_I4, 3, l_psabData );

					for( unsigned long j = 0; j <= ulBands; j++ )
					{
						for( unsigned long i = 0; i < uiBandSize; i++)
						{
							long l_lValue = 0;

							switch ( ulElementSize )
							{
								case 1:
								{
									char **l_ppArray = (char **)(aRawBytes.GetData());

									l_lValue = l_ppArray[j][i];

									break;
								}
								case 2:
								{
									short **l_ppArray = (short **)(aRawBytes.GetData());

									l_lValue = l_ppArray[j][i];

									break;
								}
							}

							l_plIndex[ 0 ] = j;
							l_plIndex[ 1 ] = i;

							::SafeArrayPutElement( p_rvarValue.parray, l_plIndex, (void *) &l_lValue );
						}
					}
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqSetLut( unsigned long p_ulIndex, VARIANT &p_rvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	int l_iSize = g_caAcqs.size();

	if ( p_ulIndex < (unsigned long)l_iSize )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorAcq ) &&
		     ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorLut ) )
		{
			CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
			CORHANDLE hLut = g_caAcqs[ p_ulIndex ].m_hCorLut;

			ULONG ulElementSize;
			ULONG ulBands;
			ULONG ulSizeOfBuffer;
			ULONG ulFormat;

			l_hrOk = SVSaperaManager::Instance().AcqGetPrm(hcorAcq, CORACQ_PRM_LUT_FORMAT, &ulFormat);

			if (l_hrOk == S_OK)
			{
				switch ( ulFormat )
				{
					case CORLUT_VAL_FORMAT_MONO8:
					case CORLUT_VAL_FORMAT_COLORNI10:
						{
							break;
						}
					default:
						{
							l_hrOk = S_FALSE;
							break;
						}
				}
			}

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_SIZE, &ulSizeOfBuffer);
			}

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_NPAGES, &ulBands);
			}

			if (l_hrOk == S_OK)
			{
				l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_DATASIZE, &ulElementSize);
			}

			if (l_hrOk == S_OK)
			{
				UINT uiBandSize = ulSizeOfBuffer / ulElementSize / ulBands;

				CByteArray aRawBytes;
				aRawBytes.SetSize(ulSizeOfBuffer);

				SAFEARRAYBOUND l_psabData[2];
				long l_plIndex[2];

				l_psabData[0].cElements = ulBands;
				l_psabData[0].lLbound = 0;
				l_psabData[1].cElements = uiBandSize;
				l_psabData[1].lLbound = 0;

				p_rvarValue.vt = VT_ARRAY | VT_I4;
				p_rvarValue.parray = SafeArrayCreate( VT_I4, 3, l_psabData );

				for( unsigned long j = 0; j <= ulBands; j++ )
				{
					for( unsigned long i = 0; i < uiBandSize; i++)
					{
						long l_lValue = 0;

						l_plIndex[ 0 ] = j;
						l_plIndex[ 1 ] = i;

						::SafeArrayGetElement( p_rvarValue.parray, l_plIndex, (void *) &l_lValue );

						switch ( ulElementSize )
						{
							case 1:
							{
								char **l_ppArray = (char **)(aRawBytes.GetData());

								l_ppArray[j][i] = (char)l_lValue;

								break;
							}
							case 2:
							{
								short **l_ppArray = (short **)(aRawBytes.GetData());

								l_ppArray[j][i] = (short)l_lValue;

								break;
							}
						}
					}
				}

				l_hrOk = SVSaperaManager::Instance().LutWrite( hLut, 0, (void*) aRawBytes.GetData(), aRawBytes.GetSize());

				if ( l_hrOk == S_OK )
				{
					l_hrOk = SVSaperaManager::Instance().AcqSetLut( hcorAcq, hLut, 0);
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqGetLut( unsigned long p_ulIndex, SVLut& lut )
{
	HRESULT l_hrOk = S_OK;

	CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
	if ( !CORHANDLE_IS_NULL( hcorAcq ) )
	{
		CORHANDLE hLut = g_caAcqs[ p_ulIndex ].m_hCorLut;

		ULONG ulFormat;
		l_hrOk = SVSaperaManager::Instance().AcqGetPrm(hcorAcq, CORACQ_PRM_LUT_FORMAT, &ulFormat);

		ULONG ulLutFormat;
		l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_FORMAT, &ulLutFormat);

		ULONG ulPixelDepth;
		HRESULT l_Temp = SVSaperaManager::Instance().AcqGetPrm(hcorAcq, CORACQ_PRM_PIXEL_DEPTH, &ulPixelDepth);

		ULONG ulNumEntries = 1 << ulPixelDepth;

		ASSERT( ulFormat == ulLutFormat );

		ULONG ulSizeOfBuffer;
		l_Temp = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_SIZE, &ulSizeOfBuffer);

		ULONG ulBands;
		l_Temp = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_NPAGES, &ulBands);

		ULONG ulEntries;
		l_Temp = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_NENTRIES, &ulEntries);

		ULONG ulElementSize;
		l_Temp = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_DATASIZE, &ulElementSize);

		ULONG ulSign;
		l_Temp = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_SIGNED, &ulSign);

		CByteArray aRawBytes;
		aRawBytes.SetSize(ulSizeOfBuffer);

		UINT uiBandSize = ulSizeOfBuffer / ulElementSize / ulBands;
		if (lut.Info().Format() != ulFormat || lut.Info().BandSize() != uiBandSize)
		{
			SVLutInfo info(lut.Info());
			info.SetFormat( ulFormat );
			info.SetBandSize( uiBandSize );
			info.SetBands( ulBands );
			info.SetMaxValue( uiBandSize );

			lut.Create( info );
		}

		l_hrOk = SVSaperaManager::Instance().LutRead(hLut, 0, aRawBytes.GetData(), ulSizeOfBuffer);

		if (l_hrOk == S_OK)
		{
			// translate Acquisition device format into Neutral format..
			l_hrOk = g_CIAcqRawLutToNeutralLut(aRawBytes, lut);
		}
	}

	return l_hrOk;
}

HRESULT g_CIAcqSetLut( unsigned long p_ulIndex, const SVLut& lut )
{
	HRESULT l_hrOk = S_OK;

	CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
	CORHANDLE hLut = g_caAcqs[ p_ulIndex ].m_hCorLut;

	ULONG ulSizeOfBuffer=0;
	l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_SIZE, &ulSizeOfBuffer);

	ULONG ulElementSize=0;
	l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_DATASIZE, &ulElementSize);

	ULONG ulBands=0;
	l_hrOk = SVSaperaManager::Instance().LutGetPrm(hLut, CORLUT_PRM_NPAGES, &ulBands);

	// translate Neutral format into Acquisition device format...
	CByteArray aRawBytes;
	l_hrOk = g_CIAcqNeutralLutToRawLut(lut, aRawBytes);

	ASSERT( aRawBytes.GetSize() == ulSizeOfBuffer );
//	ASSERT( aRawBytes.GetSize() == ulElementSize * ulBands * ulSizeOfBuffer );

	l_hrOk = SVSaperaManager::Instance().LutWrite( hLut, 0, (void*) aRawBytes.GetData(), aRawBytes.GetSize());

	l_hrOk = SVSaperaManager::Instance().AcqSetLut( hcorAcq, hLut, 0);

	return l_hrOk;
}

HRESULT g_CIAcqNeutralLutToRawLut(const SVLut& lut, CByteArray& aRawBytes)
{
	HRESULT l_hrOk = S_OK;
	switch (lut.Info().Format())
	{
		case CORLUT_VAL_FORMAT_MONO8:
			{
				UINT uiSize = lut.Info().BandSize();
				aRawBytes.SetSize(uiSize);
				for (UINT i = 0; i < uiSize; i++)
				{
					aRawBytes[i] = lut(0, i);
				}
			}
			break;
		case CORLUT_VAL_FORMAT_COLORNI10:
			{
				const UINT uiSizeOfElement = 2;
				const UINT uiNumBands = 3;
				UINT uiBandSize = lut.Info().BandSize();
				UINT uiRawBandSize = uiBandSize * uiSizeOfElement;
				UINT uiRawSize = uiRawBandSize * uiNumBands;
				aRawBytes.SetSize(uiRawSize);
				for(UINT uiBand=0; uiBand < 3; uiBand++)
				{
					for (UINT uiElement=0; uiElement < uiBandSize; uiElement++)
					{
						WORD wVal = lut(uiBand, uiElement);
						// store little endian
						*((WORD*)&(aRawBytes.GetData()[uiBand*uiRawBandSize+uiElement*uiSizeOfElement])) = wVal;
						//aRawBytes[uiBand*uiRawSize+uiElement*uiSizeOfElement] = uiVal % 256;
						//aRawBytes[uiBand*uiRawSize+uiElement*uiSizeOfElement+1] = (uiVal >> 8) % 256;
					}
				}
			}
			break;
		case CORLUT_VAL_FORMAT_COLORNI8:
			#if defined(_DEBUG) || defined(_MINDEBUG)
			AfxMessageBox("Lut Set Color NI 8");
			#endif
			break;
		case CORLUT_VAL_FORMAT_COLORI10:
			#if defined(_DEBUG) || defined(_MINDEBUG)
			AfxMessageBox("Lut Set Color I 10");
			#endif
			break;
		case CORLUT_VAL_FORMAT_COLORI8:
			#if defined(_DEBUG) || defined(_MINDEBUG)
			AfxMessageBox("Lut Set Color I 8");
			#endif
			break;
		default:
			break;
	}
	return l_hrOk;
}

HRESULT g_CIAcqRawLutToNeutralLut(const CByteArray& aRawBytes, SVLut& lut)
{
	HRESULT l_hrOk = S_OK;
	switch( lut.Info().Format() )
	{
		case CORLUT_VAL_FORMAT_MONO8:
			{
				UINT uiSize = aRawBytes.GetSize();
				ASSERT( uiSize == lut.Info().BandSize() );
				for (UINT i = 0; i < uiSize; i++)
				{
					lut(0, i) = aRawBytes.GetAt(i);
				}
			}
			break;
		case CORLUT_VAL_FORMAT_COLORNI10:
			{
				const UINT uiSizeOfElement = 2;
				const UINT uiNumBands = 3;
				UINT uiRawSize = aRawBytes.GetSize();
				UINT uiRawBandSize = uiRawSize / uiNumBands;
				UINT uiBandSize = uiRawBandSize / uiSizeOfElement;
				for(UINT uiBand=0; uiBand < uiNumBands; uiBand++)
				{
					for (UINT uiElement=0; uiElement < uiBandSize; uiElement++)
					{
						// stored as little endian
						WORD wVal = *((WORD*)&(aRawBytes.GetData()[uiBand*uiRawBandSize+uiElement*uiSizeOfElement]));
						//aRawBytes[uiBand*uiRawSize+uiElement*uiSizeOfElement] = uiVal % 256;
						//aRawBytes[uiBand*uiRawSize+uiElement*uiSizeOfElement+1] = (uiVal >> 8) % 256;
						lut(uiBand, uiElement) = wVal;
					}
				}
			}
			break;
		case CORLUT_VAL_FORMAT_COLORNI8:
			#if defined(_DEBUG) || defined(_MINDEBUG)
			AfxMessageBox("Lut Get Color NI 8");
			#endif
			break;
		case CORLUT_VAL_FORMAT_COLORI10:
			#if defined(_DEBUG) || defined(_MINDEBUG)
			AfxMessageBox("Lut Get Color I 10");
			#endif
			break;
		case CORLUT_VAL_FORMAT_COLORI8:
			#if defined(_DEBUG) || defined(_MINDEBUG)
			AfxMessageBox("Lut Get Color I 8");
			#endif
			break;
		default:
			break;
	}
	return l_hrOk;
}

HRESULT g_CIAcqEnableSoftwareTrigger( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;
	CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;

	if ( !CORHANDLE_IS_NULL( hcorAcq ) )
	{
		l_hrOk = SVSaperaManager::Instance().AcqSetPrm( hcorAcq, CORACQ_PRM_EXT_TRIGGER_ENABLE, CORACQ_VAL_EXT_TRIGGER_ON);
	}
	return l_hrOk;
}

HRESULT g_CIAcqSoftwareTrigger( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	CORACQ hcorAcq = g_caAcqs[ p_ulIndex ].m_hCorAcq;
	if ( !CORHANDLE_IS_NULL( hcorAcq ) )
	{
		l_hrOk = SVSaperaManager::Instance().AcqSoftwareTrigger(hcorAcq, CORACQ_VAL_SOFTWARE_TRIGGER_EXT);
	}
	return l_hrOk;
}

HRESULT g_LoadCCAFile(CORSERVER p_hCorServer, unsigned long p_ulIndex, LPCTSTR p_csFileName)
{
	CORCAM l_hCorCam = CORHANDLE_NULL;
	HRESULT l_hrOk = SVSaperaManager::Instance().CamNew( p_hCorServer, &l_hCorCam );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = SVSaperaManager::Instance().CamLoad( l_hCorCam, p_csFileName );
	}

	if ( l_hrOk == S_OK )
	{
		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorCam ) )
		{
			l_hrOk = SVSaperaManager::Instance().CamFree( g_caAcqs[ p_ulIndex ].m_hCorCam );

			g_caAcqs[ p_ulIndex ].m_hCorCam = CORHANDLE_NULL;
		}

		if ( l_hrOk == S_OK )
		{
			g_caAcqs[ p_ulIndex ].m_hCorCam = l_hCorCam;
		}
	}
	return l_hrOk;
}

HRESULT g_LoadCVIFile(CORSERVER p_hCorServer, unsigned long p_ulIndex, LPCTSTR p_csFileName)
{
	CORVIC l_hCorVic = CORHANDLE_NULL;

	HRESULT l_hrOk = SVSaperaManager::Instance().VicNew( p_hCorServer, &l_hCorVic );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = SVSaperaManager::Instance().VicLoad( l_hCorVic, p_csFileName );
	}

	if ( l_hrOk == S_OK )
	{
		unsigned long l_ulCount = 0;

		HRESULT l_Temp = SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_EXT_TRIGGER_FRAME_COUNT, &l_ulCount );

		if ( l_ulCount < 1 )
		{
			l_ulCount = 1;
			l_Temp = SVSaperaManager::Instance().VicSetPrm( l_hCorVic, CORACQ_PRM_EXT_TRIGGER_FRAME_COUNT, l_ulCount );
		}

		l_Temp = SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_CROP_WIDTH, &g_caAcqs[ p_ulIndex ].m_ulWidth );
		
		l_Temp = SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_CROP_HEIGHT, &g_caAcqs[ p_ulIndex ].m_ulHeight );
		
		l_Temp = SVSaperaManager::Instance().VicGetPrm( l_hCorVic, CORACQ_PRM_OUTPUT_FORMAT, &g_caAcqs[ p_ulIndex ].m_ulFormat );

		switch ( g_caAcqs[ p_ulIndex ].m_ulFormat )
		{
			case CORACQ_VAL_OUTPUT_FORMAT_RGB5551:
			case CORACQ_VAL_OUTPUT_FORMAT_RGB565:
			case CORACQ_VAL_OUTPUT_FORMAT_RGB888:
			case CORACQ_VAL_OUTPUT_FORMAT_RGB8888:
			case CORACQ_VAL_OUTPUT_FORMAT_RGB101010:
			case CORACQ_VAL_OUTPUT_FORMAT_RGB161616:
			case CORACQ_VAL_OUTPUT_FORMAT_UYVY:
			case CORACQ_VAL_OUTPUT_FORMAT_YUY2:
			case CORACQ_VAL_OUTPUT_FORMAT_YVYU:
			case CORACQ_VAL_OUTPUT_FORMAT_YUYV:
			case CORACQ_VAL_OUTPUT_FORMAT_Y411:
			case CORACQ_VAL_OUTPUT_FORMAT_Y211:
			{
				g_caAcqs[ p_ulIndex ].m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_RGB8888;
				break;
			}
			case CORACQ_VAL_OUTPUT_FORMAT_MONO8:
			case CORACQ_VAL_OUTPUT_FORMAT_MONO16:
			case CORACQ_VAL_OUTPUT_FORMAT_MONO32:
			default:
			{
				g_caAcqs[ p_ulIndex ].m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_MONO8;
				break;
			}
		}

		if ( ! CORHANDLE_IS_NULL( g_caAcqs[ p_ulIndex ].m_hCorVic ) )
		{
			l_hrOk = SVSaperaManager::Instance().VicFree( g_caAcqs[ p_ulIndex ].m_hCorVic );

			g_caAcqs[ p_ulIndex ].m_hCorVic = CORHANDLE_NULL;
		}

		if ( l_hrOk == S_OK )
		{
			g_caAcqs[ p_ulIndex ].m_hCorVic = l_hCorVic;
		}
	}
	return l_hrOk;
}

HRESULT g_LoadCCFFile(CORSERVER p_hCorServer, unsigned long p_ulIndex, LPCTSTR p_csFileName)
{
	HRESULT l_hrOk = g_LoadCCAFile( p_hCorServer, p_ulIndex, p_csFileName );
	if ( l_hrOk == S_OK )
	{
		l_hrOk = g_LoadCVIFile( p_hCorServer, p_ulIndex, p_csFileName );
	}
	return l_hrOk;
}

