//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCInterfaceClass
//* .File Name       : $Workfile:   SVPLCInterfaceClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:35:50  $
//******************************************************************************

#include "stdafx.h"
#include <vector>
#include "PLCBaseClass.h"
#include "SetupDialogSheet.h"
#include "SVPLCInterfaceClass.h"
#include "SVASABtcpMicroLogixAPI.h"
#include "SVASABtcpSLC500API.h"

//#define PLC_DEBUG

SVPLCInterfaceClass ThePLCInterface;

int MyConsoleOutput(char *fmt, ...);
CString ErrorString(DWORD err);

#ifdef PLC_DEBUG
	HANDLE __hStdOut = NULL;
#endif

// width and height is the size of console window, if you specify fname,
// the output will also be writton to this file. The file pointer is automatically closed
// when you close the app
void startConsoleWin(int width=80, int height=80, char* fname = NULL);

void startConsoleWin(int width, int height, char* fname)
{
#ifdef PLC_DEBUG
	AllocConsole();
	SetConsoleTitle("Debug Window");
	__hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD co = {width,height};
	SetConsoleScreenBufferSize(__hStdOut, co);
#endif
}

// Use MyConsoleOutput like TRACE0, TRACE1, ... (The arguments are the same as printf)
int MyConsoleOutput(char *fmt, ...)
{
#ifdef PLC_DEBUG
	char s[300];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);

	DWORD cCharsWritten;

	if(__hStdOut)
		WriteConsole(__hStdOut, s, strlen(s), &cCharsWritten, NULL);

	return(cnt);
#else
	return 0;
#endif
}


SVPLCInterfaceClass::SVPLCInterfaceClass(void)
:svmhRSLinxLib(NULL)
,svmhApplicomLib(NULL)
, m_lHeartBeatTime(15)
, m_lHeartBeatData(0)
, m_hHeartBeatThread(NULL)
, m_hResetEvent(NULL)
, m_hStopEvent(NULL)
,m_bHeartBeatThreadRunning(FALSE)
{
//	svmhRSLinxLib = LoadLibrary(_T("dtl32.dll"));
//	svmhApplicomLib = LoadLibrary(_T("Applicom.dll"));
	startConsoleWin();
}

SVPLCInterfaceClass::~SVPLCInterfaceClass(void)
{
	
	// Close heartbeat thread first because it uses the first
	// plcSender that is in the m_PLCmap for sending the heartbeat. 
	CloseHeartBeatThread();

	if( m_PLCmap.size() > 0 )
	{
		SVPLCMapType::iterator l_it;
		for( l_it = m_PLCmap.begin() ; l_it != m_PLCmap.end() ; ++l_it )
		{
			l_it->second.StopThread();
			l_it->second.m_pPLC->Close();
			delete l_it->second.m_pPLC ;
			MyConsoleOutput(_T("Deleted PLC Sender\n"));
		}
		m_PLCmap.clear();
	}


	if( svmhRSLinxLib )
	{
		FreeLibrary( svmhRSLinxLib );
	}
	if( svmhApplicomLib )
	{
		FreeLibrary( svmhApplicomLib );
	}
   OutputDebugString( _T("Exiting ~SVPLCInterfaceClass\n") );
#ifdef PLC_DEBUG
   FreeConsole();
#endif
}

