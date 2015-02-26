//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductId
//* .File Name       : $Workfile:   SVProductId.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:58:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVProductId.h"


SVProductId::SVProductId()
: m_ResealWindows( false ), m_PreviousProductId( _T( "\"     -     -     -     -     \"" ) )
{
	TCHAR l_szOEMInfoFileName[ MAX_PATH ];

	::GetSystemDirectory( l_szOEMInfoFileName, sizeof( l_szOEMInfoFileName ) );

	::strcat( l_szOEMInfoFileName, _T( "\\oeminfo.ini" ) );

	TCHAR* l_pszProductId = m_PreviousProductId.GetBuffer( 64 );
		
	GetPrivateProfileString( _T( "OEMSpecific" ), _T("ProductID"), m_PreviousProductId, l_pszProductId, 64, l_szOEMInfoFileName );

	m_PreviousProductId.ReleaseBuffer();

	::strcpy_s( m_szProductId[ 0 ], 6, m_PreviousProductId.Mid( 0, 5 ) );
	::strcpy_s( m_szProductId[ 1 ], 6, m_PreviousProductId.Mid( 6, 5 ) );
	::strcpy_s( m_szProductId[ 2 ], 6, m_PreviousProductId.Mid( 12, 5 ) );
	::strcpy_s( m_szProductId[ 3 ], 6, m_PreviousProductId.Mid( 18, 5 ) );
	::strcpy_s( m_szProductId[ 4 ], 6, m_PreviousProductId.Mid( 24, 5 ) );
}

bool SVProductId::UpdateProductId() const
{
	bool l_Status = true;

	if( m_ResealWindows && PrepareForReseal() )
	{
		CString l_ProductId;
		
		l_ProductId.Format( _T( "\"%5.5s-%5.5s-%5.5s-%5.5s-%5.5s\"" ),  
			m_szProductId[ 0 ], m_szProductId[ 1 ], m_szProductId[ 2 ], m_szProductId[ 3 ], m_szProductId[ 4 ] );

		l_Status = ( l_ProductId.GetLength() == 31 );

		if( l_Status )
		{
			for( int i = 1; l_Status && i < 30; ++i )
			{
				l_Status = ( ::isalnum( l_ProductId.GetAt( i ) ) != 0 || l_ProductId.GetAt( i ) == '-' );
			}
		}

		if( l_Status )
		{
			TCHAR l_szOEMInfoFileName[ MAX_PATH ];

			::GetSystemDirectory( l_szOEMInfoFileName, sizeof( l_szOEMInfoFileName ) );

			::strcat( l_szOEMInfoFileName, _T( "\\oeminfo.ini" ) );

      WritePrivateProfileString( _T( "OEMSpecific" ), _T("ProductID"), l_ProductId, l_szOEMInfoFileName );

      //add entry into registry RunOnce to run the SVFinishSetup.bat
      HKEY l_hKey = NULL;
      unsigned long l_ulValue = 0;

      if ( ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T( "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce" ),
                          NULL, _T(""),REG_OPTION_NON_VOLATILE,
                          KEY_ALL_ACCESS,
                          (SECURITY_ATTRIBUTES *) NULL,
                          &l_hKey, & l_ulValue ) )
      {
          RegSetValueEx( l_hKey, _T("SVObserver"),0L, REG_SZ,
              (const unsigned char *) _T("C:\\SVObserver\\bin\\SVFinishSetup1.cmd"),
              38 * sizeof(TCHAR) );
           
          RegFlushKey( l_hKey );
          RegCloseKey( l_hKey );
      }

      //  RENAME THE FILE
      //	1.cab = sysprep.bat
      //	
      CString szSource, szDestination;
      
      szSource = _T("c:\\SVObserver\\sysprep\\" ) + CString("1.cab");
      szDestination = _T("c:\\SVObserver\\sysprep\\" ) + CString("init.cmd");
      CopyFile(szSource,szDestination,FALSE);
      DeleteFile(szSource);
      
      PROCESS_INFORMATION pi;
      STARTUPINFO         si;
      
      // 
      // initialize STARTUPINFO structure
      // 
      ZeroMemory(&si, sizeof(STARTUPINFO));
      si.cb        = sizeof(STARTUPINFO);
      si.lpDesktop = _T("winsta0\\default");
      
      // 
      // launch the process
      // 
      CString szApp,szCmdLine;
      szApp = "c:\\svobserver\\sysprep\\init.cmd";
      szCmdLine = "init";
      
      if(!CreateProcess(
          szApp,NULL,//"-quiet -reboot",
          NULL, NULL,  // security attributes
          FALSE, // handle inheritance
          CREATE_DEFAULT_ERROR_MODE,     // creation flags
          NULL,     // environment block
          "c:\\svobserver\\sysprep",//NULL,     // current directory
          &si, &pi ))
      {
					l_Status = false;
      }
		}
	}

	return l_Status;
}

LPCTSTR SVProductId::GetSubElement( size_t p_Index ) const
{
	LPCTSTR l_pszResult = NULL;

	if( 0 <= p_Index && p_Index < 5 )
	{ 
		l_pszResult = m_szProductId[ p_Index ];
	}

	return l_pszResult;
}

HRESULT SVProductId::SetSubElement( size_t p_Index, LPCTSTR p_szData )
{
	HRESULT l_Status = S_OK;

	if( 0 <= p_Index && p_Index < 5 )
	{ 
		m_ResealWindows = true;
		::strcpy_s( m_szProductId[ p_Index ], 6, p_szData );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVProductId::PrepareForReseal() const
{
	bool l_Status = true;

	TCHAR l_FBResealFileName[ MAX_PATH ];

	::GetSystemDirectory( l_FBResealFileName, sizeof( l_FBResealFileName ) );

	::strcat( l_FBResealFileName, _T( "\\FBReseal.exe" ) );

	l_Status = ( ::_access( l_FBResealFileName, 0 ) == 0 );

	if( ! l_Status )
	{
		l_Status = ( CopyFile( _T( "C:\\SVObserver\\SysPrep\\579.CAB" ), l_FBResealFileName, false ) != FALSE );
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVDriveInit\SVProductId.cpp_v  $
 * 
 *    Rev 1.1   30 Sep 2013 14:58:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 08:33:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2009 09:41:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated software to fix issues with FBReseal.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Nov 2009 07:51:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated product id from oeminfo.ini because the loader was stripping of the " from the string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Nov 2009 18:46:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated drive init program to fix issues associated with new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
