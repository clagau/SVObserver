//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobal Function Module
//* .File Name       : $Workfile:   SVGlobal.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Aug 2014 15:49:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <direct.h>
#include "SVGlobal.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

TCHAR SVRCCurrentSVCPathName[ _MAX_PATH ];

CMDIChildWnd* SVSearchForMDIChildWnd( CWnd* PStartWnd )
{
	// Only for MFC-Threads!

	while( nullptr != PStartWnd )
	{
		if( ! PStartWnd->IsKindOf( RUNTIME_CLASS( CWnd ) ) )
		{
			return nullptr;
		}

		if( PStartWnd->IsKindOf( RUNTIME_CLASS( CMDIChildWnd ) ) )
		{
			return( ( CMDIChildWnd* ) PStartWnd );
		}

		PStartWnd = PStartWnd->GetParent();
	}
	return nullptr;
}

BOOL SVGetPathInformation( std::string& rOutput, LPCTSTR TStrFileInputPath, DWORD DwMask )
{
	// Only for MFC-Threads!

	// TStrFileInputPath should include a file name!
	// e.g.: c:\dir1\dir2\dir3\filename.ext
	//		 ..\filename.ext
	//	     filename
	// strlen of TStrFileInputPath must not exceed _MAX_PATH length!

	// RStrOutput only receives requested information!

	// If successful the function returns TRUE.
	// In all other cases it returns FALSE.

	if( TStrFileInputPath && _tcslen( TStrFileInputPath ) < _MAX_PATH )
	{
		TCHAR  tStrPathOut[ _MAX_PATH ];
		TCHAR  drive[ _MAX_DRIVE ];
		TCHAR  dir[ _MAX_DIR ];
		TCHAR  fname[ _MAX_FNAME ];  
		TCHAR  ext[ _MAX_EXT ];
		_tsplitpath( TStrFileInputPath, drive, dir, fname, ext );
		tStrPathOut[ 0 ] = _TCHAR( '\0' );

		if( ( DwMask & ( ( DWORD ) SVDRIVE ) ) == ( ( DWORD ) SVDRIVE ) )
			_tcscat( tStrPathOut, drive );

		if( ( DwMask & ( ( DWORD ) SVDIR ) ) == ( ( DWORD ) SVDIR ) )
			_tcscat( tStrPathOut, dir );

		if( ( DwMask & ( ( DWORD ) SVFILE ) ) == ( ( DWORD ) SVFILE ) )
			_tcscat( tStrPathOut, fname );

		if( ( DwMask & ( ( DWORD ) SVEXT ) ) == ( ( DWORD ) SVEXT ) )
			_tcscat( tStrPathOut, ext );

		rOutput = tStrPathOut;
		return TRUE;
	}
	return FALSE;
}