HRESULT SVPLCInterfaceClass::SetupConnection( LPTSTR p_szConnection, long p_lSize )
{
   long  lErr;
   long  lConnectionParameterStringSize;

   // Some defaults if connection string is missing them
	_tcscpy( m_ConnectionPars.svmczManufacturer, SV_ALLENBRADLEY);
	_tcscpy( m_ConnectionPars.svmczConnection, SV_ASABTCP );
	_tcscpy( m_ConnectionPars.svmczPLCType, SV_MICROLOGIX );
	m_ConnectionPars.svmPLCManufacturer = SV_PLCMANUFACTURER_ALLENBRADLEY;
	m_ConnectionPars.svmPLCModel = SV_PLCMODEL_MICROLOGIX;

   SetupDialogSheetClass SetupDlg (_T("PLC Connection"));

   lErr = 0;

   while (1)
   {
      lErr = ParseSetupParameterString (p_szConnection);

      if (lErr < 0)
         break;
	  SetupDlg.m_pPars = &m_ConnectionPars;
      SetupDlg.svmChannelNbr.Format (m_ConnectionPars.svmczChannelNbr);
      SetupDlg.svmEquipmentNbr.Format (m_ConnectionPars.svmczEquipmentNbr);
      SetupDlg.svmManufacturer.Format (m_ConnectionPars.svmczManufacturer);
      SetupDlg.svmPLCType.Format (m_ConnectionPars.svmczPLCType);
      SetupDlg.svmConnection.Format (m_ConnectionPars.svmczConnection);
      SetupDlg.svmDriverName.Format (m_ConnectionPars.svmczDriverName);
      SetupDlg.svmIPAddress.Format (m_ConnectionPars.svmczIPAddress);
      SetupDlg.svmDHNodeAddress.Format (m_ConnectionPars.svmczDHNodeAddress);
      SetupDlg.svmConnectionParameters.Format (m_ConnectionPars.svmczConnectionParameters);

	  // Heart Beat Stuff...
	  SetupDlg.m_lHeartBeatTime = m_lHeartBeatTime ;
	  SetupDlg.m_strHeartBeatAddress = m_strHeartBeatAddr ;

	  //TRB Move SetupConnection
      SetupDlg.svmhRSLinxLib = svmhRSLinxLib;
      SetupDlg.svmhApplicomLib = svmhApplicomLib;

      SetupDlg.SetWizardMode ();

      if ( SetupDlg.DoModal () != ID_WIZFINISH  )
      {
         lErr = -1322;
         break;
      }

      lConnectionParameterStringSize = SetupDlg.svmConnectionParameters.GetLength ();
      if (p_lSize <= lConnectionParameterStringSize)
      {
         lErr = -1334;
      }

      sv_tstrncpy (p_szConnection, 
                   SetupDlg.svmConnectionParameters, 
                   lConnectionParameterStringSize);

	  // Heart Beat Stuff...
	  m_lHeartBeatTime = SetupDlg.m_lHeartBeatTime;
	  m_strHeartBeatAddr = SetupDlg.m_strHeartBeatAddress;
      break;
   }

   return lErr;
}

long SVPLCInterfaceClass::ParseSetupParameterString (TCHAR* apszConnection)
{

   long  lErr;

   lErr = 0;

   while (1)
   {

      lErr = GetValue (apszConnection,
                       SV_CHANNEL_NBR,
                       m_ConnectionPars.svmczChannelNbr,
                       SV_NBRCHARS (m_ConnectionPars.svmczChannelNbr));
      if (lErr < 0)
         break;
  
      lErr = GetValue (apszConnection,
                       SV_EQUIPMENT_NBR,
                       m_ConnectionPars.svmczEquipmentNbr,
                       SV_NBRCHARS (m_ConnectionPars.svmczEquipmentNbr));
      if (lErr < 0)
         break;
      
      lErr = GetValue (apszConnection,
                       SV_MANUFACTURER,
                       m_ConnectionPars.svmczManufacturer,
                       SV_NBRCHARS (m_ConnectionPars.svmczManufacturer));
      if (lErr < 0)
         break;
	  if( lErr == 1 )	// Default Manufacturer..
	  {
		  _tcscpy( m_ConnectionPars.svmczManufacturer, SV_ALLENBRADLEY );
	  }

      lErr = GetValue (apszConnection,
                       SV_PLC_TYPE,
                       m_ConnectionPars.svmczPLCType,
                       SV_NBRCHARS (m_ConnectionPars.svmczPLCType));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_CONNECTION_TYPE,
                       m_ConnectionPars.svmczConnection,
                       SV_NBRCHARS (m_ConnectionPars.svmczConnection));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_DRIVER_NAME,
                       m_ConnectionPars.svmczDriverName,
                       SV_NBRCHARS (m_ConnectionPars.svmczDriverName));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_IP_ADDRESS,
                       m_ConnectionPars.svmczIPAddress,
                       SV_NBRCHARS (m_ConnectionPars.svmczIPAddress));
      if (lErr < 0)
         break;
      
      lErr = GetValue (apszConnection,
                       SV_DHNODE_ADDRESS,
                       m_ConnectionPars.svmczDHNodeAddress,
                       SV_NBRCHARS (m_ConnectionPars.svmczDHNodeAddress));
      if (lErr < 0)
         break;
	  else
		  lErr = 0;

      break;
   }

   return lErr;
}


