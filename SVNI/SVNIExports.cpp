// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIONIExports
// * .File Name       : $Workfile:   SVNIExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 16:26:36  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVNIExports.h"

#include "SVOMFCLibrary/SVCallbackListArrayClass.h"
#include "SVNIDAQ.h"
#include "NIDAQCNS.H"

#include "SVIOLibrary/SVIOParameterEnum.h"

short g_sBoardNumber = 1;
short g_sBoardType = 0;

unsigned long g_ulTriggerPortCount = 0;
short g_sTriggerPortArray[1];
long g_lTriggerEdge = 0xFFFFFFFF;
long g_lTriggerData = 0x00000000;

unsigned long g_ulInputCount = 0;
unsigned long g_ulInputPortCount = 0;
short g_sInputPortArray[3];

unsigned long g_ulOutputCount = 0;
unsigned long g_ulOutputPortCount = 0;
short g_sOutputPortArray[9];

long g_lRefCount = 0;

SVCallbackListArrayClass g_svCallbackArray;


void _cdecl g_SVIONICallback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

	try
	{
		long l_lData = 0;

		if ( g_NIDIGInPrt( g_sBoardNumber, 0, &l_lData ) == S_OK )
		{
			int l_iSize = g_svCallbackArray.GetSize();

			for ( short i = 0; g_svCallbackArray.m_bIsStarted && i < l_iSize; i++ )
			{
				long l_lMask = 1 << i;

				if ( g_svCallbackArray[ i ].m_bIsStarted &&
				     ( l_lMask & l_lData ) != ( l_lMask & g_lTriggerData ) && 
				     ( l_lMask & l_lData ) == ( l_lMask & g_lTriggerEdge ) )
				{
					SVCallbackListClass::iterator l_pos = g_svCallbackArray[ i ].GetHeadPosition();

					while( l_pos != g_svCallbackArray[ i ].end() )
					{
						const SVCallbackStruct &l_Data = g_svCallbackArray[ i ].GetAt( l_pos );

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

						g_svCallbackArray[ i ].GetNext( l_pos );
					}
				}
			}

			g_lTriggerData = l_lData;
		}
	}
	catch ( ... )
	{
	}
}

