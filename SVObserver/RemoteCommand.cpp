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
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVObserver.h"
#include "Definitions/SVUserMessage.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
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

HRESULT GlobalRCSetMode( unsigned long newMode )
{
	return static_cast<HRESULT>(SendMessage( AfxGetApp()->m_pMainWnd->m_hWnd, SV_SET_MODE, 0, static_cast<LPARAM> (newMode)));
}

HRESULT GlobalRCGetMode( unsigned long* pMode )
{
	if(nullptr != pMode)
	{
		*pMode = SVSVIMStateClass::getCurrentMode();
	}
	else
	{
		return E_POINTER;
	}

	return S_OK;
}

// Global functions for SVFocusNT Remote Commands
std::string GlobalRCGetConfigurationName()
{
	std::string Result;

	SVFileNameClass svFileName;

	svFileName.SetFullFileName( ( ( SVObserverApp* )AfxGetApp() )->getConfigFullFileName().c_str() );

	if ( !svFileName.GetFileNameOnly().empty() )
	{
		svFileName.SetPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());

		Result = svFileName.GetFullFileName();
	}

	return Result;
}

HRESULT GlobalRCSaveConfiguration(LPCTSTR pFileName)
{
	LPARAM lParam = reinterpret_cast<LPARAM> (pFileName);
	return static_cast<HRESULT> (::SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, SV_SAVE_CONFIG, 0, lParam));
}

bool GlobalRCOpenConfiguration( LPCTSTR ConfigName )
{
	SVRCSetSVCPathName( ConfigName );

	SendMessage (AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_OPEN_CURRENT_SVX, 0), 0);

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

HRESULT GlobalRCLoadPackedConfiguration(LPCTSTR pFileName, ConfigFileType fileType)
{
	WPARAM wParam = static_cast<WPARAM> (fileType);
	LPARAM lParam = reinterpret_cast<LPARAM> (pFileName);
	return static_cast<HRESULT> (::SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, SV_LOAD_PACKED_CONFIGURATION, wParam, lParam));
}

HRESULT GlobalRCAddFileToConfig(LPCTSTR pFileName)
{
	LPARAM lParam = reinterpret_cast<LPARAM> (pFileName);
	return static_cast<HRESULT>(::SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, SV_ADD_FILE_TO_CONFIG, 0, lParam));
}
#pragma endregion Global functions