long SVPLCInterfaceClass::GetValue (TCHAR*   apczSourceString, 
                               TCHAR*   apczKeyString, 
                               TCHAR*   apczValueString,
                               long     alLength)
{
   TCHAR*   pntr1;
   TCHAR*   pntr2;
   TCHAR    czWhiteSpace [40];

   long  lFound;
   long  lErr;
   long  lIndex;

   lFound = FALSE;
   lErr = 0;

   while (1)
   {
//--- Yes, I am treating a comma as white space.
      _tcscpy (czWhiteSpace, _T("\a\b\f\n\r\t\v ,"));
      pntr1 = apczSourceString;

      apczValueString [0] = 0;

      while ((pntr1 = _tcsstr (pntr1, 
                               apczKeyString)) != NULL)
      {
         pntr2 = _tcsstr (pntr1, _T("="));

         if (pntr2 == NULL)
         {
            lErr = -1317;
            break;
         }

         if ((pntr1 != apczSourceString) &&
             (_tcschr(czWhiteSpace, *(pntr1 - 1)) == NULL))
         {
//--------- If we are not at the beginning of the string, and the previous
//--------- character is not whitespace, then this is not the correct key.
            pntr1 = pntr1 + 1;
            continue;
         }

//------ Move passed the currently found string.
         pntr1 = pntr1 + _tcslen (apczKeyString);

//------ Move passed any white space.
         pntr1 = pntr1 + _tcsspn (pntr1, czWhiteSpace);

         if (pntr2 != pntr1)
         {
//--------- If we are not looking at an equal sign... 
            pntr1 = pntr2;
            continue;
         }

//------ Move passed equal sign.
         pntr1 = pntr1 + 1;

//------ Move passed any white space.
         pntr1 = pntr1 + _tcsspn (pntr1, czWhiteSpace);

         lIndex = 0;
         while ((_tcschr(czWhiteSpace, *pntr1) == NULL) &&
                (lIndex < (alLength - 1)))
         {
            apczValueString [lIndex] = *pntr1;
            lIndex = lIndex + 1;
            pntr1 = pntr1 + 1;
         }

         apczValueString [lIndex] = 0;

         lFound = TRUE;
      } // while ((pntr1 = _tcsstr (pntr1, apczKeyString)) != NULL)

      break;
   } // while (1)

   if( lFound == 0 && lErr == 0) // Not Found return.
   {
	   lErr = 1;
   }
   return lErr;
}


