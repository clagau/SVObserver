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
#include "SVLibrary/SVFileNameClass.h"
#include "SVObserver.h"
#include "Definitions/SVUserMessage.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVGlobal.h"
#include "Definitions/SVIMCommand.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion includes

#pragma region Global functions
void GlobalRCGoOnline()
{
	SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_GO_ONLINE, 0), 0);
}

void GlobalRCGoOffline()
{
	SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_GO_OFFLINE, 0), 0);
}

bool GlobalRCGetState( DWORD* pdwSVIMState )
{
	*pdwSVIMState = 0;

	bool bOk = SVSVIMStateClass::CheckState( SV_STATE_AVAILABLE );

	if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		*pdwSVIMState |= SvDef::SVIM_CONFIG_LOADED;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_SAVING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_SAVING_CONFIG;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_CONFIG_LOADING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_ONLINE;

		// testing (but not regression testing) sets the running flag
		if (! SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			*pdwSVIMState |= SvDef::SVIM_RUNNING;
		}
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		*pdwSVIMState |= SvDef::SVIM_REGRESSION_TEST;
	}
	// can be testing without regression testing, but can't be regression testing without testing
	else if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
	{
		*pdwSVIMState |= SvDef::SVIM_RUNNING_TEST;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		*pdwSVIMState |= SvDef::SVIM_SETUPMODE;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_STOPPING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_ONLINE_PENDING;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
	{
		*pdwSVIMState |= SvDef::SVIM_RAID_FAILURE;
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
SVString GlobalRCGetConfigurationName()
{
	SVString Result;

	SVFileNameClass svFileName;

	svFileName.SetFullFileName( ( ( SVObserverApp* )AfxGetApp() )->getConfigFullFileName().c_str() );

	if ( !svFileName.GetFileNameOnly().empty() )
	{
		svFileName.SetPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());

		Result = svFileName.GetFullFileName();
	}

	return Result;
}

void GlobalRCSaveConfiguration()
{
	SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_SAVE_ALL_AND_GET_CONFIG, 0), 0);
}

bool GlobalRCOpenConfiguration( LPCTSTR ConfigName )
{
	SVRCSetSVCPathName( ConfigName );

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	SVString Extension;

	_tsplitpath( ConfigName, szDrive, szDir, szFile, szExt );
	Extension = szExt;

	if( 0 == SvUl_SF::CompareNoCase( Extension, _T(".svx") ) )
	{
		SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_OPEN_CURRENT_SVX, 0), 0);
	}
	else
	{
		return false;
	}

	return true;
}

void GlobalRCCloseAndCleanConfiguration()
{
	SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_CLOSE_AND_CLEAN_RUN_DIR, 0), 0);
}

void GlobalRCCloseConfiguration()
{
	SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_CLOSE, 0), 0);
}
#pragma endregion Global functions