BOOL SVGetVersionString( std::string& rCurrentVersion, DWORD dwVersion )
{
	int subVersion   = ( int ) LOBYTE( LOWORD( dwVersion ) );
	std::string Beta;

	//
	// 0x000300ff is v3.00 release build.
	//
	if( subVersion != 0  && subVersion != 0xff )
	{
		Beta = SvUl::Format( _T( " Beta %d" ), subVersion );
	}

	rCurrentVersion = SvUl::Format( _T( "Version %u.%2.2u%s" ), 
	                            LOBYTE( HIWORD( dwVersion ) ), 
	                            HIBYTE( LOWORD( dwVersion ) ), 
	                            Beta.c_str() );

#ifdef _DEBUG
	rCurrentVersion += _T("d");        // For debug builds.
#endif
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFindMenuByCommand
// -----------------------------------------------------------------------------
// .Description : Returns 'parent' menu and position of searched menu, if any. 
//				: Otherwise nullptr.
////////////////////////////////////////////////////////////////////////////////
CMenu* SVFindMenuByCommand( CMenu *pMenu, int nID, BOOL bIncludeSubMenues, int& rMenuPos )
{
	CMenu* pRetMenu = nullptr;
	if( pMenu )
	{
		int count = pMenu->GetMenuItemCount();
		for( int i = 0; i < count && ! pRetMenu; ++ i )
		{
			if( (unsigned int)(nID) != pMenu->GetMenuItemID( i ) )
			{
				if( bIncludeSubMenues )
				{
					// Check Sub Menu...
					CMenu* pSubMenu = pMenu->GetSubMenu( i );
					pRetMenu = ::SVFindMenuByCommand( pSubMenu, nID, bIncludeSubMenues, rMenuPos );
				}
			}
			else
			{
				// Bingo...
				pRetMenu = pMenu;
				rMenuPos = i;
			}
		}
	}
	return pRetMenu;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFindMenuByName
// -----------------------------------------------------------------------------
// .Description : Returns 'parent' menu and position of searched menu, if any.
//				: Otherwise nullptr.
////////////////////////////////////////////////////////////////////////////////
CMenu* SVFindMenuByName( CMenu *pMenu, LPCTSTR MenuString, BOOL bIncludeSubMenues, int& rMenuPos )
{
	CMenu* pRetMenu = nullptr;
	if( pMenu && MenuString )
	{
		int count = pMenu->GetMenuItemCount();
		for( int i = 0; i < count && ! pRetMenu; ++ i )
		{
			CString strMenu;
			pMenu->GetMenuString( i, strMenu, MF_BYPOSITION );
			if( strMenu.CompareNoCase( MenuString ) )
			{
				// Not found...

				if( bIncludeSubMenues )
				{
					// Check Sub Menu...
					CMenu* pSubMenu = pMenu->GetSubMenu( i );
					pRetMenu = ::SVFindMenuByName( pSubMenu, MenuString, bIncludeSubMenues, rMenuPos );
				}
			}
			else
			{
				// Bingo...
				pRetMenu = pMenu;
				rMenuPos = i;
			}
		}
	}
	return pRetMenu;
}
 
////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFindSubMenuByName
// -----------------------------------------------------------------------------
// .Description : Returns pointer to sub menu which is searched for, if any.
//				: Otherwise nullptr.
////////////////////////////////////////////////////////////////////////////////
CMenu* SVFindSubMenuByName( CMenu *pMenu, LPCTSTR MenuString, BOOL bIncludeSubMenues )
{
	CMenu* pRetMenu = nullptr;
	if( pMenu && MenuString )
	{
		int count = pMenu->GetMenuItemCount();
		for( int i = 0; i < count && ! pRetMenu; ++ i )
		{
			CString strMenu;
			pMenu->GetMenuString( i, strMenu, MF_BYPOSITION );
			if( strMenu.CompareNoCase( MenuString ) )
			{
				// Not found...

				if( bIncludeSubMenues )
				{
					// Check Sub Menu...
					CMenu* pSubMenu = pMenu->GetSubMenu( i );
					pRetMenu = ::SVFindSubMenuByName( pSubMenu, MenuString, bIncludeSubMenues );
				}
			}
			else
			{
				// Bingo...
				pRetMenu = pMenu->GetSubMenu( i );
			}
		}
	}
	return pRetMenu;
}

LPCTSTR SVRCGetSVCPathName()
{
	DWORD dwType, dwSize, dwResult;
	HKEY hKey;

	dwType = 0L;
	dwSize = sizeof (SVRCCurrentSVCPathName);
	memset (SVRCCurrentSVCPathName, 0, sizeof (SVRCCurrentSVCPathName));

	dwResult = RegOpenKeyEx (HKEY_CURRENT_USER, 
                            _T( "Software\\SVR Gesellschaft für Bildverarbeitung mbH\\SVObserver\\Settings" ),
                             0L, KEY_QUERY_VALUE, &hKey);
	if (dwResult == ERROR_SUCCESS)
	{
		RegQueryValueEx (hKey, _T("CurrentSVCPathName"), (DWORD *) nullptr, &dwType, (BYTE *) SVRCCurrentSVCPathName, &dwSize);
		RegCloseKey (hKey);
	}
	else
	{
#ifdef _DEBUG
		char *pszBuff;
		pszBuff = nullptr;
		FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					   nullptr, dwResult, 0, (char *) &pszBuff, 0, nullptr);
		ASSERT( false );// pszBuff
		LocalFree (pszBuff);
#endif
	}
	return SVRCCurrentSVCPathName;
}


void SVRCSetSVCPathName( LPCTSTR TStrPathName )
{
//	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if( TStrPathName )
	{
		if( _tcslen( TStrPathName ) < _MAX_PATH )
			_tcscpy( SVRCCurrentSVCPathName, TStrPathName );
		else
		{
			memcpy( SVRCCurrentSVCPathName, TStrPathName, _MAX_PATH );
			SVRCCurrentSVCPathName[ _MAX_PATH - 1 ] = '\0';
		}
	}
	AfxGetApp()->WriteProfileString( _T( "Settings" ), _T( "CurrentSVCPathName" ), SVRCCurrentSVCPathName );
}