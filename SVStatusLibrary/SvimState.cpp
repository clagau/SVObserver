//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SvimState
//* .File Name       : $Workfile:   SvimState.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   09 Dec 2014 10:12:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <intrin.h>
#include "SvimState.h"
#include "Definitions/StringTypeDef.h"
#include "MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

std::atomic_long SvimState::ms_fullState {SV_STATE_AVAILABLE};

bool SvimState::ms_AutoSaveRequired {false};
std::atomic<__time64_t> SvimState::ms_lastModifiedTime {0LL};
std::atomic<__time64_t> SvimState::ms_loadedSinceTime {0LL};
std::mutex SvimState::ms_protectHash;
std::string SvimState::ms_hash;

std::atomic<SvPb::DeviceModeType> SvimState::ms_CurrentMode {SvPb::DeviceModeType::unknownMode};
NotifyFunctor SvimState::ms_notify {nullptr};

std::atomic<int>  SvimState::ms_LockCountSvrc {0};

int64_t SvimState::ms_EditingStates = 0; 


bool SvimState::isSvrcBlocked()
{
	return ms_LockCountSvrc > 0;
}

void SvimState::AddState(DWORD dwState)
{
	ms_fullState |= dwState;

	if (dwState & SV_STATE_MODIFIED)
	{
		setLastModifiedTime();
		SetAutoSaveRequired(true);
	}
	if (dwState & SV_STATE_LOADING)
	{
		ms_lastModifiedTime = 0;
	}
	CheckModeNotify();
}

void SvimState::RemoveState(DWORD dwState)
{
	ms_fullState &= ~dwState;
	CheckModeNotify();
}

void SvimState::changeState(DWORD addStates, DWORD removeStates)
{
	ms_fullState |= addStates;
	ms_fullState &= ~removeStates;

	if (addStates & SV_STATE_MODIFIED)
	{
		setLastModifiedTime();
		SetAutoSaveRequired(true);
	}
	if (addStates & SV_STATE_LOADING)
	{
		ms_lastModifiedTime = 0;
	}

	CheckModeNotify();
}

bool SvimState::CheckState(DWORD dwState)
{
	bool l_Status = (dwState & ms_fullState) != 0;

	return l_Status;
}

SvPb::DeviceModeType SvimState::GetMode()
{
	SvPb::DeviceModeType result {SvPb::DeviceModeType::unknownMode};

	// Pending conditions...
	if (SvimState::CheckState(SV_STATE_START_PENDING |
		SV_STATE_STARTING |
		SV_STATE_STOP_PENDING |
		SV_STATE_STOPING |
		SV_STATE_CREATING |
		SV_STATE_LOADING |
		SV_STATE_SAVING |
		SV_STATE_CLOSING |
		SV_STATE_UNAVAILABLE |
		SV_STATE_EDITING |
		SV_STATE_CANCELING))
	{
		result = SvPb::DeviceModeType::modeChanging;
	}
	else if (SvimState::CheckState(SV_STATE_EDIT))
	{
		result = SvPb::DeviceModeType::editMode;
	}
	else if (SvimState::CheckState(SV_STATE_RUNNING))
	{
		result = SvPb::DeviceModeType::runMode;
	}
	else if (SvimState::CheckState(SV_STATE_REGRESSION))
	{
		result = SvPb::DeviceModeType::regressionMode;
	}
	else if (SvimState::CheckState(SV_STATE_TEST))
	{
		result = SvPb::DeviceModeType::testMode;
	}
	else if (SvimState::CheckState(SV_STATE_STOP))
	{
		result = SvPb::DeviceModeType::stopMode;
	}
	else if (SvimState::CheckState(SV_STATE_AVAILABLE))
	{
		result = SvPb::DeviceModeType::available;
	}
	else if (SvimState::CheckState(SV_STATE_READY))
	{
		result = SvPb::DeviceModeType::modeChanging;
	}
	else
	{
		result = SvPb::DeviceModeType::unknownMode;
	}

	return result;
}

void SvimState::setNotificationFunction(const NotifyFunctor& Notify)
{
	ms_notify = Notify;
}