HRESULT SVPLCInterfaceClass::Construct( LPCTSTR p_szPLCID, LPTSTR p_szConnection )
{
	// Create the PLC here through the parameter string.
	long l_hr = -4123;
	ParseSetupParameterString( p_szConnection );

	SVPLCBaseClass* l_pCurrentPLC = NULL;
	SVPLCMapType::iterator l_it = m_PLCmap.find( p_szPLCID );
	if( l_it != m_PLCmap.end() )
	{
		l_pCurrentPLC = l_it->second.m_pPLC;
	}

	if( _tcsstr(m_ConnectionPars.svmczConnection, SV_ASABTCP) != NULL )
	{
		CString l_strAddress = m_ConnectionPars.svmczIPAddress;
		if( _tcsstr( m_ConnectionPars.svmczPLCType, SV_SLC500 ))
		{
			// clean up the last plc.
			if( l_pCurrentPLC )
			{
				if( l_it == m_PLCmap.begin() )
				{
					MyConsoleOutput(_T("Closing %s\n"), p_szPLCID );
					CloseHeartBeatThread();
				}
				MyConsoleOutput(_T("Closing %s\n"), p_szPLCID );
				l_pCurrentPLC->Close();
				delete l_pCurrentPLC;
				l_pCurrentPLC = NULL;
			}
			// Construct a ASAB tcp SLC500 PLC
			MyConsoleOutput(_T("new SVASABtcpSLC500API %s\n"), p_szPLCID );
			SVASABtcpSLC500API* l_pPLC = new SVASABtcpSLC500API(&l_hr);
			l_hr = l_pPLC->Open( m_ConnectionPars.svmczIPAddress );
			if( l_pPLC && l_hr == S_OK )
			{
				l_hr = l_pPLC->SetupPLC(m_ConnectionPars.svmczIPAddress, 2000);
				if( l_hr == S_OK )
				{
					m_PLCmap[ p_szPLCID ].m_pPLC = l_pPLC;
					m_PLCmap[ p_szPLCID ].StartThread( p_szPLCID );
				}
				else
				{
					AfxMessageBox(_T("Failed to SetupPLC SLC500 PLC"));
				}

			}
			else
			{
				delete l_pPLC;
				CString strTmp =  _T("Failed to Open a SLC500 PLC ") + ErrorString( l_hr );
				AfxMessageBox( strTmp );
			}
		}
		else
		if( _tcsstr( m_ConnectionPars.svmczPLCType, SV_MICROLOGIX ))
		{
			// clean up the last plc.
			if( l_pCurrentPLC )
			{
				if( l_it == m_PLCmap.begin() )
				{
					MyConsoleOutput(_T("Closing %s\n"), p_szPLCID );
					CloseHeartBeatThread();
				}
				MyConsoleOutput(_T("Closing %s\n"), p_szPLCID );
				l_pCurrentPLC->Close();
				delete l_pCurrentPLC;
				l_pCurrentPLC = 0;
			}
			// Construct a ASAB tcp MicroLogix PLC
			MyConsoleOutput(_T("new SVASABtcpMicroLogixAPI %s\n"), p_szPLCID );
			SVASABtcpMicroLogixAPI* l_pPLC = new SVASABtcpMicroLogixAPI(&l_hr);
			if( l_pPLC )
			{
				l_hr = l_pPLC->Open( m_ConnectionPars.svmczIPAddress );
				if( l_pPLC && l_hr == S_OK )
				{
					l_hr = l_pPLC->SetupPLC(m_ConnectionPars.svmczIPAddress, 1000);
					if( l_hr == S_OK )
					{
						m_PLCmap[ p_szPLCID ].m_pPLC = l_pPLC;
						m_PLCmap[ p_szPLCID ].StartThread( p_szPLCID );
					}
					else
					{
						AfxMessageBox(_T("Failed to SetupPLC MicroLogix PLC"));
					}
				}
				else
				{
					delete l_pPLC;

					CString strTmp =  _T("Failed to Open MicroLogix PLC ") + ErrorString( l_hr );
					AfxMessageBox( strTmp );
				}
			}
			else
			{
				l_hr = -4107;
			}
		}
	}

	// Start the Heart Beat Thread if not already running.
	if( l_hr == S_OK )
	{
		if( m_hHeartBeatThread == NULL && m_hStopEvent == NULL && m_hResetEvent == NULL )
		{
			m_hStopEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
			m_hResetEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
			DWORD dwThreadID;
			m_hHeartBeatThread = ::CreateThread(NULL, 0, SVHeartBeatThreadFunc, (LPVOID)this, 0, &dwThreadID );
			SVThreadManager::Instance().Add(m_hHeartBeatThread, _T("PLC Heartbeat Thread"));
		}
	}

	
	return l_hr;
}

DWORD WINAPI SVHeartBeatThreadFunc( LPVOID lpParam )
{
	DWORD dwResult;
	BOOL bRunning = TRUE;
	SVPLCInterfaceClass* l_pPLCInterface= (SVPLCInterfaceClass*)lpParam;
	::InterlockedExchange( reinterpret_cast<long*>(&l_pPLCInterface->m_bHeartBeatThreadRunning), TRUE);
	HANDLE aHandles[2] = { l_pPLCInterface->m_hStopEvent, l_pPLCInterface->m_hResetEvent };
	do
	{
		// Wait for any signal from another thread
		dwResult = ::WaitForMultipleObjectsEx( 2, aHandles, FALSE, l_pPLCInterface->m_lHeartBeatTime * 1000, FALSE );

		switch( dwResult )
		{
			case WAIT_OBJECT_0 :		// Stop Thread Event
				bRunning = FALSE;
				break;
			case WAIT_TIMEOUT:			// Timeout - Send Heart Beat.
				l_pPLCInterface->SendHeartBeat();
				break;
			case WAIT_OBJECT_0 + 1:		// Heart Beat Reset Event 
				::ResetEvent( l_pPLCInterface->m_hResetEvent );
				break;
			case WAIT_IO_COMPLETION :	// APC calls - Not used.
				break;
			default:
				break;
		}
	}while( bRunning );
	::InterlockedExchange(reinterpret_cast<long*>(&l_pPLCInterface->m_bHeartBeatThreadRunning), FALSE );
	MyConsoleOutput(_T("Stopped Heartbeat Thread\n") );

	return 0;
}


