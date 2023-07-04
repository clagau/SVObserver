//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ConfigurationOuttakes.h"
#include "RemoteCommand.h"
#include "SVVisionProcessorHelper.h"
#include "FilesystemUtilities/FileHelper.h"
#include "Definitions/SVIMCommand.h"
#include "Definitions/SVUserMessage.h"
#include "Definitions/SVIMCommand.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SvimState.h"
#pragma endregion includes

#pragma region Global functions
void GlobalRCGoOnline()
{
	::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), WM_COMMAND, MAKEWPARAM (ID_RC_GO_ONLINE, 0), 0);
}

void GlobalRCGoOffline()
{
	::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), WM_COMMAND, MAKEWPARAM (ID_RC_GO_OFFLINE, 0), 0);
}

bool GlobalRCGetState( DWORD* pdwSVIMState )
{
	*pdwSVIMState = 0;

	bool bOk = SvimState::CheckState( SV_STATE_AVAILABLE );

	if ( SvimState::CheckState( SV_STATE_READY ) )
	{
		*pdwSVIMState |= SvDef::SVIM_CONFIG_LOADED;
	}

	if ( SvimState::CheckState( SV_STATE_SAVING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_SAVING_CONFIG;
	}

	if ( SvimState::CheckState( SV_STATE_LOADING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_CONFIG_LOADING;
	}

	if ( SvimState::CheckState( SV_STATE_RUNNING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_ONLINE;

		// testing (but not regression testing) sets the running flag
		if (! SvimState::CheckState( SV_STATE_TEST ) )
		{
			*pdwSVIMState |= SvDef::SVIM_RUNNING;
		}
	}

	if ( SvimState::CheckState( SV_STATE_REGRESSION ) )
	{
		*pdwSVIMState |= SvDef::SVIM_REGRESSION_TEST;
	}
	// can be testing without regression testing, but can't be regression testing without testing
	else if ( SvimState::CheckState( SV_STATE_TEST ) )
	{
		*pdwSVIMState |= SvDef::SVIM_RUNNING_TEST;
	}

	if ( SvimState::CheckState( SV_STATE_EDIT ) )
	{
		*pdwSVIMState |= SvDef::SVIM_SETUPMODE;
	}

	if ( SvimState::CheckState( SV_STATE_CLOSING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_STOPPING;
	}

	if ( SvimState::CheckState( SV_STATE_START_PENDING ) )
	{
		*pdwSVIMState |= SvDef::SVIM_ONLINE_PENDING;
	}

	bOk = *pdwSVIMState != 0 || bOk;

	return bOk;
}

HRESULT GlobalRCSetMode( unsigned long newMode )
{
	return static_cast<HRESULT> (::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), SV_SET_MODE, 0, static_cast<LPARAM> (newMode)));
}

HRESULT GlobalRCGetMode( unsigned long* pMode )
{
	if(nullptr != pMode)
	{
		*pMode = SvimState::getCurrentMode();
	}
	else
	{
		return E_POINTER;
	}

	return S_OK;
}

// Global functions for SVFocusNT Remote Commands
std::string GlobalRCGetConfigurationName(bool runPath)
{
	std::string Result = getConfigFullFileName();

	if(runPath)
	{
		SvFs::FileHelper svFileName;
		svFileName.SetFullFileName(Result.c_str());

		if(!svFileName.GetFileNameOnly().empty())
		{
			svFileName.SetPathName(SvStl::GlobalPath::Inst().GetRunPath().c_str());
			Result = svFileName.GetFullFileName();
		}
		else
		{
			Result.clear();
		}
	}

	return Result;
}

HRESULT GlobalRCSaveConfiguration(LPCTSTR pFileName)
{
	LPARAM lParam = reinterpret_cast<LPARAM> (pFileName);
	return static_cast<HRESULT> (::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), SV_SAVE_CONFIG, 0, lParam));
}

bool GlobalRCOpenConfiguration( LPCTSTR ConfigName )
{
	SVRCSetSVCPathName( ConfigName );

	::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), WM_COMMAND, MAKEWPARAM (ID_RC_OPEN_CURRENT_SVX, 0), 0);

	return true;
}

void GlobalRCCloseConfiguration()
{
	::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), WM_COMMAND, MAKEWPARAM(ID_RC_CLOSE, 0), 0);
}

HRESULT GlobalRCLoadPackedConfiguration(LPCTSTR pFileName, ConfigFileType fileType)
{
	WPARAM wParam = static_cast<WPARAM> (fileType);
	LPARAM lParam = reinterpret_cast<LPARAM> (pFileName);
	return static_cast<HRESULT> (::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), SV_LOAD_PACKED_CONFIGURATION, wParam, lParam));
}

HRESULT GlobalRCAddFileToConfig(LPCTSTR pFileName)
{
	LPARAM lParam = reinterpret_cast<LPARAM> (pFileName);
	return static_cast<HRESULT>(::SendMessage(SVVisionProcessorHelper::Instance().getMainhWnd(), SV_ADD_FILE_TO_CONFIG, 0, lParam));
}
#pragma endregion Global functions
