// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekFireStackClass
// * .File Name       : $Workfile:   SVIntekFireStackClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 14:48:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekFireStackClass.h"

#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVIntek.h"
#include "SVConvert.h"
#include "SVCFireBus.h"
#include "SVStatusLibrary/SVException.h"
#include "FCTLEnable.h"
#include "SVMessage/SVMessage.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVTimerLibrary\SVClock.h"
#include "SVTriggerLibrary/SVTriggerActivatorFunc.h"
#include "SVTriggerLibrary/SVTriggerCallbackFunc.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void SVCFireBusCallback( FBCBINFO *p_pInfo )
{
	SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_HIGHEST );

  CString l_csbuf;
	
  switch( p_pInfo->Reason )
  {
		case FBN_CREATEDEVICE:
			{
				FBNCREATE* l_pFBNCreate = reinterpret_cast< FBNCREATE* >( p_pInfo->pData );

				l_csbuf.Format("%s - Board %s",
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_pFireBus->GetReasonString(p_pInfo->Reason), 
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_csFireStackName);
				TRACE( "%s\n", l_csbuf );

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				break;
			}
			
		case FBN_NEWDEVICE:
			{
				if(((CFireNode*)p_pInfo->pData)->Type()==FNT_DCAM)
				{
					CCamera* pCamera = (CCamera*)p_pInfo->pData;
					SVIntekBoardStruct* pBoardStruct = (SVIntekBoardStruct*)p_pInfo->pContext;
					bool bCameraReconnect = false;
					g_svTheApp.m_svSystem.CameraCreate( pBoardStruct, pCamera, bCameraReconnect );

					if (bCameraReconnect)
					{
						unsigned long ulIndex = reinterpret_cast<unsigned long>(pCamera->GetUser());
						g_svTheApp.m_svSystem.ReloadCameraParams(ulIndex, pCamera);
					}
					l_csbuf.Format("New Device");
					TRACE( "%s\n", l_csbuf );
				}
				break;
			}
			
    case FBN_KILLDEVICE:
			{
				if(((CFireNode*)p_pInfo->pData)->Type()==FNT_DCAM)
				{
					g_svTheApp.m_svSystem.CameraDestroy( ((CCamera*)p_pInfo->pData) );

					l_csbuf.Format("Kill Device");
					TRACE( "%s\n", l_csbuf );
				}

				break;
			}
			
    case FBN_FRAME_START:
			{
				// Do StartFrame & EndFrame Logic
				CCamera *l_pCamera = (CCamera *)(p_pInfo->pData);
				unsigned long l_ulIndex = (unsigned long)(l_pCamera->GetUser());

				g_svTheApp.m_svSystem.CameraProcessStartFrame( l_ulIndex );

				/*
				l_csbuf.Format("Process Start Frame Callback - Camera %d", l_ulIndex );
				TRACE( "%s\n", l_csbuf );
				*/

				break;
			}
			
    case FBN_FRAME:
			{
				// Do StartFrame & EndFrame Logic
				CCamera *l_pCamera = (CCamera *)(p_pInfo->pData);
				unsigned long l_ulIndex = (unsigned long)(l_pCamera->GetUser());

				g_svTheApp.m_svSystem.CameraAPCProcessEndFrame( l_ulIndex );

				/*
				l_csbuf.Format("Process End Frame Callback - Camera %d", l_ulIndex );
				TRACE( "%s\n", l_csbuf );
				*/

				break;
			}
			
    case FBN_ERROR:
			{
				l_csbuf.Format("E: Errorcode is %d",*(UINT32*)p_pInfo->pData);
				TRACE( "%s\n", l_csbuf );
				// Log Message

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				break;
			}
			
    case FBN_FLAGGEDERROR:
			{
				l_csbuf.Format("E: Errorflags are %08X",*(UINT32*)p_pInfo->pData);
				TRACE( "%s\n", l_csbuf );
				// Log Message

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				break;
			}
		case FBN_BUSRESETBEGIN:
			{
				l_csbuf.Format("%s - Board %s",
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_pFireBus->GetReasonString(p_pInfo->Reason), 
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_csFireStackName);
				TRACE( "%s\n", l_csbuf );
				// Log Message

				((SVIntekBoardStruct*)(p_pInfo->pContext))->m_bFirstBusResetComplete = false;
				((SVIntekBoardStruct*)(p_pInfo->pContext))->m_bFirstBusResetStarted = true;

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				break;
			}
		case FBN_BUSRESETEND:
			{
				l_csbuf.Format("%s - Board %s",
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_pFireBus->GetReasonString(p_pInfo->Reason), 
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_csFireStackName);
				TRACE( "%s\n", l_csbuf );
				// Log Message

				((SVIntekBoardStruct*)(p_pInfo->pContext))->m_bFirstBusResetComplete = true;
				((SVIntekBoardStruct*)(p_pInfo->pContext))->m_bFirstBusResetStarted = false;

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				break;
			}
    default:
			{
				l_csbuf.Format("%s - Board %s",
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_pFireBus->GetReasonString(p_pInfo->Reason), 
					((SVIntekBoardStruct*)(p_pInfo->pContext))->m_csFireStackName);
				TRACE( "%s\n", l_csbuf );
				// Log Message

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				break;
			}
  }
}

DWORD WINAPI SVCFireBusThreadProc( LPVOID p_lpParam )
{
	HRESULT l_hrOk = S_FALSE;

	FBSYSINFO l_fbSysInfo;

	if ( p_lpParam != NULL && GetSysInfo( &l_fbSysInfo ) )
	{
		SVIntekBoardStruct *l_psvBoard = (SVIntekBoardStruct *)p_lpParam;

		CFireBus *l_pFireBus = NULL;

		try
		{
			l_pFireBus = new CFireBus();

			if( l_pFireBus != NULL )
			{
				FBCARDID l_ulCardId = l_psvBoard->m_ulCardId;

				l_psvBoard->m_pFireBus = l_pFireBus;

				unsigned int l_uiCardIndex = l_fbSysInfo.CardCount;

				for ( unsigned long i = 0; i < l_fbSysInfo.CardCount; i++ )
				{
					if ( l_ulCardId.AsUINT32 == l_fbSysInfo.CardId[i].AsUINT32 )
					{
						l_uiCardIndex = i;

						break;
					}
				}

				if ( l_uiCardIndex < l_fbSysInfo.CardCount )
				{
					FBPARMS ArgOpen;

					ArgOpen.BusAddress=0x3FF;
					ArgOpen.AsyRxPktCnt=32;
					ArgOpen.AsyRxPktPayload=2048;
					ArgOpen.AsyTxPktCnt=32;
					ArgOpen.AsyTxPktPayload=2048;
					ArgOpen.PowerClass=0;
					ArgOpen.StoredAccessCnt=16;
					ArgOpen.IrqTaskPrio=230;
					ArgOpen.IrqPrio=6;
					ArgOpen.IrmBmTaskPrio=229;
					ArgOpen.pCfgRom=(UINT32*)NULL;
					ArgOpen.CfgRomLength=0;

					// SEJ - enable Packet level error detection
					ArgOpen.SpecialFlags=FCTLSPEC_USERFRAMES | FCTLSPEC_FRMSTARTEVT | FCTLSPEC_USEISODMAMODE;
					//ArgOpen.SpecialFlags=FCTLSPEC_USERFRAMES | FCTLSPEC_FRMSTARTEVT;

					try
					{
						l_pFireBus->Run( l_uiCardIndex, SVCFireBusCallback, &ArgOpen, l_psvBoard );
					}
					catch( ... )
					{
						TRACE("Error: CFireBus->Run -- Card Index %u\n", l_uiCardIndex );

						l_hrOk = S_FALSE;
					}
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}
		}
		catch( ... )
		{
			l_hrOk = S_FALSE;
		}

		if( l_pFireBus != NULL )
		{
			delete l_pFireBus;
		}
	}

	return l_hrOk;
}

DWORD WINAPI SVCFireBusThreadOpenProc( LPVOID p_lpParam )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_lpParam != NULL )
	{
		SVIntekFireStackClass *l_psvFireStack = (SVIntekFireStackClass *)p_lpParam;

		l_hrOk = l_psvFireStack->ThreadOpen();
	}

	return l_hrOk;
}

VOID CALLBACK SVIntekEndFrameAPCProc( ULONG_PTR dwParam )
{
}

struct SVIntekEndFrameThreadParam
{
	SVIntekEndFrameThreadParam( SVIntekFireStackClass* p_pSystem, long p_CameraIndex )
	:	m_pSystem( p_pSystem ), m_CameraIndex( p_CameraIndex ) {}

	SVIntekFireStackClass* m_pSystem;
	long m_CameraIndex;
};

DWORD WINAPI SVIntekFireStackClass::SVIntekEndFrameThreadProc( LPVOID p_lpParam )
{
	// Perform Queue Servicing
	HRESULT hrOk = S_FALSE;

	SVIntekEndFrameThreadParam *pItem = NULL;

	pItem = (SVIntekEndFrameThreadParam *)p_lpParam;

	if ( pItem )
	{
		HANDLE hShutdown = NULL;

		if ( DuplicateHandle( GetCurrentProcess(), pItem->m_pSystem->m_caCameras[ pItem->m_CameraIndex ].mhShutdown,
													GetCurrentProcess(), &hShutdown,
													0, FALSE,	DUPLICATE_SAME_ACCESS ) )
		{
			hrOk = S_OK;

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
							pItem->m_pSystem->CameraProcessEndFrame( pItem->m_CameraIndex );

							long l_NextBuffer = pItem->m_pSystem->m_caCameras[ pItem->m_CameraIndex ].m_NextProcessBuffer;

							l_Process = pItem->m_pSystem->m_caCameras[ pItem->m_CameraIndex ].m_caBuffers[ l_NextBuffer ].m_bIsLocked;
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

		delete pItem;
	}

	return hrOk;
}

/*
This constructor initializes all local attributes to desired defaults.
*/
SVIntekFireStackClass::SVIntekFireStackClass()
{
	m_lRefCount = 0;
}

/*
This destructor call the Close method before the object get destructed.
*/
SVIntekFireStackClass::~SVIntekFireStackClass()
{
	Close();
}

/*
This method will increment the reference count and initialize the Intek Fire Stack system, if the reference count is zero,  Otherwise, this method will just increment the reference count.
*/
HRESULT SVIntekFireStackClass::Create()
{
	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &m_lRefCount ) == 1 )
	{
		CString l_csbuf;
		SVException l_svLog;

		FBSYSINFO l_fbSysInfo;

		BOOL l_bDone = FALSE;

		for( long l = 0; ! l_bDone && l < 10000; l++ )
		{
			l_bDone = GetSysInfo( &l_fbSysInfo ) && 
				0 < l_fbSysInfo.CardCount &&
				l_fbSysInfo.CardCount == m_caBoards.GetSize();
		}

		if( ! l_bDone )
		{
			l_hrOk = S_FALSE;
		}

		if( l_hrOk == S_OK )
		{
			int l_iSize = m_caBoards.GetSize();

			for ( int i = 0; i < l_iSize ; i++ )
			{
				bool l_bSendReset = true;

				SVIntekBoardStruct *l_psvBoard = NULL;

				l_csbuf.Format( "SVIntekFireStackClass::Create - Wait for Bus Reset Start" );
				// Log Message

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				for( int k = 0; k < 1000; k++ )
				{
					if ( l_psvBoard == NULL )
					{
						l_psvBoard = m_caBoards[i];

						if( l_psvBoard != NULL )
						{
							l_csbuf.Format("SVIntekFireStackClass::Create - Wait for Bus Reset Start - Board %s",
								l_psvBoard->m_csFireStackName );
							// Log Message

							SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
							l_svLog.LogException( l_csbuf );
						}
					}
					else
					{
						if ( l_psvBoard->m_bFirstBusResetStarted )
						{
							l_csbuf.Format("SVIntekFireStackClass::Create - Bus Reset Started - Board %s",
								l_psvBoard->m_csFireStackName );
							// Log Message

							SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
							l_svLog.LogException( l_csbuf );

							break;
						}
					}

					::Sleep( 1 );
				}

				l_csbuf.Format("SVIntekFireStackClass::Create - Wait for Bus Reset End - Board %s",
					l_psvBoard->m_csFireStackName );
				// Log Message

				SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				if ( l_psvBoard != NULL && l_psvBoard->m_bFirstBusResetStarted )
				{
					for( int j = 0; j < 120000; j++ )
					{
						if ( l_psvBoard->m_bFirstBusResetComplete )
						{
							l_csbuf.Format("SVIntekFireStackClass::Create - Bus Reset Endded - Board %s",
								l_psvBoard->m_csFireStackName );
							// Log Message

							SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
							l_svLog.LogException( l_csbuf );

							break;
						}

						::Sleep( 1 );
					}
				}
			}
		}
	}

	m_triggerMgr.Create(_T("Intek Fire Stack"), SVAffinityAcq);

	return l_hrOk;
}