void SVPLCInterfaceClass::Destroy( LPCTSTR p_szPLCID )
{
	MyConsoleOutput(_T("Enter Destroy %s\n"), p_szPLCID );
	// Close heartbeat
	SVPLCMapType::iterator l_it = m_PLCmap.find( p_szPLCID );
	if( l_it != m_PLCmap.end() )
	{
		// If this is the last plc to close then we better stop the 
		// heartbeat since it uses the first plc in the map.
		if( l_it == m_PLCmap.begin() )
		{
			CloseHeartBeatThread();
			Sleep(1);
		}
		l_it->second.StopThread();
		Sleep(1);
		l_it->second.m_pPLC->Close();
		Sleep(1);

		if( l_it->second.m_pPLC != NULL )
		{
			l_it->second.m_pPLC->TerminateSendAndResponseThreads();
		}
		delete l_it->second.m_pPLC;
		m_PLCmap.erase( l_it );
	}
}

void SVPLCInterfaceClass::CloseHeartBeatThread()
{
	// Heart Beat Thread Cleanup
	if( m_hStopEvent )
	{
		::SetEvent( m_hStopEvent );
		while( m_bHeartBeatThreadRunning )
			Sleep(1);
		::CloseHandle( m_hStopEvent );
		m_hStopEvent = NULL;
	}

	if( m_hResetEvent )
	{
		::CloseHandle( m_hResetEvent );
		m_hResetEvent = NULL;
	}

	if( m_hHeartBeatThread != NULL )
	{
		SVThreadManager::Instance().Remove(m_hHeartBeatThread);
		::CloseHandle( m_hHeartBeatThread );
		m_hHeartBeatThread = NULL;
	}
	m_strHeartBeatAddr.Empty();
	OutputDebugString( _T( "Closed Heartbeat Thread\n" ));
}

HRESULT SVPLCInterfaceClass::SendBlock ( LPCTSTR p_szPLCID, VARIANT p_vData )
{
	HRESULT l_hr = S_OK;
	if( m_PLCmap.find( p_szPLCID) != m_PLCmap.end() )
	{
		if( m_hResetEvent )
		{
			::SetEvent( m_hResetEvent );
		}
		l_hr = m_PLCmap[p_szPLCID].SendBlock( p_vData );

	}
	else
	{
		::VariantClear( &p_vData );	// Cleanup because the sendblock did not.
		l_hr = -4117;
	}
	return l_hr;

}

