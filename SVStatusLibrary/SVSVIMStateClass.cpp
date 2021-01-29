//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
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
#include "NotificationTypeEnum.h"
#include <mutex>
#pragma endregion Includes

std::atomic_long SVSVIMStateClass::m_SVIMState{SV_STATE_AVAILABLE};

bool SVSVIMStateClass::m_AutoSaveRequired{false};
std::atomic<__time64_t> SVSVIMStateClass::m_lastModifiedTime{0LL};
std::atomic<__time64_t> SVSVIMStateClass::m_loadedSinceTime{ 0LL };
std::mutex SVSVIMStateClass::m_protectHash;
std::string SVSVIMStateClass::m_hash;

std::atomic<SvPb::DeviceModeType> SVSVIMStateClass::m_CurrentMode{SvPb::DeviceModeType::unknownMode};
NotifyFunctor SVSVIMStateClass::m_pNotify{nullptr};

std::atomic<int>  SVSVIMStateClass::m_LockCountSvrc {0};



SVSVIMStateClass::SVSVIMStateClass()
{
}

SVSVIMStateClass::~SVSVIMStateClass()
{
}

bool SVSVIMStateClass::isSvrcBlocked() 
{ 
	return m_LockCountSvrc > 0; 
}

void SVSVIMStateClass::AddState( DWORD dwState )
{
	m_SVIMState |= dwState;

	if( dwState & SV_STATE_MODIFIED )
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

void SVSVIMStateClass::RemoveState( DWORD dwState )
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

bool SVSVIMStateClass::CheckState( DWORD dwState )
{
	bool l_Status = ( dwState & m_SVIMState ) != 0;

	return l_Status;
}

SvPb::DeviceModeType SVSVIMStateClass::GetMode()
{
	SvPb::DeviceModeType result{SvPb::DeviceModeType::unknownMode};

	// Pending conditions...
	if( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING |
		SV_STATE_STARTING |
		SV_STATE_STOP_PENDING |
		SV_STATE_STOPING |
		SV_STATE_CREATING |
		SV_STATE_LOADING |
		SV_STATE_SAVING |
		SV_STATE_CLOSING |
	    SV_STATE_UNAVAILABLE |
		SV_STATE_EDITING |
		SV_STATE_CANCELING ) )
	{
		result = SvPb::DeviceModeType::modeChanging;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		result = SvPb::DeviceModeType::editMode;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		result = SvPb::DeviceModeType::runMode;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		result = SvPb::DeviceModeType::regressionMode;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
	{
		result = SvPb::DeviceModeType::testMode;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_STOP ) )
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
			(m_pNotify)(static_cast<long> (SvStl::NotificationType::mode), static_cast<long> (m_CurrentMode), 0L, nullptr);
		}
	}
}

void SVSVIMStateClass::setLastModifiedTime()
{
	m_lastModifiedTime = ::_time64(nullptr);

	if (nullptr != m_pNotify)
	{
		(m_pNotify)(static_cast<long> (SvStl::NotificationType::lastModified), static_cast<long> (SVSVIMStateClass::getLastModifiedTime()), 0L, nullptr);
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
	std::lock_guard<std::mutex> lockGuard(m_protectHash);
	if (nullptr != hash)
	{
		m_hash = hash;
	}
	else
	{
		m_hash.clear();
	}
}

std::string  SVSVIMStateClass::GetHash()
{
	std::lock_guard<std::mutex> lockGuard(m_protectHash);
	std::string result(m_hash);
	return result;
}