/*
This method will decrement the reference count.  If the reference equals zero or the p_bClose parameter is true, this method will shutdown the Intek Fire Stack system.
*/
HRESULT SVIntekFireStackClass::Destroy( bool p_bClose )
{
	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &m_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &m_lRefCount, 0 );

		m_cameraParamsMgr.Clear();
	}
	m_triggerMgr.Destroy();

	return l_hrOk;
}

/*
This method will iterate through the available resource in the Intek Fire Stack system and add them to the internal attribute of this class.
*/
HRESULT SVIntekFireStackClass::Open()
{
	HRESULT l_hrOk = S_OK;

	InitTables();

	FBSYSINFO l_fbSysInfo;

	BOOL l_bDone = FALSE;

	for( long l = 0; ! l_bDone && l < 1000; l++ )
	{
		l_bDone = GetSysInfo( &l_fbSysInfo ) && 0 < l_fbSysInfo.CardCount;
	}

	if( ! l_bDone )
	{
		l_hrOk = S_FALSE;
	}

	if ( l_hrOk == S_OK )
	{
		CString l_csbuf;
		SVException l_svLog;

		l_csbuf.Format("Card Count %d", l_fbSysInfo.CardCount );
		SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
		l_svLog.LogException( l_csbuf );

		for ( unsigned long i = 0; l_hrOk == S_OK && i < l_fbSysInfo.CardCount; i++ )
		{
			SVIntekBoardStruct *l_psvBoard = new SVIntekBoardStruct;

			if ( l_psvBoard != NULL )
			{
				l_psvBoard->m_csFireStackName.Format( "%s-%d-%d-%d",
					l_fbSysInfo.CardId[i].AsBytes.HardwareType==0?"PciLynx":"OhciLynx",
					l_fbSysInfo.CardId[i].AsBytes.BusNumber,
					l_fbSysInfo.CardId[i].AsBytes.DeviceNumber,
					l_fbSysInfo.CardId[i].AsBytes.FunctionNumber );

				l_psvBoard->m_csName = "Matrox_1394";

				l_psvBoard->m_ulCardId = l_fbSysInfo.CardId[i];

				int l_iIndex = m_caBoards.Add( l_psvBoard );

				if ( l_iIndex < 0 )
				{
					l_hrOk = S_FALSE;
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if ( l_hrOk == S_OK )
	{
		unsigned long l_ulThreadID = 0;

		HANDLE l_hThread = ::CreateThread( NULL, 0, SVCFireBusThreadOpenProc, 
							   (LPVOID)this, 0, &l_ulThreadID );

		::CloseHandle( l_hThread );
	}
	else
	{
		Close();
	}

	return l_hrOk;
}

/*
This method is used to wait for the Intek Fire Stack system to complete the intialize with the hardware before updating the internal attributes of this class.
*/
HRESULT SVIntekFireStackClass::ThreadOpen()
{
	HRESULT l_hrOk = S_OK;

	CString l_csbuf;
	SVException l_svLog;

	for ( unsigned long i = 0; l_hrOk == S_OK && i < static_cast<unsigned long>(m_caBoards.GetSize()); i++ )
	{
		SVIntekBoardStruct *l_psvBoard = m_caBoards[ i ];

		if ( l_psvBoard != NULL )
		{
			unsigned long l_ulThreadID = 0;

			l_psvBoard->m_hThread = ::CreateThread( NULL, 0, SVCFireBusThreadProc, 
			                       (LPVOID)l_psvBoard, 0, &l_ulThreadID );

			::SetThreadPriority( l_psvBoard->m_hThread, THREAD_PRIORITY_NORMAL );

			l_csbuf.Format("SVIntekFireStackClass::Open - Thread Created - Board %s",
				l_psvBoard->m_csFireStackName );
			// Log Message

			SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
			l_svLog.LogException( l_csbuf );

			l_csbuf.Format("SVIntekFireStackClass::Open - Wait for Bus Reset Start - Board %s",
				l_psvBoard->m_csFireStackName );
			// Log Message

			SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
			l_svLog.LogException( l_csbuf );

			for( int k = 0; k < 100000; k++ )
			{
				if ( l_psvBoard->m_bFirstBusResetStarted )
				{
					l_csbuf.Format("SVIntekFireStackClass::Open - Bus Reset Started - Board %s",
						l_psvBoard->m_csFireStackName );
					// Log Message

					SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
					l_svLog.LogException( l_csbuf );

					break;
				}

				Sleep( 1 );
			}

			l_csbuf.Format("SVIntekFireStackClass::Open - Wait for Bus Reset End - Board %s",
				l_psvBoard->m_csFireStackName );
			// Log Message

			SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
			l_svLog.LogException( l_csbuf );

			if ( l_psvBoard->m_bFirstBusResetStarted )
			{
				for( int j = 0; j < 120000; j++ )
				{
					if ( l_psvBoard->m_bFirstBusResetComplete )
					{
						l_csbuf.Format("SVIntekFireStackClass::Open - Bus Reset Endded - Board %s",
							l_psvBoard->m_csFireStackName );
						// Log Message

						SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
						l_svLog.LogException( l_csbuf );

						break;
					}

					Sleep( 1 );
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if ( l_hrOk != S_OK )
	{
		Close();
	}

	return l_hrOk;
}

/*
This method is used to shutdown all used resources and delete all attributes and resources.
*/
HRESULT SVIntekFireStackClass::Close()
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = Destroy( true );

	int l_iSize = m_caBoards.GetSize();

	for ( int i = l_iSize - 1; 0 <= i; i-- )
	{
		SVIntekBoardStruct *l_psvBoard = m_caBoards[i];

		m_caBoards.RemoveAt( i );

		try
		{
			l_psvBoard->m_pFireBus->Stop();

			::WaitForSingleObject( l_psvBoard->m_hThread, 10000 );

			delete l_psvBoard;
		}
		catch( ... )
		{
		}
	}

	return l_hrOk;
}

/*
This method is used to copy the processing handles into the processing thread.
*/
HRESULT SVIntekFireStackClass::GetStatusHandles( unsigned long &p_rulSize, HANDLE **p_pphArray )
{
	HRESULT hrOk = S_FALSE;

	if ( m_hThread != NULL && m_hShutdown != NULL )
	{
		p_rulSize = 2;

		*p_pphArray = new HANDLE[p_rulSize];
		if ( *p_pphArray )
		{
			
			if ( DuplicateHandle( GetCurrentProcess(), m_hThread,
														GetCurrentProcess(), (*p_pphArray) + 0,
														0, FALSE,	DUPLICATE_SAME_ACCESS ) )
			{
				if ( DuplicateHandle( GetCurrentProcess(), m_hShutdown,
															GetCurrentProcess(), (*p_pphArray) + 1,
															0, FALSE,	DUPLICATE_SAME_ACCESS ) )
				{
					hrOk = S_OK;
				}
				else
				{
					CloseHandle( (*p_pphArray)[0] );
				}
			}

			if ( hrOk != S_OK )
			{
				delete[] *p_pphArray;
				*p_pphArray = NULL;
			}
		}
	}

	return hrOk;
}

/*
This method is used to create image buffers based on the parameter data and add them to a centralized image pool.
*/
HRESULT SVIntekFireStackClass::BufferCreate( unsigned long p_ulWidth, unsigned long p_ulHeight, int p_iFormat, SVIntekBufferStruct &p_rBuffer )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulBytesPerPixel = 0;
	unsigned long l_ulColorEntries = 0;

	p_rBuffer.m_ulWidth = p_ulWidth;
	p_rBuffer.m_ulHeight = p_ulHeight;

	p_rBuffer.m_iFormat = p_iFormat;

	// Get number of bits per pixel
	switch( p_rBuffer.m_iFormat )
	{
		case SVImageFormatMono8:
		{
			l_ulBytesPerPixel=1;
			l_ulColorEntries=256;

			l_hrOk = S_OK;
			break;
		}
		case SVImageFormatMono16:
		case SVImageFormatRGB888:
		case SVImageFormatRGB8888:
		case SVImageFormatRGB161616:
		case SVImageFormatY411:
		case SVImageFormatY422:
		case SVImageFormatY444:
		{
			l_ulBytesPerPixel=4;
			l_ulColorEntries=0;

			l_hrOk = S_OK;
			break;
		}
		default:
		{
			break;
		}
	}

	unsigned long l_ulRowWidth = p_ulWidth * l_ulBytesPerPixel;
	unsigned long l_ulRowPadding = 0;
	
	//Add row padding for WinNT style systems up to long bonudary
	if( l_ulRowWidth % sizeof(long) != 0 )
	{
		l_ulRowPadding = sizeof(long) - ( l_ulRowWidth ) % sizeof(long);
	}

	unsigned long l_ulBufferSize = ( l_ulRowWidth + l_ulRowPadding ) * p_ulHeight;
	unsigned long l_ulBitmapInfoSize = sizeof(BITMAPINFOHEADER) + l_ulColorEntries * sizeof(RGBQUAD);

	if ( l_hrOk == S_OK )
	{
		p_rBuffer.m_pucImage = new unsigned char[ l_ulBufferSize ];
		p_rBuffer.m_pBitmapInfo = (BITMAPINFO*)( new unsigned char[ l_ulBitmapInfoSize ] );

		if ( p_rBuffer.m_pBitmapInfo == NULL || p_rBuffer.m_pucImage == NULL )
		{
			l_hrOk = S_FALSE;
		}
	}

	if ( l_hrOk == S_OK )
	{
		// Set info header
		p_rBuffer.m_pBitmapInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		p_rBuffer.m_pBitmapInfo->bmiHeader.biWidth=p_ulWidth;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biHeight=-(LONG)p_ulHeight;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biPlanes=1;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biBitCount=(UINT16)(l_ulBytesPerPixel*8);
		p_rBuffer.m_pBitmapInfo->bmiHeader.biCompression=BI_RGB;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biSizeImage=l_ulBufferSize;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biXPelsPerMeter=0;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biYPelsPerMeter=0;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biClrUsed=l_ulColorEntries;
		p_rBuffer.m_pBitmapInfo->bmiHeader.biClrImportant=0;

		memset( p_rBuffer.m_pucImage, 0, l_ulBufferSize );
	}

	return l_hrOk;
}

/*
This method is used to destroy image buffers from the centralized image pool.
*/
HRESULT SVIntekFireStackClass::BufferDestroy( SVIntekBufferStruct &p_rBuffer )
{
	HRESULT l_hrOk = S_OK;

	if ( p_rBuffer.m_pucImage != NULL )
	{
		try
		{
			delete[] p_rBuffer.m_pucImage;
		}
		catch( ... )
		{
			l_hrOk = S_FALSE;
		}

		p_rBuffer.m_pucImage = NULL;
	}

	if ( p_rBuffer.m_pBitmapInfo != NULL )
	{
		try
		{
			delete[] (char *)(p_rBuffer.m_pBitmapInfo);
		}
		catch( ... )
		{
			l_hrOk = S_FALSE;
		}

		p_rBuffer.m_pBitmapInfo = NULL;
	}

	return l_hrOk;
}

/*
This method is used to respond to a Intek camera create callback.  This method will construct and populate the appropriate camera object, and add it to the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraCreate( SVIntekBoardStruct *p_pBoard, CCamera *p_pCamera, bool& bNewCamera )
{
	HRESULT l_hrOk = S_OK;

	CString l_csCameraName;

	SVIntekCameraIdUnion l_svCameraId;

	l_svCameraId.m_ui64CameraId = 0;

	try
	{
		if( ! p_pCamera->ReadQuad(0xFFFF, 0xF000040C, &( l_svCameraId.m_ul040Ch ) ) ||
		    ! p_pCamera->ReadQuad(0xFFFF, 0xF0000410, &( l_svCameraId.m_ul0410h ) ) )
		{
			FNGUID* l_pGuid = p_pCamera->pGuid();

			if( l_pGuid != NULL )
			{
				l_svCameraId.m_ul040Ch = l_pGuid->Vendor << 8 | l_pGuid->ChipHigh;
				l_svCameraId.m_ul0410h = l_pGuid->ChipLow;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	catch( ... )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		unsigned long l_ulSize = m_caCameras.GetSize();
		unsigned long l_ulNewIndex = l_ulSize;
		unsigned long i( 0 );

		for ( i = 0; l_ulNewIndex == l_ulSize && i < l_ulSize; i++ )
		{
			if ( m_caCameras[i].m_svCameraId.m_ui64CameraId == l_svCameraId.m_ui64CameraId )
			{
				l_ulNewIndex = i;
				bNewCamera = true; // indicates reconnected camera
				break;
			}
		}

		for ( i = 0; l_ulNewIndex == l_ulSize && i < l_ulSize; i++ )
		{
			if ( m_caCameras[i].m_pCamera == NULL )
			{
				l_ulNewIndex = i;
			}
		}

		if ( l_ulNewIndex < l_ulSize )
		{
			m_caCameras[l_ulNewIndex].m_pCamera = p_pCamera;
		}
		else
		{
			SVIntekCameraStruct l_Camera;

			l_Camera.m_pCamera = p_pCamera;
			l_Camera.m_svCameraId = l_svCameraId;

			l_ulNewIndex = (unsigned long)m_caCameras.Add( l_Camera );

			// SEJ - track camera ID
			TrackCameraParams(l_ulNewIndex); 
		}

		try
		{
			/*
			m_caCameras[l_ulNewIndex].m_csName.Format( "Camera-%d-%d", p_pCamera->BusAddress(), p_pCamera->NodeAddress() );
			//*/
			m_caCameras[l_ulNewIndex].m_csName.Format( "Dig_%d", l_ulNewIndex );
			m_caCameras[l_ulNewIndex].m_csFullName.Format( "%s.%s", p_pBoard->m_csName, m_caCameras[l_ulNewIndex].m_csName );
			m_caCameras[l_ulNewIndex].m_pCamera->SetUser( (void *)l_ulNewIndex );

			l_csCameraName = m_caCameras[l_ulNewIndex].m_csFullName;

			CameraUpdateFormatModeRateColorFormat( l_ulNewIndex );

			p_pCamera->Lock();

			p_pCamera->Unlock();

			if ( m_caCameras[l_ulNewIndex].m_lIsStarted == 1 )
			{
				if( ! m_caCameras[l_ulNewIndex].m_pCamera->IsoActive() )
				{
					for( size_t i = 0; i < static_cast< size_t >( m_caCameras[l_ulNewIndex].m_caBuffers.GetSize() ); ++i )
					{
						m_caCameras[l_ulNewIndex].m_caBuffers[ i ].m_bIsLocked = false;
					}

					m_caCameras[l_ulNewIndex].m_NextCopyBuffer = 0;
					m_caCameras[l_ulNewIndex].m_NextProcessBuffer = 0;

					unsigned long l_ThreadID;

					if( m_caCameras[l_ulNewIndex].mhShutdown != NULL )
					{
						::ResetEvent( m_caCameras[l_ulNewIndex].mhShutdown );
					}
					else
					{
						m_caCameras[l_ulNewIndex].mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );
					}

					m_caCameras[l_ulNewIndex].mhThread = CreateThread( NULL, 0, SVIntekEndFrameThreadProc, new SVIntekEndFrameThreadParam( this, l_ulNewIndex ), 0, &l_ThreadID );

					::SetThreadPriority( m_caCameras[l_ulNewIndex].mhThread, THREAD_PRIORITY_NORMAL );

					CameraUpdateFormatModeRateColorFormat( l_ulNewIndex );

					// Reset frame counter
					m_caCameras[l_ulNewIndex].m_pCamera->ClearFrameCounter();

					// SEJ - Not using UserFrames (or GetAsyncKeyState)
					m_caCameras[l_ulNewIndex].m_pCamera->UseUserFrames(false);

 					if ( m_caCameras[l_ulNewIndex].m_pCamera->StartIso() )
					{
						l_hrOk = S_OK;
					}
				}
			}
		}
		catch( ... )
		{
			l_hrOk = S_FALSE;
		}
	}

	CString l_csbuf;
	SVException l_svLog;

	l_csbuf.Format("Create Camera = %s - S/N = 0x%08X%08X - HRESULT = 0x%08X", 
								 l_csCameraName, l_svCameraId.m_ul040Ch,l_svCameraId.m_ul0410h, l_hrOk );
	SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
	l_svLog.LogException( l_csbuf );

	return l_hrOk;
}

/*
This method is used to respond to a Intek camera destroy callback.  This method will destroy the appropriate camera object, and remove it to the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraDestroy( CCamera *p_pCamera )
{
	HRESULT l_hrOk = S_OK;

	CString l_csCameraName;

	unsigned long l_ulSize = m_caCameras.GetSize();
	unsigned long l_ulNewIndex = l_ulSize;

	for ( unsigned long i = 0; i < l_ulSize; i++ )
	{
		if ( m_caCameras[i].m_pCamera == p_pCamera )
		{
			l_ulNewIndex = i;

			break;
		}
	}

	if ( l_ulNewIndex < l_ulSize )
	{
		l_csCameraName = m_caCameras[l_ulNewIndex].m_csFullName;

		// SEJ - make sure there is a valid pointer to the camera class
		if ( m_caCameras[l_ulNewIndex].m_pCamera != NULL )
		{
			if( m_caCameras[l_ulNewIndex].m_pCamera->IsoActive() )
			{
				if( ! m_caCameras[l_ulNewIndex].m_pCamera->StopIso() )
				{
					l_hrOk = S_FALSE;
				}
			}

			m_caCameras[l_ulNewIndex].m_pCamera->SetUser( (void *)-1 );
			m_caCameras[l_ulNewIndex].m_pCamera = NULL;

			if( m_caCameras[l_ulNewIndex].mhShutdown != NULL )
			{
				::SetEvent( m_caCameras[l_ulNewIndex].mhShutdown );
				::CloseHandle( m_caCameras[l_ulNewIndex].mhShutdown );
				m_caCameras[l_ulNewIndex].mhShutdown = NULL;
			}

			if( m_caCameras[l_ulNewIndex].mhThread != NULL )
			{
				::CloseHandle( m_caCameras[l_ulNewIndex].mhThread );
				m_caCameras[l_ulNewIndex].mhThread = NULL;
			}
		}
	}

	CString l_csbuf;
	SVException l_svLog;

	l_csbuf.Format("Destroy Camera = %s - HRESULT = 0x%08X", l_csCameraName, l_hrOk );
	SETEXCEPTION1( l_svLog, SVMSG_SVINTEK_NO_ERROR, l_csbuf );
	l_svLog.LogException( l_csbuf );

	return l_hrOk;
}

/*
This method is used get the camera count from the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraGetCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	p_rulCount = m_caCameras.GetSize();

	return l_hrOk;
}

/*
This method is used get the camera name from the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraGetName( unsigned long p_ulIndex, BSTR &p_rbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		l_hrOk = S_OK;

		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = NULL;
		}

		p_rbstrName = m_caCameras[p_ulIndex].m_csFullName.AllocSysString();
	} 

	return l_hrOk;
}

/*
This method is used get the camera image height from the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraGetHeight( unsigned long p_ulIndex, unsigned long &p_rulHeight )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		p_rulHeight = m_caCameras[p_ulIndex].m_ulHeight;

		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

/*
This method is used get the camera image width from the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraGetWidth( unsigned long p_ulIndex, unsigned long &p_rulWidth )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		p_rulWidth = m_caCameras[p_ulIndex].m_ulWidth;

		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

/*
This method is used get the camera parameter list from the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraGetParameterList( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		::VariantClear( p_pvarValue );

		if( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
		{
			SAFEARRAYBOUND l_psabData[1];
			long l_plIndex[1];

			l_psabData[0].cElements = SVIntekParameterDCamSize;
			l_psabData[0].lLbound = 0;

			p_pvarValue->vt = VT_ARRAY | VT_I4;
			p_pvarValue->parray = ::SafeArrayCreate( VT_I4, 1, l_psabData );

			for( int i = 0; i < SVIntekParameterDCamSize; i++ )
			{
				//Parameters
				l_plIndex[0] = i;
				::SafeArrayPutElement( p_pvarValue->parray, l_plIndex, (void *)(l_plIndex) );
			}

			l_hrOk = S_OK;
		}
	} 

	return l_hrOk;
}

/*
This method is used get the camera parameter name.
*/
HRESULT SVIntekFireStackClass::CameraGetParameterName( unsigned long p_ulIndex, int p_iParameterID, BSTR &p_rbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		l_hrOk = S_OK;

		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = NULL;
		}

		switch( p_iParameterID )
		{
			case SVIntekParameterDCamTriggerMode:
			{
				p_rbstrName = ::SysAllocString( L"Trigger Mode" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamIsoSpeed:
			{
				p_rbstrName = ::SysAllocString( L"ISO Speed" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat:
			{
				p_rbstrName = ::SysAllocString( L"Video Format" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamMode:
			{
				p_rbstrName = ::SysAllocString( L"Video Mode" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamRate:
			{
				p_rbstrName = ::SysAllocString( L"Video Rate" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamYIsRawMode:
			{
				p_rbstrName = ::SysAllocString( L"Y is Raw Mode" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamBayerPattern:
			{
				p_rbstrName = ::SysAllocString( L"Bayer Pattern" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamY16InverseOrder:
			{
				p_rbstrName = ::SysAllocString( L"Y16 Inverse Order" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat7XOffset:
			{
				p_rbstrName = ::SysAllocString( L"Format 7 X Offset" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat7YOffset:
			{
				p_rbstrName = ::SysAllocString( L"Format 7 Y Offset" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat7XSize:
			{
				p_rbstrName = ::SysAllocString( L"Format 7 X Size" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat7YSize:
			{
				p_rbstrName = ::SysAllocString( L"Format 7 Y Size" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat7ColorFormat:
			{
				p_rbstrName = ::SysAllocString( L"Format 7 Color Format" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFormat7IsoPacketSize:
			{
				p_rbstrName = ::SysAllocString( L"Format 7 ISO Packet Size" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamFeatureBrightness:
			{
				p_rbstrName = ::SysAllocString( L"Brightness" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureAutoExposure:
			{
				p_rbstrName = ::SysAllocString( L"Auto Exposure" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureSharpness:
			{
				p_rbstrName = ::SysAllocString( L"Sharpness" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureWhiteBalanceU:
			{
				p_rbstrName = ::SysAllocString( L"White Balance - U" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureWhiteBalanceV:
			{
				p_rbstrName = ::SysAllocString( L"White Balance - V" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureHue:
			{
				p_rbstrName = ::SysAllocString( L"Hue" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureSaturation:
			{
				p_rbstrName = ::SysAllocString( L"Saturation" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureGamma:
			{
				p_rbstrName = ::SysAllocString( L"Gamma" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureShutter:
			{
				p_rbstrName = ::SysAllocString( L"Shutter" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureGain:
			{
				p_rbstrName = ::SysAllocString( L"Gain" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureIris:
			{
				p_rbstrName = ::SysAllocString( L"Iris" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureFocus:
			{
				p_rbstrName = ::SysAllocString( L"Focus" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureTemperature:
			{
				p_rbstrName = ::SysAllocString( L"Temperature" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureZoom:
			{
				p_rbstrName = ::SysAllocString( L"Zoom" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeaturePan:
			{
				p_rbstrName = ::SysAllocString( L"Pan" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureTilt:
			{
				p_rbstrName = ::SysAllocString( L"Tilt" );

				l_hrOk = S_OK;
				
				break;
			}
			case SVIntekParameterDCamFeatureOpticalFilter:
			{
				p_rbstrName = ::SysAllocString( L"Optical Filter" );

				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamVendorId:
			{
				p_rbstrName = ::SysAllocString( L"Vendor Id" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamVendorName:
			{
				p_rbstrName = ::SysAllocString( L"Vendor Name" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamModelName:
			{
				p_rbstrName = ::SysAllocString( L"Model Name" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamSerialNumberHigh:
			{
				p_rbstrName = ::SysAllocString( L"Serial Number High" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamSerialNumberLow:
			{
				p_rbstrName = ::SysAllocString( L"Serial Number Low" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamSerialNumberString:
			{
				p_rbstrName = ::SysAllocString( L"SerialNumberString" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamColorFormat:
			{
				p_rbstrName = ::SysAllocString( L"Color Format" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamIsoPacketSize:
			{
				p_rbstrName = ::SysAllocString( L"ISO Packet Size" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamIsoPacketCount:
			{
				p_rbstrName = ::SysAllocString( L"ISO Packet Count" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamRegisterAddress:
			{
				p_rbstrName = ::SysAllocString( L"Register Address" );

				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamRegisterDataQuad:
			{
				p_rbstrName = ::SysAllocString( L"Register Data Quad" );

				l_hrOk = S_OK;

				break;
			}
			case SVIntekParameterDCamRegisterDataBlock:
			{
				p_rbstrName = ::SysAllocString( L"Register Data Block" );

				l_hrOk = S_OK;

				break;
			}
		}
	}

	return l_hrOk;
}

/*
This method is used get the camera parameter from the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraGetParameter( unsigned long p_ulIndex, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
		{
			CCamera* l_pCamera = m_caCameras[ p_ulIndex ].m_pCamera;

			if( l_pCamera != NULL )
			{
				switch( p_iParameterID )
				{
					case SVIntekParameterDCamTriggerMode:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->TriggerMode;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamIsoSpeed:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->IsoSpeed;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->VideoFormat;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamMode:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->VideoMode;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamRate:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->FrameRate;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamYIsRawMode:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->YIsRawMode;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamBayerPattern:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->BayerPattern;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamY16InverseOrder:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->Y16InversOrder;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat7XOffset:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->XPos;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat7YOffset:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->YPos;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat7XSize:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->XSize;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat7YSize:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->YSize;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat7ColorFormat:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->ColorFormat;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFormat7IsoPacketSize:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->PktSize;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamFeatureBrightness:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_BRIGHTNESS, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureAutoExposure:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_AUTOEXPOSURE, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureSharpness:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_SHARPNESS, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureWhiteBalanceU:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_WHITEBAL, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.SubValue;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureWhiteBalanceV:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_WHITEBAL, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureHue:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_HUE, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureSaturation:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_SATURATION, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureGamma:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_GAMMA, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureShutter:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_SHUTTER, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureGain:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_GAIN, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureIris:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_IRIS, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureFocus:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_FOCUS, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureTemperature:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_TEMPERATURE, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureZoom:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_ZOOM, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeaturePan:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_PAN, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureTilt:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_TILT, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}
						
						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamFeatureOpticalFilter:
					{
						FEATPROP l_oFeatProp;

						::VariantClear( p_pvarValue );

						//m_caCameras[ p_ulIndex ].m_pCamera->Lock();

						if( l_pCamera->GetFeature( CFT_OPTICALFILTER, &l_oFeatProp ) )
						{
							p_pvarValue->vt = VT_I4;
							p_pvarValue->ulVal = l_oFeatProp.Value;
							
							l_hrOk = S_OK;
						}

						//m_caCameras[ p_ulIndex ].m_pCamera->Unlock();

						break;
					}
					case SVIntekParameterDCamVendorId:
					{
						unsigned long l_ulQuadlet = 0;

						::VariantClear( p_pvarValue );

						if( l_pCamera->ReadQuad(0xFFFF, 0xF000040C, &l_ulQuadlet) )
						{
							CString l_csName;

							l_csName.Format( "%06LX", l_ulQuadlet >> 8 );

							p_pvarValue->vt = VT_BSTR;
							p_pvarValue->bstrVal = l_csName.AllocSysString();
							
							l_hrOk = S_OK;
						}

						break;
					}
					case SVIntekParameterDCamVendorName:
					{
						::VariantClear( p_pvarValue );

						CString l_csName = l_pCamera->pProps()->VendorName;

						p_pvarValue->vt = VT_BSTR;
						p_pvarValue->bstrVal = l_csName.AllocSysString();
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamModelName:
					{
						::VariantClear( p_pvarValue );

						CString l_csName = l_pCamera->pProps()->ModelName;

						p_pvarValue->vt = VT_BSTR;
						p_pvarValue->bstrVal = l_csName.AllocSysString();

						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamSerialNumberHigh:
					{
						unsigned long l_ulQuadlet = 0;

						::VariantClear( p_pvarValue );

						if( l_pCamera->ReadQuad(0xFFFF, 0xF000040C, &l_ulQuadlet) )
						{
							p_pvarValue->vt = VT_UI4;
							p_pvarValue->ulVal = l_ulQuadlet & 0x000000FF;
							
							l_hrOk = S_OK;
						}

						break;
					}
					case SVIntekParameterDCamSerialNumberLow:
					{
						unsigned long l_ulQuadlet = 0;

						::VariantClear( p_pvarValue );

						if( l_pCamera->ReadQuad(0xFFFF, 0xF0000410, &l_ulQuadlet) )
						{
							p_pvarValue->vt = VT_UI4;
							p_pvarValue->ulVal = l_ulQuadlet;
							
							l_hrOk = S_OK;
						}

						break;
					}
					case SVIntekParameterDCamSerialNumberString:
					{
						::VariantClear( p_pvarValue );

						unsigned long lSerialNumberLow = 0;  // unit (chip) number
						unsigned long lSerialNumberHigh = 0; // vendor number

						if( l_pCamera->ReadQuad(0xFFFF, 0xF000040C, &lSerialNumberHigh) &&
							l_pCamera->ReadQuad(0xFFFF, 0xF0000410, &lSerialNumberLow) )
						{
							CString sSerialNumber;

							unsigned __int64 iUnitID = (unsigned __int64) lSerialNumberLow | ( ( ( (unsigned __int64) ( lSerialNumberHigh & 0x000000FF ) ) ) << 32);
							unsigned __int64 iVendorID = lSerialNumberHigh >> 8;

							sSerialNumber.Format(_T("%010I64X-%06I64X"), iUnitID, iVendorID);

							p_pvarValue->vt = VT_BSTR;
							p_pvarValue->bstrVal = sSerialNumber.AllocSysString();

							l_hrOk = S_OK;
						}

						break;
					}
					case SVIntekParameterDCamColorFormat:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->ColorFormat;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamIsoPacketSize:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->PktSize;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamIsoPacketCount:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_I4;
						p_pvarValue->ulVal = l_pCamera->pParms()->PktCnt;
						
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamRegisterAddress:
					{
						::VariantClear( p_pvarValue );

						p_pvarValue->vt = VT_UI4;
						p_pvarValue->ulVal = m_caCameras[ p_ulIndex ].m_ulRegisterAddress;
							
						l_hrOk = S_OK;

						break;
					}
					case SVIntekParameterDCamRegisterDataQuad:
					{
						unsigned long l_ulQuadlet = 0;
						unsigned long l_ulAddress = m_caCameras[ p_ulIndex ].m_ulRegisterAddress;

						::VariantClear( p_pvarValue );

						if( l_pCamera->ReadQuad(0xFFFF, l_ulAddress, &l_ulQuadlet) )
						{
							p_pvarValue->vt = VT_UI4;
							p_pvarValue->ulVal = l_ulQuadlet;
							
							l_hrOk = S_OK;
						}

						break;
					}
					case SVIntekParameterDCamRegisterDataBlock:
					{
						unsigned long l_ulAddress = m_caCameras[ p_ulIndex ].m_ulRegisterAddress;

						try
						{
							long l_lCount = p_pvarValue->parray->rgsabound[0].cElements;

							unsigned char *l_pucArray = new unsigned char[ l_lCount ];

							if( l_pCamera->ReadBlock( 0xFFFF, l_ulAddress, l_pucArray, l_lCount ) )
							{
								l_hrOk = S_OK;
							}

							for( long l = 0; l_hrOk == S_OK && l < l_lCount; l++ )
							{
								long l_plIndex[1];

								l_plIndex[0] = l;

								l_hrOk = ::SafeArrayPutElement( p_pvarValue->parray, l_plIndex, (void *)(l_pucArray + l) );
							}

							delete[] l_pucArray;
						}
						catch( ... )
						{
						}

						break;
					}
					default:
					{
						break;
					}
				}
			}
		}
	} 

	return l_hrOk;
}

/*
This method is used set the camera parameter to the appropriate object container.
*/
HRESULT SVIntekFireStackClass::CameraSetParameter( unsigned long p_ulIndex, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
		{
			switch( p_iParameterID )
			{
				// Special case for start of main parameter tracking
				case SVIntekBeginTrackParameters:
				{
					TrackCameraParams(p_ulIndex);
					l_hrOk = S_OK;
					break;
				}
				// Special case for end of main parameter tracking
				case SVIntekEndTrackParameters:
				{
					LockMainCameraParamList(p_ulIndex);
					l_hrOk = S_OK;
					break;
				}

				case SVIntekParameterDCamTriggerMode:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						m_caCameras[ p_ulIndex ].m_pCamera->pParms()->TriggerMode = (unsigned char)p_pvarValue->ulVal;
						
						// SEJ
						SetIntekCameraParams(p_ulIndex);

						l_hrOk = S_OK;
					}
					break;
				}
				case SVIntekParameterDCamIsoSpeed:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						m_caCameras[ p_ulIndex ].m_pCamera->pParms()->IsoSpeed = (unsigned char)p_pvarValue->ulVal;
						
						// SEJ
						SetIntekCameraParams(p_ulIndex);

						l_hrOk = S_OK;
					}
					break;
				}
				case SVIntekParameterDCamFormat:
				{
					m_caCameras[ p_ulIndex ].m_ucVideoFormat = (unsigned char)p_pvarValue->ulVal;
					
					l_hrOk = S_OK;

					break;
				}
				case SVIntekParameterDCamMode:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoMode != (unsigned char)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_ucVideoMode = (unsigned char)p_pvarValue->ulVal;
							
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamRate:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->FrameRate != (unsigned char)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_ucFrameRate = (unsigned char)p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamYIsRawMode:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						m_caCameras[ p_ulIndex ].m_pCamera->pParms()->YIsRawMode = (unsigned char)p_pvarValue->ulVal;
						
						// SEJ
						SetIntekCameraParams(p_ulIndex);

						l_hrOk = S_OK;
					}
					break;
				}
				case SVIntekParameterDCamBayerPattern:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						m_caCameras[ p_ulIndex ].m_pCamera->pParms()->BayerPattern = (unsigned char)p_pvarValue->ulVal;
						
						// SEJ
						SetIntekCameraParams(p_ulIndex);

						l_hrOk = S_OK;
					}
					break;
				}
				case SVIntekParameterDCamY16InverseOrder:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						m_caCameras[ p_ulIndex ].m_pCamera->pParms()->Y16InversOrder = (unsigned char)p_pvarValue->ulVal;
						
						// SEJ
						SetIntekCameraParams(p_ulIndex);

						l_hrOk = S_OK;
					}
					break;
				}
				case SVIntekParameterDCamFormat7XOffset:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->XPos != (unsigned short)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_usXPos = (unsigned short)p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamFormat7YOffset:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->YPos != (unsigned short)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_usYPos = (unsigned short)p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamFormat7XSize:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->XSize != (unsigned short)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_ulWidth = p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamFormat7YSize:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->YSize != (unsigned short)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_ulHeight = p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamFormat7ColorFormat:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->ColorFormat != (unsigned char)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_ucColorFormat = (unsigned char)p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamFormat7IsoPacketSize:
				{
					// SEJ - make sure there is a valid pointer to the camera class
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
					{
						if( m_caCameras[ p_ulIndex ].m_pCamera->pParms()->PktSize != (unsigned short)p_pvarValue->ulVal )
						{
							m_caCameras[ p_ulIndex ].m_usPktSize = (unsigned short)p_pvarValue->ulVal;
						
							l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

							// SEJ
							SetIntekCameraParams(p_ulIndex);
						}
						else
						{
							l_hrOk = S_OK;
						}
					}
					break;
				}
				case SVIntekParameterDCamFeatureBrightness:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_BRIGHTNESS, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureAutoExposure:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_AUTOEXPOSURE, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureSharpness:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_SHARPNESS, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureWhiteBalanceU:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_WHITEBAL, p_pvarValue->ulVal, true);
					break;
				}
				case SVIntekParameterDCamFeatureWhiteBalanceV:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_WHITEBAL, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureHue:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_HUE, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureSaturation:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_SATURATION, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureGamma:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_GAMMA, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureShutter:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_SHUTTER, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureGain:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_GAIN, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureIris:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_IRIS, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureFocus:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_FOCUS, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureTemperature:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_TEMPERATURE, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureZoom:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_ZOOM, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeaturePan:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_PAN, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureTilt:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_TILT, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamFeatureOpticalFilter:
				{
					// SEJ
					l_hrOk = SetFeature(p_ulIndex, CFT_OPTICALFILTER, p_pvarValue->ulVal);
					break;
				}
				//
				case SVIntekSoftwareTriggerRegisterAddress:
				{
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL)
					{
						m_caCameras[ p_ulIndex ].m_DCamTriggerMode = TM_SOFTWARE;

						m_caCameras[ p_ulIndex ].m_ulSoftwareTriggerRegisterAddress = p_pvarValue->ulVal;
					}
					break;
				}
				case SVIntekSoftwareTriggerRegisterValue:
				{
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL)
					{
						m_caCameras[ p_ulIndex ].m_DCamTriggerMode = TM_SOFTWARE;

						m_caCameras[ p_ulIndex ].m_ulSoftwareTriggerValue = p_pvarValue->ulVal;
					}
					break;
				}

				case SVIntekParameterDCamRegisterAddress:
				{
					if (m_caCameras[ p_ulIndex ].m_pCamera != NULL)
					{
						m_caCameras[ p_ulIndex ].m_ulRegisterAddress = p_pvarValue->ulVal;

						if( ( p_pvarValue->ulVal & 0xFFFF0000 ) == 0 )
						{
							m_caCameras[ p_ulIndex ].m_ulRegisterAddress |= m_caCameras[ p_ulIndex ].m_pCamera->pProps()->CmdRegBase;
						}

						l_hrOk = S_OK;
					}
					break;
				}
				case SVIntekParameterDCamRegisterDataQuad:
				{
					// SEJ
					l_hrOk = WriteQuad(p_ulIndex, p_pvarValue->ulVal);
					break;
				}
				case SVIntekParameterDCamRegisterDataBlock:
				{
					// SEJ
					l_hrOk = WriteBlock(p_ulIndex, p_pvarValue);
					break;
				}
			}
		}
	} 

	return l_hrOk;
}

/*
This method is used get the camera image format from the appropriate object container.

Valid image formats for FireStack are:

	SVImageFormatMono8      CM_Y8
	SVImageFormatMono16     CM_Y16
	SVImageFormatRGB888     CM_RGB8
	SVImageFormatRGB161616  CM_RGB16
	SVImageFormatY411       CM_YUV411 
	SVImageFormatY422       CM_YUV422
	SVImageFormatY444       CM_YUV444

*/
HRESULT SVIntekFireStackClass::CameraGetFormat( unsigned long p_ulIndex, int &p_riFormat )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		switch( m_caCameras[p_ulIndex].m_iFormat )
		{
			case SVImageFormatMono16:
			case SVImageFormatRGB888:
			case SVImageFormatRGB8888:
			case SVImageFormatRGB161616:
			case SVImageFormatY411:
			case SVImageFormatY422:
			case SVImageFormatY444:
			{
				p_riFormat = SVImageFormatRGB8888;

				break;
			}
			default:
			{
				p_riFormat = m_caCameras[p_ulIndex].m_iFormat;

				break;
			}
		}

		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

/*
This method is used to create all of the image buffers associated with a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		l_hrOk = CameraUpdateFormatModeRateColorFormat( p_ulIndex );

		if( l_hrOk == S_OK )
		{
			if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
			{
				m_caCameras[ p_ulIndex ].m_pCamera->pParms()->BufferCnt = p_ulCount;

				for ( unsigned long i = 0; l_hrOk == S_OK && i < p_ulCount; i++ )
				{
					if( i < static_cast< size_t >( m_caCameras[ p_ulIndex ].m_caBuffers.GetSize() ) )
					{
						BufferDestroy( m_caCameras[ p_ulIndex ].m_caBuffers[ i ] );
					}
					else
					{
						m_caCameras[ p_ulIndex ].m_caBuffers.Add( SVIntekBufferStruct() );
					}

					l_hrOk = BufferCreate( m_caCameras[ p_ulIndex ].m_ulWidth,
																 m_caCameras[ p_ulIndex ].m_ulHeight,
																 m_caCameras[ p_ulIndex ].m_iFormat,
																 m_caCameras[ p_ulIndex ].m_caBuffers[ i ] );

					if( l_hrOk == S_OK )
					{
						m_caCameras[ p_ulIndex ].m_caBuffers[ i ].m_CameraIndex = p_ulIndex;
					}
				}

				if ( l_hrOk != S_OK )
				{
					CameraBufferDestroyAll( p_ulIndex );
				}
			}
		}
	}

	return l_hrOk;
}

/*
This method is used to destroy all of the image buffers associated with a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraBufferDestroyAll(unsigned long p_ulIndex)
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		l_hrOk = CameraStop( p_ulIndex );

		// SEJ - make sure there is a valid pointer to the camera class
		if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
		{
			m_caCameras[ p_ulIndex ].m_pCamera->pParms()->BufferCnt = 0;
		}

		long l_lCount = m_caCameras[ p_ulIndex ].m_caBuffers.GetSize();

		for ( long i = l_lCount - 1 ; 0 <= i; i-- )
		{
			BufferDestroy( m_caCameras[ p_ulIndex ].m_caBuffers[ i ] );
		}
	}

	return l_hrOk;
}

HRESULT SVIntekFireStackClass::CameraRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		m_caCameras[p_ulIndex].m_pBufferInterface = p_pInterface;
	} 
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVIntekFireStackClass::CameraUnregisterBufferInterface( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		m_caCameras[p_ulIndex].m_pBufferInterface = NULL;
	} 
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

/*
This method is used to start acquiring images for a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraStart( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		if ( ::InterlockedExchange( &(m_caCameras[p_ulIndex].m_lIsStarted), 1 ) == 0 )
		{
			// SEJ - make sure there is a valid pointer to the camera class
			if (m_caCameras[p_ulIndex].m_pCamera != NULL)
			{
				if( ! m_caCameras[p_ulIndex].m_pCamera->IsoActive() )
				{
					for( size_t i = 0; i < static_cast< size_t >( m_caCameras[p_ulIndex].m_caBuffers.GetSize() ); ++i )
					{
						m_caCameras[p_ulIndex].m_caBuffers[ i ].m_bIsLocked = false;
					}

					m_caCameras[p_ulIndex].m_NextCopyBuffer = 0;
					m_caCameras[p_ulIndex].m_NextProcessBuffer = 0;

					unsigned long l_ThreadID;

					if( m_caCameras[p_ulIndex].mhShutdown != NULL )
					{
						::ResetEvent( m_caCameras[p_ulIndex].mhShutdown );
					}
					else
					{
						m_caCameras[p_ulIndex].mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );
					}

					m_caCameras[p_ulIndex].mhThread = CreateThread( NULL, 0, SVIntekEndFrameThreadProc, new SVIntekEndFrameThreadParam( this, p_ulIndex ), 0, &l_ThreadID );

					::SetThreadPriority( m_caCameras[p_ulIndex].mhThread, THREAD_PRIORITY_ABOVE_NORMAL );

					CameraUpdateFormatModeRateColorFormat( p_ulIndex );

					// Reset frame counter
					m_caCameras[p_ulIndex].m_pCamera->ClearFrameCounter();

					// SEJ - Not using UserFrames (or GetAsyncKeyState)
					m_caCameras[p_ulIndex].m_pCamera->UseUserFrames(false);

 					if ( m_caCameras[p_ulIndex].m_pCamera->StartIso() )
					{
						// SEJ - to save ISOChannel Number
						SetIntekCameraParams(p_ulIndex);

						l_hrOk = S_OK;
					}
				}
			}
			
			if( l_hrOk != S_OK )
			{
				::InterlockedExchange( &(m_caCameras[p_ulIndex].m_lIsStarted), 0 );
			}
		}
	} 

	return l_hrOk;
}

/*
This method is used to stop acquiring images for a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraStop( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		if ( ::InterlockedExchange( &(m_caCameras[p_ulIndex].m_lIsStarted), 0 ) == 1 )
		{
			// SEJ - make sure there is a valid pointer to the camera class
			if (m_caCameras[p_ulIndex].m_pCamera != NULL)
			{
				if( ! m_caCameras[p_ulIndex].m_pCamera->IsoActive() ||
					! m_caCameras[p_ulIndex].m_pCamera->StopIso() )
				{
					l_hrOk = S_FALSE;
				}

				if( m_caCameras[p_ulIndex].mhShutdown != NULL )
				{
					::SetEvent( m_caCameras[p_ulIndex].mhShutdown );
				}

				if( m_caCameras[p_ulIndex].mhThread != NULL )
				{
					::CloseHandle( m_caCameras[p_ulIndex].mhThread );
					m_caCameras[p_ulIndex].mhThread = NULL;
				}
			}
		}
	} 

	return l_hrOk;
}

/*
This method is used to process the start frame event for a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraProcessStartFrame( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		m_caCameras[ p_ulIndex ].m_StartFrameTimeStamp = 0;

		if ( m_caCameras[p_ulIndex].m_pCamera != NULL )
		{
			if ( m_caCameras[p_ulIndex].m_lIsStarted == 1 )
			{
				if (m_caCameras[p_ulIndex].IsAcquisitionTriggered())
				{
					DoAcquisitionTrigger(m_caCameras[p_ulIndex]);
				}
				if( m_caCameras[p_ulIndex].m_pBufferInterface != NULL )
				{
					m_caCameras[p_ulIndex].m_StartFrameTimeStamp = m_caCameras[p_ulIndex].m_pBufferInterface->GetTimeStamp();
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

/*
This method is used to process the end frame event for a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraAPCProcessEndFrame( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() && m_caCameras[p_ulIndex].m_pCamera != NULL )
	{
		SVClock::SVTimeStamp l_StartFrameTimeStamp = m_caCameras[p_ulIndex].m_StartFrameTimeStamp;

		m_caCameras[p_ulIndex].m_StartFrameTimeStamp = 0;

		CCamera *l_pCamera = m_caCameras[p_ulIndex].m_pCamera;

		if ( l_pCamera != NULL )
		{
			FCTLFRMINFO l_FrameInfo;

			unsigned long l_ulFrameNumber;

			BOOL l_bResult = TRUE;

			if ( l_bResult && l_pCamera->FrameQueueSize() != 0 )
			{
				l_pCamera->Lock();

				l_bResult = (BOOL)l_pCamera->GetFrame( &l_FrameInfo, 0, &l_ulFrameNumber );

				if( l_bResult )
				{
					if( ! m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextCopyBuffer ].m_bIsLocked )
					{
						m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextCopyBuffer ].m_bIsLocked = true;

						char l_szbuf[128];

						unsigned long l_lTimeStamp = 0;
						unsigned long l_lStartStamp = ((unsigned long *)(l_FrameInfo.SysHdr.Arg))[1];
						unsigned long l_lEndStamp = ((unsigned long *)(l_FrameInfo.SysHdr.Arg))[2];

						FCTLGetCycleTime( l_pCamera->CardHandle(), &l_lTimeStamp );

						SVIntekTimeStruct l_svStartTime( l_lStartStamp );
						SVIntekTimeStruct l_svEndTime( l_lEndStamp );
						SVIntekTimeStruct l_svCurrentTime( l_lTimeStamp );

						long l_lPacketCount = l_svEndTime.GetCycleCountDifference( l_svStartTime );
						long l_lMinPacketCount = static_cast<long>(m_caCameras[ p_ulIndex ].m_pCamera->pParms()->PktCnt * 0.95);
						long l_lMaxPacketCount = static_cast<long>(m_caCameras[ p_ulIndex ].m_pCamera->pParms()->PktCnt * 1.25);

						if ( m_caCameras[p_ulIndex].m_lIsStarted == 1 && 
								 l_FrameInfo.UserInfo.ErrorCode == HALER_NOERROR &&
								 l_StartFrameTimeStamp != 0 && m_caCameras[p_ulIndex].m_pBufferInterface != NULL )
						{
							SVClock::SVTimeStamp l_Before = 0;
							SVClock::SVTimeStamp l_After = 0;
							SVClock::SVTimeStamp l_Current = 0;
			
							l_Before = m_caCameras[p_ulIndex].m_pBufferInterface->GetTimeStamp();
							FCTLGetCycleTime( l_pCamera->CardHandle(), &l_lTimeStamp );
							l_After = m_caCameras[p_ulIndex].m_pBufferInterface->GetTimeStamp();

							l_Current = ( l_Before + l_After ) / 2;

							unsigned char *l_pBuffer = m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextCopyBuffer ].m_pucImage;

							if( l_pBuffer != NULL )
							{
								switch ( l_FrameInfo.UserInfo.ColorFormat )
								{
									case CM_Y8:
									{
										ConvertY8( l_FrameInfo.UserInfo.XSize,
																l_FrameInfo.UserInfo.YSize,
																l_FrameInfo.pData,
																l_pBuffer,
																8 );

										break;
									}
									case CM_RGB8:
									{
										ConvertRGB( l_FrameInfo.UserInfo.XSize,
																l_FrameInfo.UserInfo.YSize,
																l_FrameInfo.pData,
																l_pBuffer,
																32 );

										break;
									}
									case CM_YUV411:
									{
										Convert411( l_FrameInfo.UserInfo.XSize,
																l_FrameInfo.UserInfo.YSize,
																l_FrameInfo.pData,
																l_pBuffer,
																32 );

										break;
									}
									case CM_YUV422:
									{
										Convert422( l_FrameInfo.UserInfo.XSize,
																l_FrameInfo.UserInfo.YSize,
																l_FrameInfo.pData,
																l_pBuffer,
																32 );

										break;
									}
									case CM_YUV444:
									{
										Convert444( l_FrameInfo.UserInfo.XSize,
																l_FrameInfo.UserInfo.YSize,
																l_FrameInfo.pData,
																l_pBuffer,
																32 );

										break;
									}
									case CM_Y16:
									{
										ConvertY16( l_FrameInfo.UserInfo.XSize,
																l_FrameInfo.UserInfo.YSize,
																l_FrameInfo.pData,
																l_pBuffer,
																32 );
										break;
									}
									default:
									{
										l_hrOk = S_FALSE;

										wsprintf(l_szbuf,"Error in Color Format");
											
										TRACE( "%s\n", l_szbuf );

										break;
									}
								}

								if( l_hrOk == S_OK )
								{
									/*
									SVClock::SVTimeStamp l_SF = 0;

									l_SF = SVIntekTimeStruct::GetSystemTime( l_svStartTime, l_lTimeStamp, l_Current );

									if( l_SF < l_StartFrameTimeStamp )
									{
										l_StartFrameTimeStamp = l_SF;
									}
									*/

									m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextCopyBuffer ].m_StartTimeStamp = l_StartFrameTimeStamp;
									m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextCopyBuffer ].m_EndTimeStamp = m_caCameras[p_ulIndex].m_pBufferInterface->GetTimeStamp();

									m_caCameras[p_ulIndex].m_NextCopyBuffer = ( m_caCameras[p_ulIndex].m_NextCopyBuffer + 1 ) % m_caCameras[p_ulIndex].m_caBuffers.GetSize();

									QueueUserAPC( SVIntekEndFrameAPCProc, m_caCameras[ p_ulIndex ].mhThread, NULL );
								}
							}
						}
						else
						{
							wsprintf(l_szbuf,"IsStarted = %s", (m_caCameras[p_ulIndex].m_lIsStarted)?"TRUE":"FALSE" );
							TRACE( "%s\n", l_szbuf );

							wsprintf(l_szbuf,"Is ErrorCode(%d) == HALER_NOERROR(%d)", l_FrameInfo.UserInfo.ErrorCode, HALER_NOERROR );
							TRACE( "%s\n", l_szbuf );

							wsprintf(l_szbuf,"l_lMinPacketCount(%d) <= l_lPacketCount(%d) <= l_lMaxPacketCount(%d)", l_lMinPacketCount, l_lPacketCount, l_lMaxPacketCount );
							TRACE( "%s\n", l_szbuf );

							wsprintf(l_szbuf,"Start Frame Time Stamp - Second Count = %d - Cycle Count = %d - Cycle Offset = %d",
								l_svStartTime.m_lSecondCount, l_svStartTime.m_lCycleCount, l_svStartTime.m_lCycleOffset );
							TRACE( "%s\n", l_szbuf );

							wsprintf(l_szbuf,"End Frame Time Stamp - Second Count = %d - Cycle Count = %d - Cycle Offset = %d",
								l_svEndTime.m_lSecondCount, l_svEndTime.m_lCycleCount, l_svEndTime.m_lCycleOffset );
							TRACE( "%s\n", l_szbuf );

							wsprintf(l_szbuf,"Current Time Stamp - Second Count = %d - Cycle Count = %d - Cycle Offset = %d",
								l_svCurrentTime.m_lSecondCount, l_svCurrentTime.m_lCycleCount, l_svCurrentTime.m_lCycleOffset );
							TRACE( "%s\n", l_szbuf );
						}
					}
					else
					{
						TRACE( "Error Buffer Locked\n" );
					}
				}
				else
				{
					TRACE( "Error In GetFrame\n" );
				}
				
				l_pCamera->ReturnFrame( &l_FrameInfo );
				l_pCamera->Unlock();
			}
		}
		else // SEJ
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

/*
This method is used to notify all register listeners to the end frame event for this camera.
*/
HRESULT SVIntekFireStackClass::CameraProcessEndFrame( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		if( m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextProcessBuffer ].m_bIsLocked )
		{
			if ( m_caCameras[p_ulIndex].m_lIsStarted == 1 && m_caCameras[p_ulIndex].m_pBufferInterface != NULL )
			{
				SVImageBufferInterface l_Buffer;
				
				if( m_caCameras[p_ulIndex].m_pBufferInterface->GetNextBuffer( l_Buffer ) == S_OK )
				{
					unsigned char *l_pBuffer = l_Buffer.GetBufferAddress();

					if( l_pBuffer != NULL )
					{
						::memcpy( l_pBuffer, 
							m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextProcessBuffer ].m_pucImage, 
							m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextProcessBuffer ].m_pBitmapInfo->bmiHeader.biSizeImage );

						l_Buffer.SetStartFrameTimeStamp( m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextProcessBuffer ].m_StartTimeStamp );
						l_Buffer.SetEndFrameTimeStamp( m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextProcessBuffer ].m_EndTimeStamp );

						l_hrOk = m_caCameras[p_ulIndex].m_pBufferInterface->UpdateWithCompletedBuffer( l_Buffer );
					}
					else
					{
						l_hrOk = E_FAIL;
					}
				}
				else
				{
					l_hrOk = E_FAIL;
				}
			}
				
			m_caCameras[p_ulIndex].m_caBuffers[ m_caCameras[p_ulIndex].m_NextProcessBuffer ].m_bIsLocked = false;

			m_caCameras[p_ulIndex].m_NextProcessBuffer = ( m_caCameras[p_ulIndex].m_NextProcessBuffer + 1 ) % m_caCameras[p_ulIndex].m_caBuffers.GetSize();
		}
	}

	return l_hrOk;
}

/*
This method is used to get the video format, video mode, and frame rate associated with a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraGetFormatModeRate( unsigned long p_ulIndex, unsigned long &p_rulValue )
{
	HRESULT l_hrOk = S_FALSE;

	p_rulValue = 0;
	
	if( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		// SEJ - make sure there is a valid pointer to the camera class
		if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
		{
			p_rulValue += m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoFormat << 16;
			p_rulValue += m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoMode << 8;
			p_rulValue += m_caCameras[ p_ulIndex ].m_pCamera->pParms()->FrameRate;

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

/*
This method is used to set the video format, video mode, and frame rate to a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraSetFormatModeRate( unsigned long p_ulIndex, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_ulIndex < (unsigned long)m_caCameras.GetSize() )
	{
		// SEJ - make sure there is a valid pointer to the camera class
		if (m_caCameras[ p_ulIndex ].m_pCamera != NULL && m_caCameras[ p_ulIndex ].m_pCamera->pParms() != NULL)
		{
			m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoFormat = (unsigned char)( ( 0x00FF0000 & p_ulValue ) >> 16 );
			m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoMode = (unsigned char)( ( 0x0000FF00 & p_ulValue ) >> 8 );
			m_caCameras[ p_ulIndex ].m_pCamera->pParms()->FrameRate = (unsigned char)( ( 0x000000FF & p_ulValue ) );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

/*
This method is used to synchronize the video format, video mode, and frame rate with a particular camera.
*/
HRESULT SVIntekFireStackClass::CameraUpdateFormatModeRateColorFormat( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_ulIndex < (unsigned long)m_caCameras.GetSize() && m_caCameras[p_ulIndex].m_pCamera != 0 )
	{
		VIDSET VidSet;

		UINT32 VidSetNumber = m_caCameras[p_ulIndex].m_pCamera->GetVidSet();

		long l_lCount = m_caCameras[p_ulIndex].m_pCamera->GetVidSetCount();

		if( m_caCameras[ p_ulIndex ].m_iFormat == SVImageFormatUnknown )
		{
			m_caCameras[ p_ulIndex ].m_ucVideoFormat = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoFormat;
			m_caCameras[ p_ulIndex ].m_ucVideoMode = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoMode;
		}

		unsigned long l_ulFormat = m_caCameras[ p_ulIndex ].m_ucVideoFormat;
		unsigned long l_ulMode = m_caCameras[ p_ulIndex ].m_ucVideoMode;
		unsigned long l_ulColorFormat = m_caCameras[ p_ulIndex ].m_ucColorFormat;

		for( long l = 0; l < l_lCount; l++ )
		{
			m_caCameras[p_ulIndex].m_pCamera->GetVidSetValues( l, &VidSet );

			if( l_ulFormat == VidSet.VideoFormat &&	l_ulMode == VidSet.VideoMode )
			{
				if( l_ulFormat == 7 )
				{
					if( VidSet.ColorInquiry & ( 0x80000000 >> l_ulColorFormat ) )
					{
						VidSetNumber = l;

						break;
					}
				}
				else
				{
					if( m_caCameras[ p_ulIndex ].m_ucFrameRate == VidSet.FrameRate )
					{
						VidSetNumber = l;

						break;
					}
				}
			}
		}

		m_caCameras[p_ulIndex].m_pCamera->SelectVidSetByNr( VidSetNumber );

		l_ulFormat = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoFormat;

		if( l_ulFormat == m_caCameras[ p_ulIndex ].m_ucVideoFormat )
		{
			if( l_ulFormat == 7 )
			{
				FORMAT7INFO l_oInfo;

				l_oInfo.VideoMode=m_caCameras[ p_ulIndex ].m_ucVideoMode;
				l_oInfo.ColorFormat=m_caCameras[ p_ulIndex ].m_ucColorFormat;
				l_oInfo.Speed=m_caCameras[ p_ulIndex ].m_pCamera->pParms()->IsoSpeed;
				l_oInfo.XPos=m_caCameras[ p_ulIndex ].m_usXPos;
				l_oInfo.YPos=m_caCameras[ p_ulIndex ].m_usYPos;
				l_oInfo.XSize = static_cast<unsigned short>(m_caCameras[ p_ulIndex ].m_ulWidth);
				l_oInfo.YSize = static_cast<unsigned short>(m_caCameras[ p_ulIndex ].m_ulHeight);
  
				// Try to set parameters in camera and acquire packet settings
				m_caCameras[p_ulIndex].m_pCamera->AcquirePacketInfo( &l_oInfo );

				VidSet.XPos=l_oInfo.XPos;
				VidSet.YPos=l_oInfo.YPos;
				VidSet.XSize=l_oInfo.XSize;
				VidSet.YSize=l_oInfo.YSize;
				VidSet.ColorFormat=l_oInfo.ColorFormat;
				VidSet.FrameRate=0x07;
				VidSet.PktSize=l_oInfo.PacketMax;

				VidSet.IsScalable = TRUE;

				m_caCameras[p_ulIndex].m_pCamera->SelectVidSetByValue( &VidSet );
			}
			else
			{
				m_caCameras[ p_ulIndex ].m_ucVideoFormat = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoFormat;
				m_caCameras[ p_ulIndex ].m_ucVideoMode = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->VideoMode;
				m_caCameras[ p_ulIndex ].m_ucFrameRate = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->FrameRate;
			}

			SVIntekCameraStruct& l_rsvCamera = m_caCameras[ p_ulIndex ];

			l_rsvCamera.m_ucColorFormat = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->ColorFormat;
			l_rsvCamera.m_usPktSize = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->PktSize;
			l_rsvCamera.m_usXPos = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->XPos;
			l_rsvCamera.m_usYPos = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->YPos;
			l_rsvCamera.m_ulWidth = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->XSize;
			l_rsvCamera.m_ulHeight = m_caCameras[ p_ulIndex ].m_pCamera->pParms()->YSize;

			switch ( m_caCameras[p_ulIndex].m_pCamera->pParms()->ColorFormat )
			{
			case CM_Y8:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatMono8;
					break;
				}
			case CM_Y16:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatMono16;
					break;
				}
			case CM_RGB8:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatRGB888;
					break;
				}
			case CM_RGB16:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatRGB161616;
					break;
				}
			case CM_YUV411:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatY411;
					break;
				}
			case CM_YUV422:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatY422;
					break;
				}
			case CM_YUV444:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatY444;
					break;
				}
			default:
				{
					m_caCameras[p_ulIndex].m_iFormat = SVImageFormatUnknown;
					break;
				}
			}

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

/*
This method set the camera feature to the desired camera based on the provided parameters.
*/
HRESULT SVIntekFireStackClass::SetFeature(int p_ulIndex, unsigned char featureId,  unsigned long ulVal, bool bSubValue)
{
	HRESULT l_hrOk = S_FALSE;

	if (p_ulIndex < m_caCameras.GetSize() && m_caCameras[ p_ulIndex ].m_pCamera != NULL)
	{
		FEATPROP l_oFeatProp;

		if( m_caCameras[ p_ulIndex ].m_pCamera->GetFeature( featureId, &l_oFeatProp ) )
		{
			SVIntekCameraParam param;

			param.m_paramType = SVIntekCameraParam::FeatureSet;
			param.m_featureId = featureId;
			param.m_featureSet.On = 1;
			param.m_featureSet.Auto = 0;
			param.m_featureSet.OneShot = 0;

			if (bSubValue)
			{
				param.m_featureSet.Value = l_oFeatProp.Value;
				param.m_featureSet.SubValue = ulVal;
			}
			else
			{
				param.m_featureSet.Value = ulVal;
				param.m_featureSet.SubValue = l_oFeatProp.SubValue;
			}

			m_caCameras[ p_ulIndex ].m_pCamera->Lock();

			if( m_caCameras[ p_ulIndex ].m_pCamera->SetFeature( featureId, &param.m_featureSet ) )
			{
				l_hrOk = S_OK;
				//save it
				m_cameraParamsMgr.AddCameraParam(m_caCameras[ p_ulIndex ].m_svCameraId.m_ui64CameraId, param);
			}

			m_caCameras[ p_ulIndex ].m_pCamera->Unlock();
		}
	}
	return l_hrOk;
}

/*
This method writes data to the camera based on the interanal register value.
*/
HRESULT SVIntekFireStackClass::WriteQuad(int p_ulIndex, unsigned long ulVal)
{
	HRESULT l_hrOk = S_FALSE;

	if (p_ulIndex < m_caCameras.GetSize() && m_caCameras[ p_ulIndex ].m_pCamera != NULL)
	{
		unsigned long l_ulAddress = m_caCameras[ p_ulIndex ].m_ulRegisterAddress;

		if( m_caCameras[ p_ulIndex ].m_pCamera->WriteQuad( 0xFFFF, l_ulAddress, ulVal, FALSE ) )
		{
			l_hrOk = S_OK;
			// save it
			SVIntekCameraParam param;
			param.m_paramType = SVIntekCameraParam::Quad;
			param.m_quad.Speed = 0;					// don't care
			param.m_quad.BusAddress = 0;			// don't care
			param.m_quad.NodeAddress = 0;			// don't care
			param.m_quad.OffsetHigh = 0xFFFF;
			param.m_quad.OffsetLow = l_ulAddress;
			param.m_quad.Quadlet = ulVal;			// don't swap here...

			// trap triggerMode and strobePulse ???

			m_cameraParamsMgr.AddCameraParam(m_caCameras[ p_ulIndex ].m_svCameraId.m_ui64CameraId, param);
		}
	}
	return l_hrOk;
}

/*
This method writes data to the camera based on the interanal register value.
*/
HRESULT SVIntekFireStackClass::WriteBlock(int p_ulIndex, VARIANT* p_pvarValue)
{
	HRESULT l_hrOk = S_FALSE;

	if( p_ulIndex < m_caCameras.GetSize() && m_caCameras[ p_ulIndex ].m_pCamera != NULL)
	{
		unsigned long l_ulAddress = m_caCameras[ p_ulIndex ].m_ulRegisterAddress;

		try
		{
			long l_lCount = p_pvarValue->parray->rgsabound[0].cElements;

			unsigned char *l_pucArray = new unsigned char[ l_lCount ];

			for( long l = 0; l < l_lCount; l++ )
			{
				long l_plIndex[1];

				l_plIndex[0] = l;

				l_hrOk = ::SafeArrayGetElement( p_pvarValue->parray, l_plIndex, l_pucArray + l );
			}

			if( m_caCameras[ p_ulIndex ].m_pCamera->WriteBlock( 0xFFFF, l_ulAddress, l_pucArray, l_lCount ) )
			{
				l_hrOk = S_OK;

				SVIntekCameraParam param;
				param.m_paramType = SVIntekCameraParam::Block;
				param.m_block.Speed = 0;				// don't care
				param.m_block.BusAddress = 0;			// don't care
				param.m_block.NodeAddress = 0;			// don't care
				param.m_block.OffsetHigh = 0xFFFF;
				param.m_block.OffsetLow = l_ulAddress;
				
				param.m_block.pData = l_pucArray;
				param.m_block.DataLen = l_lCount;
  
				m_cameraParamsMgr.AddCameraParam(m_caCameras[ p_ulIndex ].m_svCameraId.m_ui64CameraId, param);
			}
		}
		catch( ... )
		{
		}
	}
	return l_hrOk;
}

/*
This method sets the camera identification data into the camera parameters for that camera.
*/
HRESULT SVIntekFireStackClass::SetIntekCameraParams(unsigned long p_ulIndex)
{
	return (m_caCameras[ p_ulIndex ].m_pCamera != 0)?
		m_cameraParamsMgr.SetIntekCameraParams(m_caCameras[ p_ulIndex ].m_svCameraId.m_ui64CameraId, *m_caCameras[ p_ulIndex ].m_pCamera->pParms())
		: S_FALSE;
}

/*
This method reloads the camera parameters from the camera parameters manager.
*/
HRESULT SVIntekFireStackClass::ReloadCameraParams(unsigned long p_ulIndex, CCamera* p_pCamera)
{
	// restore Intek FireStack/camera info (videoMode, format7, etc.)
	CAMPARMS camParams;

	HRESULT l_hrOk = m_cameraParamsMgr.GetIntekCameraParams(m_caCameras[ p_ulIndex ].m_svCameraId.m_ui64CameraId, camParams);
	
	if (l_hrOk == S_OK)
	{
		*(p_pCamera->pParms()) = camParams;

		SVIntekCameraStruct& l_rsvCamera = m_caCameras[p_ulIndex];
		
		l_rsvCamera.m_ucColorFormat = camParams.ColorFormat;
		l_rsvCamera.m_usPktSize = camParams.PktSize;
		l_rsvCamera.m_usXPos = camParams.XPos;
		l_rsvCamera.m_usYPos = camParams.YPos;
		l_rsvCamera.m_ulWidth = camParams.XSize;
		l_rsvCamera.m_ulHeight = camParams.YSize;

		l_rsvCamera.m_ucVideoFormat = camParams.VideoFormat;
		l_rsvCamera.m_ucVideoMode = camParams.VideoMode;
		l_rsvCamera.m_ucFrameRate = camParams.FrameRate;

		// send dcam parameters to camera device
		l_hrOk = m_cameraParamsMgr.ReloadCameraParams(m_caCameras[p_ulIndex].m_svCameraId.m_ui64CameraId, p_pCamera);
	}
	return l_hrOk;
}

/*
This method sets the camera manager to start tracking all camera paarameter for a particular camera.
*/
void SVIntekFireStackClass::TrackCameraParams(unsigned long p_ulIndex)
{
	m_cameraParamsMgr.TrackCameraParams(m_caCameras[p_ulIndex].m_svCameraId.m_ui64CameraId);
}

/*
This method sets the camera manager to stop tracking all camera paarameter for a particular camera.
*/
void SVIntekFireStackClass::UnTrackCameraParams(unsigned long p_ulIndex)
{
	m_cameraParamsMgr.UnTrackCameraParams(m_caCameras[p_ulIndex].m_svCameraId.m_ui64CameraId);
}

/*
This method sets the camera manager to lock all camera paarameter for a particular camera.
*/
void SVIntekFireStackClass::LockMainCameraParamList(unsigned long p_ulIndex)
{
	m_cameraParamsMgr.SetMainListLocked(m_caCameras[p_ulIndex].m_svCameraId.m_ui64CameraId);
}

// Just a stub, actual loading is done in SVObserver via SV1394CameraFileReader
HRESULT SVIntekFireStackClass::CameraLoadFiles(unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames)
{
	return CameraUpdateFormatModeRateColorFormat( p_ulIndex );
}

HRESULT SVIntekFireStackClass::InternalTriggerEnable( unsigned long p_ulIndex )
{
	_variant_t val;
	val = m_caCameras[ p_ulIndex ].m_DCamTriggerMode;
	HRESULT hr = CameraSetParameter(p_ulIndex, SVIntekParameterDCamTriggerMode, 0, &val);
	return hr;
}

HRESULT SVIntekFireStackClass::InternalTrigger( unsigned long p_ulIndex )
{
	HRESULT hr = S_FALSE;
	if (m_triggerMgr.HasSubscribers(p_ulIndex))
	{
		hr = m_triggerMgr.Fire(p_ulIndex);
	}
	else
	{
		if( m_caCameras[ p_ulIndex ].m_DCamTriggerMode == TM_SOFTWARE )
		{
			hr = FireSoftwareTrigger(p_ulIndex);
		}
		else
		{
			hr = FireOneShot(p_ulIndex);
		}
	}
	return hr;
}

HRESULT SVIntekFireStackClass::RegisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct )
{
	SVTriggerCallbackStruct triggerCallbackStruct; 
	triggerCallbackStruct.pCallback = callbackStruct.m_pCallback;
	triggerCallbackStruct.pOwner = callbackStruct.m_pOwner;
	triggerCallbackStruct.pData = callbackStruct.m_pData;

	m_triggerMap.Add(p_ulIndex, triggerCallbackStruct);
	
	typedef SVTriggerActivatorFunc<SVIntekFireStackClass> Activator;
	typedef SVTriggerCallbackFunc<SVIntekFireStackClass> TriggerCallback;

	HRESULT l_Status = S_OK;

	if( m_caCameras[ p_ulIndex ].m_DCamTriggerMode == TM_SOFTWARE )
	{
		SVTriggerHandler handler(p_ulIndex, 
								SVTriggerActivator(new Activator(this, &SVIntekFireStackClass::FireSoftwareTrigger)), 
								SVTriggerCallback(new TriggerCallback(this, &SVIntekFireStackClass::DispatchTriggerCallback)));

		l_Status = m_triggerMgr.Subscribe(p_ulIndex, handler);
	}
	else
	{
		SVTriggerHandler handler(p_ulIndex, 
								SVTriggerActivator(new Activator(this, &SVIntekFireStackClass::FireOneShot)), 
								SVTriggerCallback(new TriggerCallback(this, &SVIntekFireStackClass::DispatchTriggerCallback)));

		l_Status = m_triggerMgr.Subscribe(p_ulIndex, handler);
	}

	return l_Status;
}

HRESULT SVIntekFireStackClass::UnregisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct )
{
	SVTriggerCallbackStruct triggerCallbackStruct; 
	triggerCallbackStruct.pCallback = callbackStruct.m_pCallback;
	triggerCallbackStruct.pOwner = callbackStruct.m_pOwner;
	triggerCallbackStruct.pData = callbackStruct.m_pData;

	m_triggerMap.Remove(p_ulIndex, triggerCallbackStruct);
	return m_triggerMgr.Unsubscribe( p_ulIndex);
}

HRESULT SVIntekFireStackClass::UnregisterAllInternalTriggerCallbacks( unsigned long p_ulIndex )
{
	m_triggerMap.RemoveAll(p_ulIndex);
	return m_triggerMgr.Unsubscribe( p_ulIndex );
}

HRESULT SVIntekFireStackClass::FireOneShot( unsigned long p_ulIndex )
{
	HRESULT hr = S_FALSE;
	if( m_caCameras[ p_ulIndex ].m_pCamera != NULL)
	{
		if (m_caCameras[ p_ulIndex ].m_pCamera->DoOneShot())
			hr = S_OK;
	}
	return hr;
}

HRESULT SVIntekFireStackClass::FireSoftwareTrigger( unsigned long p_ulIndex )
{
	HRESULT hr = S_FALSE;
	if( m_caCameras[ p_ulIndex ].m_pCamera != NULL)
	{
		unsigned long ulRegister = m_caCameras[ p_ulIndex ].m_ulSoftwareTriggerRegisterAddress;
		unsigned long ulValue = m_caCameras[ p_ulIndex ].m_ulSoftwareTriggerValue;

		if (m_caCameras[ p_ulIndex ].m_pCamera->DoSoftwareTrigger(ulRegister, ulValue))
			hr = S_OK;
	}
	return hr;
}

HRESULT SVIntekFireStackClass::DispatchTriggerCallback( unsigned long p_ulIndex )
{
	HRESULT hr = m_triggerMap.Dispatch( p_ulIndex );
	return hr;
}

static unsigned long GetDigitizerHandle(unsigned long p_ulIndex)
{
	return p_ulIndex; // oridinal is the handle
}

bool SVIntekFireStackClass::IsValidDigitizer(unsigned long p_ulIndex) const
{
	return ( p_ulIndex < (unsigned long)m_caCameras.GetSize() );
}

SVIntekCameraStruct& SVIntekFireStackClass::GetDigitizer(unsigned long p_ulHandle)
{
	return m_caCameras[p_ulHandle];
}

const SVIntekCameraStruct& SVIntekFireStackClass::GetDigitizer(unsigned long p_ulHandle) const
{
	return m_caCameras[p_ulHandle];
}

// Trigger functions
HRESULT SVIntekFireStackClass::TriggerGetCount(unsigned long& p_ulCount)
{
	HRESULT l_Result = CameraGetCount(p_ulCount);
	return l_Result;
}

// convert ordinal to handle
unsigned long SVIntekFireStackClass::TriggerGetHandle(unsigned long p_ulIndex)
{
	return GetDigitizerHandle(p_ulIndex); 
}

HRESULT SVIntekFireStackClass::TriggerGetName(unsigned long p_ulHandle, BSTR& p_rbstrName)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );
			p_rbstrName = NULL;
		}

		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		SVString name;
		name.Format("CameraTrigger.Dig_%d", p_ulHandle);
		p_rbstrName = name.ToBSTR();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVIntekFireStackClass::TriggerRegisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.SetTriggerCallback(p_Callback);
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVIntekFireStackClass::TriggerUnregisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVIntekFireStackClass::TriggerUnregisterAllCallbacks(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVIntekFireStackClass::TriggerStart(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.m_bAcquisitionTriggered = true;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVIntekFireStackClass::TriggerStop(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.m_bAcquisitionTriggered = false;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVIntekFireStackClass::TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		if (IsValidDigitizer(p_ulHandle))
		{
			*p_pulCount = 1;
			l_hrOk = S_OK;
		}
		else
		{
			*p_pulCount = 0;
		}
	}
	return l_hrOk;
}

HRESULT SVIntekFireStackClass::TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );
			*p_pbstrName = NULL;
		}

		if (IsValidDigitizer(p_ulHandle))
		{
			switch ( p_ulIndex )
			{
				case SVAcquisitionTriggered:
				*p_pbstrName = ::SysAllocString( L"Acquisition Triggered" );
				break;
			}
			if ( *p_pbstrName != NULL )
			{
				l_hrOk = S_OK;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVIntekFireStackClass::TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			if (IsValidDigitizer(p_ulHandle))
			{
				switch ( p_ulIndex)
				{
					case SVAcquisitionTriggered:
					{
						p_pvarValue->vt = VT_BOOL;
						bool bAcquisitonTriggered;
						l_hrOk = IsAcquisitionTriggered(p_ulHandle, bAcquisitonTriggered);
						if (l_hrOk == S_OK)
						{
							p_pvarValue->bVal = (bAcquisitonTriggered) ? VARIANT_TRUE : VARIANT_FALSE;
						}
						break;
					}
				
					default:
						break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVIntekFireStackClass::TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		if ( p_pvarValue != NULL )
		{
			switch (p_ulIndex)
			{
				case SVAcquisitionTriggered:
				{
					if( p_pvarValue->vt == VT_BOOL )
					{
						l_hrOk = SetAcquisitionTriggered( p_ulHandle, (p_pvarValue->boolVal == VARIANT_TRUE) ? true : false);
					}
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVIntekFireStackClass::IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		l_hrOk = S_OK;
		const SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		bAcquisitionTriggered = l_rCamera.IsAcquisitionTriggered();
	}
	return l_hrOk;
}

HRESULT SVIntekFireStackClass::SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered)
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		l_hrOk = S_OK;
		SVIntekCameraStruct& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.SetAcquisitionTriggered(bAcquisitionTriggered);
	}
	return l_hrOk;
}

void SVIntekFireStackClass::DoAcquisitionTrigger(const SVIntekCameraStruct& p_rCamera)
{
	// Get The Timestamp
	double timestamp = 0.0; // how do we get this ?
	
	{
		// Simulate Trigger and send Timestamp and Line State...
		bool lineState = p_rCamera.GetLineState();
		const SVCallbackStruct& callback = p_rCamera.GetTriggerCallback();
		if (callback.m_pCallback)
		{
			SVMaterials materials;
			materials.AddMaterial("Timestamp", _variant_t(timestamp));
			materials.AddMaterial("LineState", _variant_t((lineState) ? VARIANT_TRUE : VARIANT_FALSE));
			
			callback.m_pCallback(callback.m_pOwner, reinterpret_cast<void *>(&materials));
		}
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIntek\SVIntekFireStackClass.cpp_v  $
 * 
 *    Rev 1.3   01 Dec 2014 14:48:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thred label and attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 09:09:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 14:56:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   29 Apr 2013 12:49:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2013 14:53:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   21 Jan 2013 09:50:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   30 Aug 2012 09:19:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  786
 * SCR Title:  Fix Problems with IEEE 1394 Light Reference and LUT Camera Parameters
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Fire Bus callback and Intek Fire Stack class Create Camera method to fix problems with sending parameters during the re-connect process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   26 Sep 2011 19:15:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated thread proirities.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Sep 2011 14:17:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated thread priority for the callback process to not allow it to get interrupted and removed some debug logging.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   17 Aug 2010 10:44:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   24 Feb 2010 09:54:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change thread creation location.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   15 Feb 2010 10:23:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   15 Dec 2009 12:14:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   09 Nov 2009 16:11:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix time conversion issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   02 Nov 2009 19:04:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated trigger mode to use the new software trigger value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   27 Oct 2009 18:32:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Update code to include changes for Intek driver update and fixed issue with camera recovery when on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   15 Oct 2009 10:40:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added FireSoftwareTrigger method.
 * Revised InternalTriggerEnable to use TM_EXTERN.
 * Revised CameraSetParameter to handle Software Trigger Register address and value.
 * Revised RegisterInternalTriggercallback to use FireSoftwareTrigger method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   14 Oct 2009 18:38:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.29
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code based on changes from Intek version 2v15 to 2v30.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   24 Sep 2009 09:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to create the shutdown event when missing.  Updated code to fix missing camera crashes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   03 Sep 2009 08:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   30 Jul 2009 15:49:02   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  667
 * SCR Title:  Fixing the access violation bug in Camera Manager
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added tests for null before accessing m_pCamera
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   29 Jul 2009 16:10:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   11 Sep 2008 10:05:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for Software triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   20 Jun 2007 12:54:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13.1.1   10 Sep 2009 11:10:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Intek DLL to check for NULL cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13.1.0   14 Jul 2009 10:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new buffer acquisition methodology and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   11 Apr 2007 10:49:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to Set/Reset the BusReset Start and Complete flags appropriately.
 * Revised CameraStart to save the IsoChannel Number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   31 Jan 2007 11:05:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CameraSetParameter method to check for a valid camera instance.
 * Revised CameraBufferDestroyAll method to check for a valid camera instance.
 * Revised CameraStart method to check for a valid camera instance.
 * Revised CameraGetFormatModeRate method to check for a valid camera instance.
 * Revised CameraSetFormatModeRate method to check for a valid camera instance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   28 Nov 2006 17:43:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Camera ParamertersTracking logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   01 Aug 2006 10:58:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Y8 conversion routine.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Aug 2006 08:00:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  573
 * SCR Title:  Update Intek images to present 32-bit color instead of 24-bit color
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to convert image to a 32-bit color bitmap instead of a 24-bit color bitmap.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   31 Jul 2006 14:24:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated Set Patameter Address Function to or in the cammand address instead of adding the command address.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Jul 2006 10:31:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated Get Parameters function to check for a NULL Camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Jul 2006 15:22:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Process End Frame method to properly convert flat image block to a Windows Bitmap DIB block.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Jul 2006 15:07:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated class to fix format 7 problems and to serialize the load/open of each 1394 card to fix a hung-up issue on the 9150.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jul 2006 10:51:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to scan the image format inquary register to see if color mode availible for Fomat 7 Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Mar 2006 14:31:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  550
 * SCR Title:  Fix Format7 problem when changing width of image using the Intek driver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add format7 camera parameters and use camera parameters to finalize acquisition definition information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Mar 2006 08:29:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  552
 * SCR Title:  Fix problem with Intek driver when attaching multiple cameras to one board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed licence call when a camera gets created and updated camera format methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Dec 2005 09:44:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to inlcude Bitmap line padding and new licence code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 13:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