HRESULT SVPLCInterfaceClass::SendHeartBeat( )
{
	HRESULT l_hr = -4101;
	// Check for at least one plc to be constructed
	if( m_PLCmap.size() > 0 && m_strHeartBeatAddr.GetLength() > 1)
	{
		long l_lType;
		long l_lSize;
		long l_lElements;
		_bstr_t l_bstVal(m_strHeartBeatAddr);
		VARIANT l_vTmp;
		l_vTmp.vt = VT_BSTR;
		l_vTmp.bstrVal = l_bstVal.Detach();
		l_hr = GetDataType( &l_lSize, &l_lType, l_vTmp );
		switch( l_lType )
		{
			case SV_PLC_INT16:
			{
				l_lElements = 2;
				break;
			}
			case SV_PLC_INT32:
			{
				l_lElements = 1;
				break;
			}
			default :
			{
				l_lElements = 2;
				break;
			}
		}

		////////////////////////// Build Safearray of variants..
		SAFEARRAY* psaData;
		SAFEARRAYBOUND l_rgsbound[2];
		l_rgsbound[0].cElements = 2 * l_lElements;	// 
		l_rgsbound[0].lLbound = 0;
		l_rgsbound[1].cElements = 2;
		l_rgsbound[1].lLbound = 0;
		psaData = ::SafeArrayCreate( VT_VARIANT, 2, l_rgsbound );
		if( psaData && l_hr == S_OK )
		{
			// Counter
			int CountIndex = 0;
			m_lHeartBeatCounter ++;
			for(  ; CountIndex < l_lElements ; CountIndex++ )
			{
				l_hr = SetHeartBeatData( psaData, CountIndex, m_lHeartBeatCounter >> (16 * CountIndex), l_lType );
			}

			if( !m_pfnGetState.empty())
			{
				if( l_hr == S_OK )
				{
					l_hr = m_pfnGetState( &m_lHeartBeatData );
				}
			}

			// Heart Beat Data
			for( int DataIndex = 0 ; DataIndex < l_lElements ; DataIndex++ )
			{

				if( l_hr == S_OK )
				{
					l_hr = SetHeartBeatData( psaData, DataIndex + CountIndex, m_lHeartBeatData >> (16 * DataIndex), l_lType );
				}
			}

			// Add an additonal array to wrap that has type info.
			if( l_hr == S_OK )
			{
				VARIANT l_vData;
				l_vData.vt = VT_ARRAY | VT_VARIANT;
				l_vData.parray = psaData;

				VARIANT l_vPLCData;	// Type & Data.
				SAFEARRAY* l_saPLCData = NULL;
				SAFEARRAYBOUND l_saBound;
				l_saBound.cElements = 2;
				l_saBound.lLbound = 0;
				l_saPLCData = ::SafeArrayCreate( VT_VARIANT, 1, &l_saBound );
				ASSERT( l_saPLCData );

				VARIANT l_vType;	// PLC Data Transfer Type not the actual data type.
				l_vType.vt = VT_I4;
				l_vType.lVal = l_lType;


				long l_lInd;
				l_lInd = 0;
				l_hr = ::SafeArrayPutElement( l_saPLCData, &l_lInd, &l_vType );
				// Clear Variant....
				l_hr = ::VariantClear( &l_vType );

				ASSERT( l_hr == S_OK );
				l_lInd = 1;
				if( l_hr == S_OK )
				{
					l_hr = ::SafeArrayPutElement( l_saPLCData,&l_lInd, &l_vData );
					l_hr = ::VariantClear( &l_vData );
				}
				ASSERT( l_hr == S_OK );

				l_vPLCData.vt = VT_ARRAY | VT_VARIANT;
				l_vPLCData.parray = l_saPLCData;

				if( l_hr == S_OK )
				{
					l_hr = m_PLCmap.begin()->second.SendBlock( l_vPLCData );
				}
			}
		}
	}
	return l_hr;
}

HRESULT SVPLCInterfaceClass::SetHeartBeatData(SAFEARRAY* p_psa, long p_lIndex, long p_lValue, long p_lType )
{
	HRESULT l_hr;
	// First Heart Beat Address
	long l_rgInd[2];
	VARIANT l_vAddress;
	::VariantInit( &l_vAddress );
	_bstr_t l_bstValue = m_strHeartBeatAddr;
	l_rgInd[0] = p_lIndex;
	l_rgInd[1] = 0; // Address is element zero.
	l_vAddress.vt = VT_BSTR;
	l_vAddress.bstrVal = l_bstValue.Detach();
	AddOffsetToAddress( &l_vAddress, p_lIndex, -1 );
	l_hr = ::SafeArrayPutElement( p_psa, l_rgInd, &l_vAddress );
	// clear variant now that we are done.
	::VariantClear( &l_vAddress );

	// Value.
	VARIANT l_vValues;
	::VariantInit( &l_vValues );
	if( p_lType == SV_PLC_INT32 )
	{
		l_vValues.vt = VT_I4;
	}
	else
	{
		l_vValues.vt = VT_I2;
	}

	l_vValues.lVal = p_lValue;
	l_rgInd[1] = 1;
	l_hr = ::SafeArrayPutElement( p_psa, l_rgInd, &l_vValues );
	::VariantClear( &l_vValues );

	return l_hr;
}

HRESULT WINAPI SVPLCInterfaceClass::SetParameterValue( LPCTSTR p_szPLCID, unsigned long p_ulIndex, VARIANT p_varValue )
{
	HRESULT l_hr = -4117 ; // Invalid Index

	switch ( p_ulIndex )
	{
		case 0:	// Queue Depth
		{
			if( p_varValue.vt == VT_I4 )
			{
				if( m_PLCmap.find( p_szPLCID ) != m_PLCmap.end() )
				{
					m_PLCmap[p_szPLCID].m_MaxQueueDepth = p_varValue.lVal;
					l_hr = S_OK;
				}
				else
				{
					l_hr = -4118;
				}
			}
			else
			{
				l_hr = -4119;
			}
			break;
		}
		case 1:	// Heart Beat Address
		{
			if( p_varValue.vt == VT_BSTR )
			{
				// Set heartbeat address
				m_strHeartBeatAddr = p_varValue.bstrVal;
				l_hr = S_OK;
			}
			else
			{
				l_hr = -4119;
			}
			break;
		}
		case 2:	// Heart Beat Time
		{
			if( p_varValue.vt == VT_I4 )
			{
				// Set heartbeat address
				m_lHeartBeatTime = p_varValue.lVal;
				::SetEvent( m_hResetEvent );	// Reset heart beat thread.
				l_hr = S_OK;
			}
			else
			{
				l_hr = -4119;
			}
			break;
		}
		case 3:	// Heart Beat Data
		{
			if( p_varValue.vt == VT_I4 )
			{
				// Set heartbeat data
				m_lHeartBeatData = p_varValue.lVal;
				l_hr = S_OK;
			}
			else
			{
				l_hr = -4119;
			}
			break;
		}
		default:
		{
			l_hr = -4117;
			break;
		}
	}
	::VariantClear( &p_varValue );

	return l_hr;
}


