// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOMEExports
// * .File Name       : $Workfile:   SVMEExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:44:48  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVMEExports.h"

#include "SVOMFCLibrary/SVCallbackListArrayClass.h"
#include "SVME14.h"

#include <medefs.h>

int g_iBoardNumber = 0;
int g_iBoardType = 0;

long g_lRefCount = 0;

unsigned long g_ulInputCount = 0;
unsigned long g_ulInputPortCount = 0;

unsigned long g_ulOutputCount = 0;
unsigned long g_ulOutputPortCount = 0;

SVCallbackListArrayClass g_svCallbackArray;


void _stdcall g_SVIOMECallback( void )
{

	try
	{
		if ( 0 < g_svCallbackArray.GetSize() ) 
		{
			SVCallbackListClass::iterator l_pos = g_svCallbackArray[ 0 ].GetHeadPosition();

			while ( l_pos != g_svCallbackArray[ 0 ].end() )
			{
				const SVCallbackStruct &l_Data = g_svCallbackArray[ 0 ].GetAt( l_pos );

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

				g_svCallbackArray[ 0 ].GetNext( l_pos );
			}
		}
	}
	catch ( ... )
	{
	}
}

HRESULT WINAPI SVMEDestroy( bool p_bClose = false )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &g_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &g_lRefCount, 0 );

		SVTriggerStop( 0 );
		SVTriggerUnregisterAll( 0 );

		g_svCallbackArray.SetSize( 0 );

		g_ulInputCount = 0;
		g_ulInputPortCount = 0;
		g_ulOutputCount = 0;
		g_ulOutputPortCount = 0;
	}

	return l_hrOk;
}

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &g_lRefCount ) == 1 )
	{
		int l_iBoardVersion = 0;

		if ( g_ME14GetBoardVersion( g_iBoardNumber, &l_iBoardVersion ) )
		{
			switch ( l_iBoardVersion )
			{
				case 0x140:
				case 0x14A:
				case 0x14B:
				{
					g_iBoardType = 1;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					g_iBoardType = 0;

					l_hrOk = S_FALSE;

					break;
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		if ( l_hrOk == S_OK )
		{
			// Trigger Section
			if ( l_hrOk == S_OK ) 
			{
				g_svCallbackArray.SetSize( 1 );

				g_svCallbackArray[0].m_csName.Format( "IO_Board_1.Dig_%d", 0 );
			}

			// Input Section
			if ( l_hrOk == S_OK )
			{
				switch ( g_iBoardType )
				{
					case 1:
					{
						//  Port A is used as Input Port ...
						//	... Module Input ( SVIM IN 1-8 )
						if( ! g_ME14DIOSetPortDirection( g_iBoardNumber, PORTA, MEINPUT ) )
						{
							l_hrOk = S_FALSE;
						}

						break;
					}
				}

				if ( l_hrOk == S_OK )
				{
					g_ulInputCount = 8;
					g_ulInputPortCount = 1;
				}
			}

			// Output Section
			if ( l_hrOk == S_OK )
			{
				switch ( g_iBoardType )
				{
					case 1:
					{
						//  Port B is used as Output Port ...
						//	... Result Output ( SVIM OUT 1-8 )
						if( ! g_ME14DIOSetPortDirection( g_iBoardNumber, PORTB, MEOUTPUT ) )
						{
							l_hrOk = S_FALSE;
						}

						if ( l_hrOk == S_OK )
						{
							//  Port C is used as Output Port ...
							//	... Result Output ( SVIM OUT 9-16 )
							if( g_ME14DIOSetPortDirection( g_iBoardNumber, PORTCL, MEOUTPUT ) )
							{
								if( ! g_ME14DIOSetPortDirection( g_iBoardNumber, PORTCH, MEOUTPUT ) )
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
							g_ME14DOSetByte( g_iBoardNumber, PORTB, 0xFF );
							g_ME14DOSetByte( g_iBoardNumber, PORTC, 0xFF );
						}				

						break;
					}
				}

				if ( l_hrOk == S_OK )
				{
					g_ulOutputCount = 16;
					g_ulOutputPortCount = 2;
				}
			}

			if ( l_hrOk != S_OK )
			{
				SVMEDestroy( true );
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = SVMEDestroy();

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
			int l_iPort = -1;

			switch ( p_ulChannel / 8 )
			{
				case 0:
				{
					l_iPort = PORTA;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					break;
				}
			}

			if ( l_hrOk == S_OK )
			{
				int l_iState = 0;
				int l_iLine = (int)(p_ulChannel % 8);

				switch ( g_iBoardType )
				{
					case 1:
					{
						if ( ! g_ME14DIGetBit( g_iBoardNumber, l_iPort, l_iLine, &l_iState ) )
						{
							l_hrOk = S_FALSE;
						}

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
					*p_pbValue = l_iState == 1;
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
		if ( 0 <= p_ulPort && p_ulPort < g_ulInputPortCount )
		{
			int l_iPort = -1;

			switch ( p_ulPort )
			{
				case 0:
				{
					l_iPort = PORTA;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					break;
				}
			}

			if ( l_hrOk == S_OK )
			{
				int l_iState = 0;

				switch ( g_iBoardType )
				{
					case 1:
					{
						if ( ! g_ME14DIGetByte( g_iBoardNumber, l_iPort, &l_iState ) )
						{
							l_hrOk = S_FALSE;
						}

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
					*p_pulValue = l_iState;
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
		int l_iPort = -1;

		switch ( p_ulChannel / 8 )
		{
			case 0:
			{
				l_iPort = PORTB;

				l_hrOk = S_OK;

				break;
			}
			case 1:
			{
				l_iPort = PORTC;

				l_hrOk = S_OK;

				break;
			}
			default:
			{
				break;
			}
		}

		if ( l_hrOk == S_OK )
		{
			int l_iLine = (int)(p_ulChannel % 8);
			int l_iState = 0;

			if ( p_bValue )
			{
				l_iState = 1;
			}

			switch ( g_iBoardType )
			{
				case 1:
				{
					if ( ! g_ME14DOSetBit( g_iBoardNumber, l_iPort, l_iLine, l_iState ) )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				default:
				{
					l_hrOk = S_FALSE;

					break;
				}
			}
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

	if ( 0 <= p_ulPort && p_ulPort < g_ulOutputPortCount )
	{
		int l_iPort = -1;

		switch ( p_ulPort )
		{
			case 0:
			{
				l_iPort = PORTB;

				l_hrOk = S_OK;

				break;
			}
			case 1:
			{
				l_iPort = PORTC;

				l_hrOk = S_OK;

				break;
			}
			default:
			{
				break;
			}
		}

		if ( l_hrOk == S_OK )
		{
			int l_iState = (int)p_ulValue;

			switch ( g_iBoardType )
			{
				case 1:
				{
					if ( ! g_ME14DOSetByte( g_iBoardNumber, l_iPort, l_iState ) )
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				default:
				{
					l_hrOk = S_FALSE;

					break;
				}
			}
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

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVMECallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
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

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVMECallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
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
		switch ( g_iBoardType )
		{
			case 1:
			{
				if ( g_ME14EnableInt( g_iBoardNumber, g_SVIOMECallback, 1 ) )
				{
					l_hrOk = S_OK;
				}

				break;
			}
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
		switch ( g_iBoardType )
		{
			case 1:
			{
				if ( g_ME14DisableInt( g_iBoardNumber, 1 ) )
				{
					l_hrOk = S_OK;
				}

				break;
			}
		}
	} 

	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVME\SVMEExports.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:44:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Nov 2010 15:48:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2009 12:32:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Oct 2003 12:42:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Sep 2003 13:55:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated SVCreate fucntion to change the name of the Trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Aug 2003 07:42:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVCreate function to initialize output to an off state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2003 15:37:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated callback information and changed calling convention type for Meilhaus functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:27:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the Meilhaus Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