void SvimState::CheckModeNotify()
{
	SvPb::DeviceModeType NewMode = GetMode();

	if (NewMode != ms_CurrentMode)
	{
		ms_CurrentMode = NewMode;

		if (nullptr != ms_notify)
		{
			(ms_notify)(SvPb::NotifyType::currentMode, _variant_t(static_cast<long> (ms_CurrentMode)));
		}
	}
	//OutputDebugState();
}

void SvimState::setLastModifiedTime()
{
	ms_lastModifiedTime = ::_time64(nullptr);

	if (nullptr != ms_notify)
	{
		(ms_notify)(SvPb::NotifyType::lastModified, _variant_t(SvimState::getLastModifiedTime()));
	}
}

void SvimState::ConfigWasLoaded(LPCSTR hash)
{
	ms_loadedSinceTime = ::_time64(nullptr);
	SetHash(hash);
}
void SvimState::ConfigWasUnloaded()
{
	ms_loadedSinceTime = 0L;
	SetHash(nullptr);
}


void SvimState::SetHash(LPCSTR hash)
{
	std::lock_guard<std::mutex> lockGuard(ms_protectHash);
	if (nullptr != hash)
	{
		ms_hash = hash;
	}
	else
	{
		ms_hash.clear();
	}
}

std::string  SvimState::GetHash()
{
	std::lock_guard<std::mutex> lockGuard(ms_protectHash);
	std::string result(ms_hash);
	return result;
}

HRESULT SvimState::CheckNotAllowedState(DWORD States /*=SV_DefaultNotAllowedStates*/)
{

	bool accessDenied = SvimState::CheckState(States);
	if (accessDenied)
	{
		SvDef::StringVector msgList;
		msgList.emplace_back(std::move(std::format(_T("{:#X}"), SvimState::GetState())));
		msgList.emplace_back(std::move(std::format(_T("{:#X}"), States)));
		SvStl::MessageManager message(SvStl::MsgType::Log);
		message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVRC_AccessDenied, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return  accessDenied ? SVMSG_SVO_ACCESS_DENIED : S_OK;
}

void SvimState::OutputDebugState()
{
	std::unordered_map<DWORD, std::string> StateNames =
	{
	{SV_STATE_AVAILABLE, "SV_STATE_AVAILABLE"},
	{SV_STATE_READY, "SV_STATE_READY"},
	{SV_STATE_RUNNING, "SV_STATE_RUNNING"},
	{SV_STATE_UNAVAILABLE, "SV_STATE_UNAVAILABLE"},
	{SV_STATE_CREATING, "SV_STATE_CREATING"},
	{SV_STATE_LOADING, "SV_STATE_LOADING"},

	{SV_STATE_SAVING, "SV_STATE_SAVING"},
	{SV_STATE_CLOSING, "SV_STATE_CLOSING"},
	{SV_STATE_EDITING, "SV_STATE_EDITING"},
	{SV_STATE_CANCELING, "SV_STATE_CANCELING"},
	{SV_STATE_INTERNAL_RUN, "SV_STATE_INTERNAL_RUN"},
	{SV_STATE_START_PENDING, "SV_STATE_START_PENDING"},
	{SV_STATE_STARTING, "SV_STATE_STARTING"},
	{SV_STATE_STOP_PENDING, "SV_STATE_STOP_PENDING"},
	{SV_STATE_STOPING, "SV_STATE_STOPING"},
	{SV_STATE_TEST, "SV_STATE_TEST"},
	{SV_STATE_REGRESSION, "SV_STATE_REGRESSION"},
	{SV_STATE_EDIT, "SV_STATE_EDIT"},
	{SV_STATE_STOP, "SV_STATE_STOP"},
	{SV_STATE_MODIFIED, "SV_STATE_MODIFIED"},
	{SV_STATE_REMOTE_CMD, "SV_STATE_REMOTE_CMD"}
	};

	
	std::string msg {"SV_STATE_UNKNOWN"};
	int count {0};
	for (auto& entry : StateNames)
	{
		if (entry.first & ms_fullState)
		{
			if (count++ == 0)
				msg = entry.second;
			else
			{
				msg += " | ";
				msg += entry.second;
			}
		}
	}
	msg += "\n";
	OutputDebugString(msg.c_str());
}