HRESULT WINAPI SVPLCInterfaceClass::GetParameterValue( LPCTSTR p_szPLCID, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hr = -4120; // Invalid index
	switch ( p_ulIndex )
	{
		case 0:// Queue Depth 
		{
			if( m_PLCmap.find( p_szPLCID ) != m_PLCmap.end() )
			{
				p_pvarValue->vt = VT_I4;
				p_pvarValue->lVal = m_PLCmap[p_szPLCID].m_MaxQueueDepth;
				l_hr = S_OK;
			}
			else
			{
				l_hr = -4121 ;
			}
			break;
		}
		case 1:// Heart Beat Address - one for all p_szPLCID
		{
			_bstr_t l_bstVal( m_strHeartBeatAddr );
			p_pvarValue->vt = VT_BSTR;
			p_pvarValue->bstrVal = l_bstVal.Detach();
			l_hr = S_OK;
			break;
		}
		case 2:// Heart Beat Time - one for all p_szPLCID
		{
			p_pvarValue->vt = VT_I4;
			p_pvarValue->lVal = m_lHeartBeatTime;
			l_hr = S_OK;
			break;
		}
		case 3:// Heart Beat Data - This is state data to be sent during the heartbeat
		{
			p_pvarValue->vt = VT_I4;
			p_pvarValue->lVal = m_lHeartBeatData;
			l_hr = S_OK;
			break;
		}

		default:
		{
			l_hr = -4120;
			break;
		}
	}
	return l_hr;
}


// This function should always succeeds. It sets p_plType to SV_PLC_INT16 by default.
// The size is in Bytes and the default is 2 or 16Bit.
HRESULT SVPLCInterfaceClass::GetDataType( long* p_plSize, long* p_plType, VARIANT p_vAddress )
{
	HRESULT l_hr = S_OK;
	SVPLCDataTypeEnum l_eType = SV_PLC_INT16;
	long l_lSize = 2;
	CString l_strAddress;

	if( p_vAddress.vt == VT_BSTR )
	{
		l_strAddress = p_vAddress.bstrVal;
		SysFreeString( p_vAddress.bstrVal );
	}

	if( l_strAddress.GetLength() > 3 )
	{

		switch( l_strAddress[0] )
		{
			case _T('S'):
			case _T('s'):
			{
				switch( l_strAddress[1] )
				{
					case _T('T'):
					case _T('t'):
					{	// String
						l_eType = SV_PLC_STRING;
						l_lSize = 81;
						break;
					}
					case _T(':'):
					{	// System file
						l_eType = SV_PLC_INT16;
						l_lSize = 2;
						break;
					}
					default:
					{	// default to Word
						l_eType = SV_PLC_INT16;
						l_lSize = 2;
						break;
					}
				}
				break;
			}
			case _T('N'):
			case _T('n'):
			{
				l_eType = SV_PLC_INT16;
				l_lSize = 2;
				break;
			}
			case _T('L'):
			case _T('l'):
			{
				l_eType = SV_PLC_INT32;
				l_lSize = 4;
				break;
			}
			case _T('B'):
			case _T('b'):
			{
				l_eType = SV_PLC_BIT;
				l_lSize = 2;
				break;
			}
			case _T('F'):
			case _T('f'):
			{
				
				l_eType = SV_PLC_SPFLOAT;
				l_lSize = 4;
				break;
			}
			default :
			{
				l_eType = SV_PLC_INT16;
				l_lSize = 2;
				break;
			}
		}
	}
	*p_plType = l_eType;
	*p_plSize = l_lSize;
	return l_hr;
}


