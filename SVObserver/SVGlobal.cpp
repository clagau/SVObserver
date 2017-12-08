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
#include "SVDataManagerLibrary/DataManager.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

TCHAR SVRCCurrentSVCPathName[ _MAX_PATH ];

SVDataManager TheSVDataManager;

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

// Calculate real pixel number of a line, which is built by Bresenham.
// Works also with negative values.
long SVCalcLinePixelNumber( long Width, long Height )
{
	long Length = std::max( labs( Width ), labs( Height ) );
	return( ( Width && Height ) ? ++Length : Length );
}

double SVGetDataTypeMin( DWORD DataType )
{
	switch( DataType )
	{
		case SV_1BIT:
			return 0.0;

		case SV_8BIT_INT:
			return 0.0;

		case SV_SIGNED_8BIT_INT:
			return -128.0;
			
		case SV_16BIT_INT:
			return 0.0;

		case SV_SIGNED_16BIT_INT:
			return -32768.0;

		case SV_32BIT_INT:
			return 0.0;

		case SV_SIGNED_32BIT_INT:
			// Be careful: lose precision!
			return -2147483648.0;

		case SV_SIGNED_64BIT_INT:
			// Be careful: lose precision!
			return -9223372036854775808.0;

		case SV_FLOAT:
			return 3.4E-38;
	}
	return 0.0;
}

double SVGetDataTypeMax( DWORD DataType )
{
	switch( DataType )
	{
		case SV_1BIT:
			return 1.0;

		case SV_8BIT_INT:
			return 255.0;

		case SV_SIGNED_8BIT_INT:
			return 127.0;
			
		case SV_16BIT_INT:
			return 65535.0;

		case SV_SIGNED_16BIT_INT:
			return 32767.0;

		case SV_32BIT_INT:
			// Be careful: lose precision!
			return 4294967295.0;

		case SV_SIGNED_32BIT_INT:
			// Be careful: lose precision!
			return 2147483647.0;

		case SV_SIGNED_64BIT_INT:
			// Be careful: lose precision!
			return 9223372036854775807.0;

		case SV_FLOAT:
			return 3.4E38;
	}
	return 0.0;
}

double SVGetDataTypeRange( DWORD DataType )
{
	switch( DataType )
	{
		case SV_1BIT:
			return 2.0;

		case SV_8BIT_INT:
		case SV_SIGNED_8BIT_INT:
			return 256.0;

		case SV_16BIT_INT:
		case SV_SIGNED_16BIT_INT:
			return 65536.0;

		case SV_32BIT_INT:
		case SV_SIGNED_32BIT_INT:
			// Be careful: lose precision!
			return 4294967296.0;

		case SV_SIGNED_64BIT_INT:
			// Be careful: lose precision!
			return 18446744073709551616.0;

		case SV_FLOAT:
			return 6.8E38;
	}
	return 0.0;
	// ( long ) pow( 2.0, ( BYTE ) ( DataType ) )
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

// Convert Hex Data to a Hex Dumop String
// Len of the data is first in the String
void SVConvertToHexString( DWORD len, LPBYTE buff, std::string& rHexString )
{
	// put len in string first
	rHexString = SvUl::Format( _T( "0x%08x" ), len );

	for( DWORD i = 0; i < len; i++ )
	{
		std::string Text = SvUl::Format( _T( ",0x%02x" ), buff[i] );
		rHexString += Text;
	}
}

// Convert Hex Dump String to hex binary data
// Sets len of the hex binary data
// Note: Allocates buffer so caller must delete it
BOOL SVConvertFromHexString( DWORD &len, LPBYTE *buff, const std::string& rHexString )
{
	*buff = nullptr;
	len = 0;

	// get length first
	size_t Index = rHexString.find( _T(",") );

	// index should be at position 10
	if( Index == 10 )
	{
		std::string Text = SvUl::Left( rHexString, Index );

		// Convert to hex
		_stscanf_s( Text.c_str(), _T("%x"), &len );

		// Allocate buffer
		*buff = new BYTE[ len ];
		memset( *buff, '\0', len );

		size_t startIndex = Index + 1;
		size_t HexSize = rHexString.size();

		for( DWORD i = 0;i < len;i++ )
		{
			Index = std::string::npos;
			
			if ( startIndex < HexSize )
			{
				Index = rHexString.find( _T(","), startIndex );
			}

			if( std::string::npos != Index )
			{
				// get Hex BYTE String Data
				Text = rHexString.substr( startIndex, ( Index - startIndex ) );

				// convert to hex BYTE
				BYTE hexByte;
				DWORD value;
				_stscanf_s( Text.c_str(), _T("%x"), &value );
				hexByte = static_cast<unsigned char>(value);

				(*buff)[i] = hexByte;

				// set next index
				startIndex = Index + 1;
			}
			else
				break;
		}
		return true;
	}
	return false;
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



#define INDEX_FOUND  -2

//************************************
// Method:    _FindMenuIndex
// Description:  Function is only used in EnableParentMenu
// Parameter: CMenu *  & pParent pointer to parent menu (output)
// Parameter: int &ParentMenuIndex index of the parenmenuentry
// Parameter: CMenu * pMenu 
// Parameter: UINT ID CommandId 
// Parameter: int  start index of submenu where search for the command entry is started.
// Returns:   int index of Entry, 
//************************************
int   _FindMenuIndex( CMenu*  &pParent,  int&  ParentMenuIndex, CMenu*  pMenu, UINT ID , int start =0)
{
	for(int i =start ; i < pMenu->GetMenuItemCount(); i++)
	{
		if( pMenu->GetMenuItemID(i) == ID)
		{
			return i;
		}
		else
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);

			if(pSubMenu)
			{
				int index = _FindMenuIndex(pParent,ParentMenuIndex, pSubMenu, ID);

				if(index == INDEX_FOUND)
				{
					return INDEX_FOUND;
				}

				if(index > -1)
				{
					ParentMenuIndex = i;
					pParent =  pMenu;
					return INDEX_FOUND;
				}
			}
		}

	}
	return -1;
}

bool EnableParentMenu(CMenu* pMenu, UINT ID, bool Enable, int start )
{
	UINT flag = Enable?  MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION |  MF_GRAYED;
	CMenu* pParentMenu =nullptr;
	int ParentMenuIndex =-1;
	if( _FindMenuIndex( pParentMenu, ParentMenuIndex, pMenu, ID , start ) == INDEX_FOUND)
	{
		if(pParentMenu )
		{
			return (-1 != pParentMenu->EnableMenuItem(ParentMenuIndex, flag ));
		}
	}
	return false;
}