HRESULT WINAPI SVNIInitialize( bool p_bCreate )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	HRESULT l_hrOk = S_OK;
	
	g_sBoardType = 0;

	g_ulTriggerPortCount = 0;
	g_lTriggerEdge = 0xFFFFFFFF;
	g_lTriggerData = 0x00000000;

	g_ulInputCount = 0;
	g_ulInputPortCount = 0;

	g_ulOutputCount = 0;
	g_ulOutputPortCount = 0;
	
	l_hrOk = g_NIInitDABrds( g_sBoardNumber, &g_sBoardType );
	
	if ( l_hrOk == S_OK && p_bCreate )
	{
		switch ( g_sBoardType )
		{
		case 7: // PC-DIO-24
			{
				g_ulInputCount = 8;
				g_ulInputPortCount = 1;
				g_sInputPortArray[0] = 0;

				g_ulOutputCount = 16;
				g_ulOutputPortCount = 2;
				g_sOutputPortArray[0] = 1;
				g_sOutputPortArray[1] = 2;

				break;
			}
		case 12: // PC-DIO-96
			{
				g_ulInputCount = 24;
				g_ulInputPortCount = 3;
				g_sInputPortArray[0] = 0;
				g_sInputPortArray[1] = 1;
				g_sInputPortArray[2] = 2;

				g_ulOutputCount = 72;
				g_ulOutputPortCount = 9;
				g_sOutputPortArray[0] = 3;
				g_sOutputPortArray[1] = 4;
				g_sOutputPortArray[2] = 5;
				g_sOutputPortArray[3] = 6;
				g_sOutputPortArray[4] = 7;
				g_sOutputPortArray[5] = 8;
				g_sOutputPortArray[6] = 9;
				g_sOutputPortArray[7] = 10;
				g_sOutputPortArray[8] = 11;

				break;
			}
		case 35: // DAQCard DIO-24
			{
				g_ulInputCount = 8;
				g_ulInputPortCount = 1;
				g_sInputPortArray[0] = 0;

				g_ulOutputCount = 16;
				g_ulOutputPortCount = 2;
				g_sOutputPortArray[0] = 1;
				g_sOutputPortArray[1] = 2;

				break;
			}
		case 65: // PC-DIO-24/PnP
			{
				g_ulInputCount = 8;
				g_ulInputPortCount = 1;
				g_sInputPortArray[0] = 0;

				g_ulOutputCount = 16;
				g_ulOutputPortCount = 2;
				g_sOutputPortArray[0] = 1;
				g_sOutputPortArray[1] = 2;

				break;
			}
		case 66: // PC-DIO-96/PnP
			{
				g_ulInputCount = 24;
				g_ulInputPortCount = 3;
				g_sInputPortArray[0] = 0;
				g_sInputPortArray[1] = 1;
				g_sInputPortArray[2] = 2;

				g_ulOutputCount = 72;
				g_ulOutputPortCount = 9;
				g_sOutputPortArray[0] = 3;
				g_sOutputPortArray[1] = 4;
				g_sOutputPortArray[2] = 5;
				g_sOutputPortArray[3] = 6;
				g_sOutputPortArray[4] = 7;
				g_sOutputPortArray[5] = 8;
				g_sOutputPortArray[6] = 9;
				g_sOutputPortArray[7] = 10;
				g_sOutputPortArray[8] = 11;

				break;
			}
		case 200: // PCI-DIO-96
			{
				g_ulInputCount = 24;
				g_ulInputPortCount = 3;
				g_sInputPortArray[0] = 0;
				g_sInputPortArray[1] = 1;
				g_sInputPortArray[2] = 2;

				g_ulOutputCount = 72;
				g_ulOutputPortCount = 9;
				g_sOutputPortArray[0] = 3;
				g_sOutputPortArray[1] = 4;
				g_sOutputPortArray[2] = 5;
				g_sOutputPortArray[3] = 6;
				g_sOutputPortArray[4] = 7;
				g_sOutputPortArray[5] = 8;
				g_sOutputPortArray[6] = 9;
				g_sOutputPortArray[7] = 10;
				g_sOutputPortArray[8] = 11;

				break;
			}
		case 256: // PCI-6503
			{
				g_ulInputCount = 8;
				g_ulInputPortCount = 1;
				g_sInputPortArray[0] = 0;

				g_ulOutputCount = 16;
				g_ulOutputPortCount = 2;
				g_sOutputPortArray[0] = 1;
				g_sOutputPortArray[1] = 2;

				break;
			}
		case 285: // PCI-6527
			{
				g_ulTriggerPortCount = 1;
				g_sTriggerPortArray[0] = 0;
				g_lTriggerEdge = 0xFFFFFFFF;
				g_lTriggerData = 0x00000000;

				g_ulInputCount = 16;
				g_ulInputPortCount = 2;
				g_sInputPortArray[0] = 1;
				g_sInputPortArray[1] = 2;

				g_ulOutputCount = 24;
				g_ulOutputPortCount = 3;
				g_sOutputPortArray[0] = 3;
				g_sOutputPortArray[1] = 4;
				g_sOutputPortArray[2] = 5;

				break;
			}
		default :
			{
				l_hrOk = S_FALSE;

				break;
			}
		}
	}
	
	return l_hrOk;
}