// This is an AllenBradley Address Specific function
// This function modifies the address by adding the offset to the string address.
HRESULT SVPLCInterfaceClass::AddOffsetToAddress( VARIANT* p_pvAddress, long p_lOffset, long p_lBit)
{
	HRESULT l_hr = -4122;
	if( p_pvAddress->vt == VT_BSTR )
	{
		CString lNewAddress;
		CString lstrTmp;
		CString l_strAddress;
		_bstr_t l_bstValue;
		l_bstValue.Attach( p_pvAddress->bstrVal );
		p_pvAddress->bstrVal = NULL;
		l_strAddress = static_cast<TCHAR*>(l_bstValue);
		int iPos = l_strAddress.Find(_T(':'));
		if( iPos > -1 )
		{
			long lNum = atol( l_strAddress.Mid( iPos + 1));
			lstrTmp = l_strAddress.Left( iPos + 1 );
			lNewAddress.Format( _T("%d"), lNum+p_lOffset);
			lstrTmp += lNewAddress;

			// Bit Stuff.
			if( p_lBit > -1)
			{
				iPos =  l_strAddress.Find('/');
				if( iPos > -1 )
				{
					long lBit = atol( l_strAddress.Mid( iPos + 1 ));
					lBit += p_lBit;
					lNewAddress.Format( "/%d", lBit );
					lstrTmp += lNewAddress;
				}
			}
			l_strAddress = lstrTmp;
			_bstr_t l_bstTmp(l_strAddress);
			p_pvAddress->bstrVal = l_bstTmp.Detach();
			l_hr = S_OK;
		}
	}
	return l_hr;
}



// Expects "/" to denote a bit follows
HRESULT SVPLCInterfaceClass::GetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, VARIANT p_vAddress )
{
	HRESULT l_hr = S_OK;
 
	SVPLCElementStruct l_Element;
	SVPLCElementStructArray l_ElementArray;

	_bstr_t l_bstAddress;
	l_bstAddress.Attach( p_vAddress.bstrVal );

	_tcscpy_s( l_Element.svmPLCAddress, sizeof( l_Element.svmPLCAddress), l_bstAddress );
	l_ElementArray.Add( &l_Element );

	// Make sure that a plc is defined
	if( m_PLCmap.size() == 0 )
	{
//		ASSERT(FALSE);
		return S_FALSE;
	}
	SVPLCBaseClass* l_pPLC = m_PLCmap.begin()->second.GetPLC();

	l_hr = l_pPLC->ParseElements( & l_ElementArray );

	*p_plFileNum = l_Element.svmPLCFileNbr;
	*p_plElement = l_Element.svmElement;
	*p_plBitNum = l_Element.svmlBitNum;

	return l_hr;
}

HRESULT SVPLCInterfaceClass::SetHeartBeatCallback( SVGetStateCallback& p_callback )
{
	m_pfnGetState = p_callback;
	return S_OK;
}

long SVPLCInterfaceClass::GetMaxDTSize( void )
{
	long l_lSize = 512;	// Default
	// Make sure that a plc is defined
	if( m_PLCmap.size() == 0 )
	{
//		ASSERT(FALSE);
	}
	else
	{
		SVPLCBaseClass* l_pPLC = m_PLCmap.begin()->second.GetPLC();
		l_lSize = l_pPLC->GetMaxDTSize();
	}

	
	return l_lSize;
}

HRESULT SVPLCInterfaceClass::GetDefaultPLCAddress( VARIANT* p_pvAddress )
{
	p_pvAddress->vt = VT_BSTR;
	p_pvAddress->bstrVal = ::SysAllocString(L"N7:0");
	return S_OK;
}

HRESULT SVPLCInterfaceClass::TestCommunications(LPCTSTR p_szPLCID)
{
	HRESULT l_hr = S_OK;
	if( m_PLCmap.find( p_szPLCID) != m_PLCmap.end() )
	{
		if( m_hResetEvent )
		{
			::SetEvent( m_hResetEvent );
		}
		l_hr = m_PLCmap[p_szPLCID].TestCommunications();

	}
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVP_SRC\SVPLCInterfaceClass.cpp_v  $
 * 
 *    Rev 1.1   01 Dec 2014 13:35:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attributes and lables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 16:03:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/