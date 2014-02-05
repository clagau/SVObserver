//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteCommand
//* .File Name       : $Workfile:   RemoteCommand.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   04 Feb 2014 13:16:36  $
//******************************************************************************
#pragma region includes
#include "stdafx.h"
#include "RemoteCommand.h"
#include "SVFileNameClass.h"
#include "SVObserver.h"
#include "SVUserMessage.h"
#include "SVSVIMStateClass.h"
#include "SVGlobal.h"
#include "SVIMCommand\SVIMCommand.h"
#pragma endregion includes

namespace Seidenader
{
	namespace SVObserver
	{
#pragma region Global functions
		BOOL GlobalRCGoOnline()
		{
			SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_GO_ONLINE, 0), 0);
			return TRUE;
		}

		BOOL GlobalRCGoOffline()
		{
			SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_GO_OFFLINE, 0), 0);
			return TRUE;
		}

		BOOL GlobalRCGetState( DWORD* pdwSVIMState )
		{
			*pdwSVIMState = 0;

			BOOL bOk = SVSVIMStateClass::CheckState( SV_STATE_AVAILABLE );

			if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
			{
				*pdwSVIMState |= SVIM_CONFIG_LOADED;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_SAVING ) )
			{
				*pdwSVIMState |= SVIM_SAVING_CONFIG;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
			{
				*pdwSVIMState |= SVIM_CONFIG_LOADING;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
			{
				*pdwSVIMState |= SVIM_ONLINE;

				// testing (but not regression testing) sets the running flag
				if (! SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
				{
					*pdwSVIMState |= SVIM_RUNNING;
				}
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
			{
				*pdwSVIMState |= SVIM_REGRESSION_TEST;
			}
			// can be testing without regression testing, but can't be regression testing without testing
			else if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
			{
				*pdwSVIMState |= SVIM_RUNNING_TEST;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
			{
				*pdwSVIMState |= SVIM_SETUPMODE;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
			{
				*pdwSVIMState |= SVIM_STOPPING;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
			{
				*pdwSVIMState |= SVIM_ONLINE_PENDING;
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
			{
				*pdwSVIMState |= SVIM_RAID_FAILURE;
			}

			bOk = *pdwSVIMState != 0 || bOk;

			return bOk;
		}

		HRESULT GlobalRCSetMode( unsigned long p_lNewMode )
		{
			return static_cast<HRESULT>(SendMessage( AfxGetApp()->m_pMainWnd->m_hWnd, SV_SET_MODE, 0, ( LPARAM )p_lNewMode ));
		}

		HRESULT GlobalRCGetMode( unsigned long* p_plMode )
		{
			HRESULT hr = S_OK;

			if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
			{
				*p_plMode = SVIM_MODE_EDIT;
			}
			else if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
			{
				*p_plMode = SVIM_MODE_ONLINE;
			}
			else if( SVSVIMStateClass::CheckState( SV_STATE_EDIT_MOVE ) )
			{
				*p_plMode = SVIM_MODE_EDIT_MOVE;
			}
			else if( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
			{
				*p_plMode = SVIM_MODE_REGRESSION;
			}
			else if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
			{
				*p_plMode = SVIM_MODE_TEST;
			}
			// Pending conditions...
			else if( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) ||
				SVSVIMStateClass::CheckState( SV_STATE_STARTING ) ||
				SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) ||
				SVSVIMStateClass::CheckState( SV_STATE_STOPING ) )
			{
				*p_plMode = SVIM_MODE_CHANGING;
			}
			else if( SVSVIMStateClass::CheckState( SV_STATE_READY ) &&
				!SVSVIMStateClass::CheckState( SV_STATE_EDIT ) &&
				!SVSVIMStateClass::CheckState( SV_STATE_EDIT_MOVE ) )
			{
				*p_plMode = SVIM_MODE_OFFLINE;
			}
			else
			{
				*p_plMode = 0;
			}

			return hr;
		}

		// Global functions for SVFocusNT Remote Commands
		BOOL GlobalRCGetConfigurationName( char* pszConfigName )
		{
			BOOL bOk = FALSE;

			SVFileNameClass svFileName;

			svFileName.SetFullFileName( ( ( SVObserverApp* )AfxGetApp() )->getConfigFullFileName() );

			if ( !CString( svFileName.GetFileNameOnly() ).IsEmpty() )
			{
				svFileName.SetPathName( _T( "C:\\RUN" ) );

				strcpy( pszConfigName, svFileName.GetFullFileName() );

				bOk = TRUE;
			}

			return bOk;
		}

		BOOL GlobalRCSaveConfiguration()
		{
			SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_SAVE_ALL_AND_GET_CONFIG, 0), 0);
			return TRUE;
		}

		BOOL GlobalRCOpenConfiguration( char* pszConfigName )
		{
			CString szConfigName(pszConfigName);

			SVRCSetSVCPathName(szConfigName);

			TCHAR szDrive[_MAX_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFile[_MAX_FNAME];
			TCHAR szExt[_MAX_EXT];
			CString sComp;

			_tsplitpath( pszConfigName, szDrive, szDir, szFile, szExt );
			sComp = szExt;

			if (sComp.CompareNoCase(_T(".svx"))==0)
			{
				SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_OPEN_CURRENT_SVX, 0), 0);
			}
			else
			{
				return FALSE;
			}

			return TRUE;
		}

		BOOL GlobalRCCloseAndCleanConfiguration()
		{
			SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_CLOSE_AND_CLEAN_RUN_DIR, 0), 0);
			return TRUE;
		}

		BOOL GlobalRCCloseConfiguration()
		{
			SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_CLOSE, 0), 0);
			return TRUE;
		}
#pragma endregion Global functions

	} //namespace SVObserver
} //namespace Seidenader