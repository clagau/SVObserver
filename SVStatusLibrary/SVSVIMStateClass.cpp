//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSVIMStateClass
//* .File Name       : $Workfile:   SVSVIMStateClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   09 Dec 2014 10:12:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <intrin.h>
#include "SVSVIMStateClass.h"
#include "Definitions/StringTypeDef.h"
#include "MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

std::atomic_long SVSVIMStateClass::m_SVIMState {SV_STATE_AVAILABLE};

bool SVSVIMStateClass::m_AutoSaveRequired {false};
std::atomic<__time64_t> SVSVIMStateClass::m_lastModifiedTime {0LL};
std::atomic<__time64_t> SVSVIMStateClass::m_loadedSinceTime {0LL};
std::mutex SVSVIMStateClass::ms_protectHash;
std::string SVSVIMStateClass::ms_hash;

std::atomic<SvPb::DeviceModeType> SVSVIMStateClass::m_CurrentMode {SvPb::DeviceModeType::unknownMode};
NotifyFunctor SVSVIMStateClass::m_pNotify {nullptr};

std::atomic<int>  SVSVIMStateClass::ms_LockCountSvrc {0};
std::atomic<bool> SVSVIMStateClass::m_isReloadedAfterCopyToolsToClipboard {false};



bool SVSVIMStateClass::isSvrcBlocked()
{
	return ms_LockCountSvrc > 0;
}

void SVSVIMStateClass::AddState(DWORD dwState)
{
	m_SVIMState |= dwState;

	if (dwState & SV_STATE_MODIFIED)
	{
		setLastModifiedTime();
		SetAutoSaveRequired(true);
	}
	if (dwState & SV_STATE_LOADING)
	{
		m_lastModifiedTime = 0;
	}
	CheckModeNotify();
}

void SVSVIMStateClass::RemoveState(DWORD dwState)
{
	m_SVIMState &= ~dwState;
	CheckModeNotify();
}

void SVSVIMStateClass::changeState(DWORD addStates, DWORD removeStates)
{
	m_SVIMState |= addStates;
	m_SVIMState &= ~removeStates;

	if (addStates & SV_STATE_MODIFIED)
	{
		setLastModifiedTime();
		SetAutoSaveRequired(true);
	}
	if (addStates & SV_STATE_LOADING)
	{
		m_lastModifiedTime = 0;
	}

	CheckModeNotify();
}

bool SVSVIMStateClass::CheckState(DWORD dwState)
{
	bool l_Status = (dwState & m_SVIMState) != 0;

	return l_Status;
}

SvPb::DeviceModeType SVSVIMStateClass::GetMode()
{
	SvPb::DeviceModeType result {SvPb::DeviceModeType::unknownMode};

	// Pending conditions...
	if (SVSVIMStateClass::CheckState(SV_STATE_START_PENDING |
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
	else if (SVSVIMStateClass::CheckState(SV_STATE_EDIT))
	{
		result = SvPb::DeviceModeType::editMode;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		result = SvPb::DeviceModeType::runMode;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION))
	{
		result = SvPb::DeviceModeType::regressionMode;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_TEST))
	{
		result = SvPb::DeviceModeType::testMode;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_STOP))
	{
		result = SvPb::DeviceModeType::stopMode;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_AVAILABLE))
	{
		result = SvPb::DeviceModeType::available;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		result = SvPb::DeviceModeType::modeChanging;
	}
	else
	{
		result = SvPb::DeviceModeType::unknownMode;
	}

	return result;
}

void SVSVIMStateClass::setNotificationFunction(const NotifyFunctor& Notify)
{
	m_pNotify = Notify;
}


void SVSVIMStateClass::CheckModeNotify()
{
	SvPb::DeviceModeType NewMode = GetMode();

	if (NewMode != m_CurrentMode)
	{
		m_CurrentMode = NewMode;

		if (nullptr != m_pNotify)
		{
			(m_pNotify)(SvPb::NotifyType::currentMode, _variant_t(static_cast<long> (m_CurrentMode)));
		}
	}
	//OutputDebugState();
}

void SVSVIMStateClass::setLastModifiedTime()
{
	m_lastModifiedTime = ::_time64(nullptr);

	if (nullptr != m_pNotify)
	{
		(m_pNotify)(SvPb::NotifyType::lastModified, _variant_t(SVSVIMStateClass::getLastModifiedTime()));
	}
}

void SVSVIMStateClass::ConfigWasLoaded(LPCSTR hash)
{
	m_loadedSinceTime = ::_time64(nullptr);
	SetHash(hash);
}
void SVSVIMStateClass::ConfigWasUnloaded()
{
	m_loadedSinceTime = 0L;
	SetHash(nullptr);
}


void SVSVIMStateClass::SetHash(LPCSTR hash)
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

std::string  SVSVIMStateClass::GetHash()
{
	std::lock_guard<std::mutex> lockGuard(ms_protectHash);
	std::string result(ms_hash);
	return result;
}

HRESULT SVSVIMStateClass::CheckNotAllowedState(DWORD States /*=SV_DefaultNotAllowedStates*/)
{

	bool accessDenied = SVSVIMStateClass::CheckState(States);
	if (accessDenied)
	{
		SvDef::StringVector msgList;
		msgList.emplace_back(std::move(std::format(_T("{:#X}"), SVSVIMStateClass::GetState())));
		msgList.emplace_back(std::move(std::format(_T("{:#X}"), States)));
		SvStl::MessageManager message(SvStl::MsgType::Log);
		message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVRC_AccessDenied, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return  accessDenied ? SVMSG_SVO_ACCESS_DENIED : S_OK;
}

void SVSVIMStateClass::OutputDebugState()
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
		if (entry.first & m_SVIMState)
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