HRESULT WINAPI SVNIDestroy( bool p_bClose = false )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &g_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &g_lRefCount, 0 );

		int l_iSize = g_svCallbackArray.GetSize();

		for ( int i = l_iSize - 1; 0 <= i ; i-- )
		{
			SVTriggerStop( i );
			SVTriggerUnregisterAll( i );
		}

		g_svCallbackArray.SetSize( 0 );

		if ( 0 < g_ulTriggerPortCount )
		{
			if ( g_NIDIGChangeMessageControl( g_sBoardNumber, 1 ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			if ( g_NIDIGChangeMessageConfig( g_sBoardNumber, 0, NULL, NULL, NULL, 0, 0 ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}

		if ( SVNIInitialize( false ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &g_lRefCount ) == 1 )
	{
		l_hrOk = SVNIInitialize( true );

		if ( l_hrOk == S_OK )
		{
			// Trigger Section
			if ( 0 < g_ulTriggerPortCount ) 
			{
				short i = 0;

				g_svCallbackArray.SetSize( 0 );

				l_hrOk = g_NIDIGPrtConfig( g_sBoardNumber, 0, 0, 0 );

				if ( l_hrOk == S_OK )
				{
					l_hrOk = g_NIDIGFilterConfig( g_sBoardNumber, 1, "0.0:7", 0.0);
				}
				
				if ( l_hrOk == S_OK )
				{
					l_hrOk = g_NIDIGChangeMessageConfig( g_sBoardNumber, 1, "0.0:7", "0.0:7",
																							 NULL, 0, (unsigned long)g_SVIONICallback );
				}
				
				if ( l_hrOk == S_OK )
				{
					g_svCallbackArray.SetSize( 8 );

					for ( int i = 0; i < 8; i++ )
					{
						g_svCallbackArray[i].m_csName.Format( "IO_Board_1.Dig_%d", i );
					}
				}
			}

			// Input Section
			if ( l_hrOk == S_OK )
			{
				for ( unsigned long i = 0; l_hrOk == S_OK && i < g_ulInputPortCount; i++ )
				{
					l_hrOk = g_NIDIGPrtConfig( g_sBoardNumber, g_sInputPortArray[i], 0, 0 );
				}
			}

			// Output Section
			if ( l_hrOk == S_OK )
			{
				for ( unsigned long i = 0; l_hrOk == S_OK && i < g_ulOutputPortCount; i++ )
				{
					l_hrOk = g_NIDIGPrtConfig( g_sBoardNumber, g_sOutputPortArray[i], 0, 1 );

					g_NIDIGOutPrt( g_sBoardNumber, g_sOutputPortArray[i], 0xFF );
				}
			}

			if ( l_hrOk != S_OK )
			{
				SVNIDestroy( true );
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = SVNIDestroy();

	return l_hrOk;
}

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_ulInputCount;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbValue != NULL )
	{
		if ( 0 <= p_ulChannel && p_ulChannel < g_ulInputCount )
		{
			short l_sPort = g_sInputPortArray[ p_ulChannel / 8 ];

			if ( 0 <= l_sPort )
			{
				short l_sState = 0;
				short l_sLine = (short)(p_ulChannel % 8);

				l_hrOk = g_NIDIGInLine( g_sBoardNumber, l_sPort, l_sLine, &l_sState );

				if ( l_hrOk == S_OK )
				{
					*p_pbValue = l_sState == 1;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVInputGetPortCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_ulInputPortCount;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulValue != NULL )
	{
		if ( 0 <= p_ulPort && p_ulPort < g_ulInputCount )
		{
			short l_sPort = g_sInputPortArray[ p_ulPort ];

			if ( 0 <= l_sPort )
			{
				long l_lState = 0;

				l_hrOk = g_NIDIGInPrt( g_sBoardNumber, l_sPort, &l_lState );

				if ( l_hrOk == S_OK )
				{
					*p_pulValue = l_lState;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_ulOutputCount;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 <= p_ulChannel && p_ulChannel < g_ulOutputCount )
	{
		short l_sPort = g_sOutputPortArray[ p_ulChannel / 8 ];

		if ( 0 <= l_sPort )
		{
			short l_sLine = (short)(p_ulChannel % 8);
			short l_sState = 0;

			if ( p_bValue )
			{
				l_sState = 1;
			}

			l_hrOk = g_NIDIGOutLine( g_sBoardNumber, l_sPort, l_sLine, l_sState );
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_ulOutputPortCount;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 <= p_ulPort && p_ulPort < g_ulOutputCount )
	{
		short l_sPort = g_sOutputPortArray[ p_ulPort ];

		if ( 0 <= l_sPort )
		{
			long l_lState = (long)p_ulValue;

			l_hrOk = g_NIDIGOutPrt( g_sBoardNumber, l_sPort, l_lState );
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_svCallbackArray.GetSize();

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHandle != NULL )
	{
		if ( p_ulIndex < (unsigned long)g_svCallbackArray.GetSize() )
		{
			l_hrOk = S_OK;

			*p_pulHandle = p_ulIndex + 1;
		}
		else
		{
			*p_pulHandle = NULL;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		l_hrOk = S_OK;

		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		*p_pbstrName = g_svCallbackArray[ p_ulHandle - 1 ].m_csName.AllocSysString();
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVNICallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		l_hrOk = S_OK;

		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		SVCallbackListClass::iterator l_pos = g_svCallbackArray[ p_ulHandle - 1 ].Find( l_Callback );
		if ( l_pos != g_svCallbackArray[ p_ulHandle - 1 ].end() )
		{
			g_svCallbackArray[ p_ulHandle - 1 ].SetAt( l_pos, l_Callback );
		}
		else
		{
			g_svCallbackArray[ p_ulHandle - 1 ].AddTail( l_Callback );
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVNICallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		l_hrOk = S_OK;

		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		SVCallbackListClass::iterator l_pos = g_svCallbackArray[ p_ulHandle - 1 ].Find( l_Callback );
		if ( l_pos != g_svCallbackArray[ p_ulHandle - 1 ].end() )
		{
			g_svCallbackArray[ p_ulHandle - 1 ].RemoveAt( l_pos );
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		l_hrOk = S_OK;

		g_svCallbackArray[ p_ulHandle - 1 ].RemoveAll();
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		if ( ! g_svCallbackArray.m_bIsStarted )
		{
			l_hrOk = g_NIDIGInPrt( g_sBoardNumber, 0, &g_lTriggerData );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = g_NIDIGChangeMessageControl( g_sBoardNumber, 0 );
			}

			g_svCallbackArray.m_bIsStarted = l_hrOk == S_OK;
		}

		if ( g_svCallbackArray.m_bIsStarted )
		{
			g_svCallbackArray[ p_ulHandle - 1 ].m_bIsStarted = true;

			l_hrOk = S_OK;
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		if ( g_svCallbackArray.m_bIsStarted )
		{
			l_hrOk = g_NIDIGChangeMessageControl( g_sBoardNumber, 1 );

			g_svCallbackArray.m_bIsStarted = false;
		}
		else
		{
			l_hrOk = S_OK;
		}

		g_svCallbackArray[ p_ulHandle - 1 ].m_bIsStarted = false;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
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

HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
		{
			// SVSignalEdge and SVBoardVersion enums are used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			if ( p_ulIndex == SVSignalEdge )
			{
				*p_pbstrName = ::SysAllocString( L"Edge" );

			}

			if( p_ulIndex == SVBoardVersion )
			{
				*p_pbstrName = ::SysAllocString( L"Board Version");
			}

			if ( *p_pbstrName != NULL )
			{
				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
			{
				// SVSignalEdge and SVBoardVersion enums are used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				if ( p_ulIndex == SVSignalEdge )
				{
					long l_lMask = 1 << ( p_ulHandle - 1 );

					p_pvarValue->vt = VT_I4;

					if ( ( g_lTriggerEdge & l_lMask ) == l_lMask )
					{
						p_pvarValue->lVal = -1;
					}
					else
					{
						p_pvarValue->lVal = 1;
					}

					l_hrOk = S_OK;
				}
				if( p_ulIndex == SVBoardVersion )
				{
					unsigned long l_lSerial;
					WCHAR wbuf[256];
					l_hrOk = g_NIGetDAQDeviceInfo( g_sBoardNumber, ND_DEVICE_SERIAL_NUMBER, &l_lSerial);
					swprintf( wbuf, L"NI PCI-6527 Serial 0x%x", l_lSerial );
					p_pvarValue->vt = VT_BSTR;
					p_pvarValue->bstrVal = ::SysAllocString( wbuf );
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_svCallbackArray.GetSize() )
	{
		if ( p_pvarValue != NULL )
		{
			// SVSignalEdge enum is used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			if ( p_ulIndex == SVSignalEdge )
			{
				if ( p_pvarValue->vt == VT_I4 )
				{
					long l_lMask = 1 << ( p_ulHandle - 1 );

					switch ( p_pvarValue->lVal )
					{
						case -1:
						{
							g_lTriggerEdge |= l_lMask;

							l_hrOk = S_OK;

							break;
						}
						case 1:
						{
							g_lTriggerEdge &= ~l_lMask;

							l_hrOk = S_OK;

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
	}

	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVNI\SVNIExports.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:26:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Nov 2010 15:50:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   15 Dec 2009 12:43:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   20 Mar 2006 15:02:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added comment about enum usage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Mar 2006 10:55:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVNI Dll to return a parameter for board version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Mar 2006 10:54:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVNI Dll to return a parameter for board version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Jan 2004 07:22:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   set thread priority in callback handling;
 * added trigger parameter functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Dec 2003 07:55:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated prt operation functions from short to long.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Dec 2003 11:11:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added two new global variables to handle the previous state of the inputs to determine which input changed state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Oct 2003 11:11:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated SVCreate function to remove one of the supported board types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Oct 2003 12:38:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Oct 2003 10:25:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated code to correct problems with DIO board support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Sep 2003 13:53:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated SVCreate fucntion to change the name of the Trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Aug 2003 07:39:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVCreate function to initialize output to an off state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2003 16:12:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated callback information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:31:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the National Instruments DAQ Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
