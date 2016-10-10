//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteCommand
//* .File Name       : $Workfile:   RemoteCommand.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   29 Aug 2014 15:42:32  $
//******************************************************************************

#pragma region includes
#include "stdafx.h"
#include "RemoteCommand.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVObserver.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVSVIMStateClass.h"
#include "SVGlobal.h"
#include "ObjectInterfaces/SVIMCommand.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
#pragma endregion includes

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
		*pdwSVIMState |= SvOi::SVIM_CONFIG_LOADED;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_SAVING ) )
	{
		*pdwSVIMState |= SvOi::SVIM_SAVING_CONFIG;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
	{
		*pdwSVIMState |= SvOi::SVIM_CONFIG_LOADING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		*pdwSVIMState |= SvOi::SVIM_ONLINE;

		// testing (but not regression testing) sets the running flag
		if (! SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			*pdwSVIMState |= SvOi::SVIM_RUNNING;
		}
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		*pdwSVIMState |= SvOi::SVIM_REGRESSION_TEST;
	}
	// can be testing without regression testing, but can't be regression testing without testing
	else if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
	{
		*pdwSVIMState |= SvOi::SVIM_RUNNING_TEST;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		*pdwSVIMState |= SvOi::SVIM_SETUPMODE;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
	{
		*pdwSVIMState |= SvOi::SVIM_STOPPING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		*pdwSVIMState |= SvOi::SVIM_ONLINE_PENDING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
	{
		*pdwSVIMState |= SvOi::SVIM_RAID_FAILURE;
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
	*p_plMode = SVSVIMStateClass::GetMode();

	return S_OK;
}

// Global functions for SVFocusNT Remote Commands
BOOL GlobalRCGetConfigurationName( char* pszConfigName )
{
	BOOL bOk = FALSE;

	SVFileNameClass svFileName;

	svFileName.SetFullFileName( ( ( SVObserverApp* )AfxGetApp() )->getConfigFullFileName() );

	if ( !CString( svFileName.GetFileNameOnly() ).IsEmpty() )
	{
		svFileName.SetPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());

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

