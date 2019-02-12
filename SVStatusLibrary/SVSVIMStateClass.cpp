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
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

std::atomic_long SVSVIMStateClass::m_SVIMState{SV_STATE_AVAILABLE};

bool SVSVIMStateClass::m_AutoSaveRequired{false};
std::atomic<__time32_t> SVSVIMStateClass::m_CurrentModifiedTime{0L};
std::atomic<SvPb::DeviceModeType> SVSVIMStateClass::m_CurrentMode{SvPb::DeviceModeType::unknownMode};
NotifyFunctor SVSVIMStateClass::m_pNotify{nullptr};

SVSVIMStateClass::SVSVIMStateClass()
{
}

SVSVIMStateClass::~SVSVIMStateClass()
{
}

bool SVSVIMStateClass::AddState( DWORD dwState )
{
	m_SVIMState |= dwState;

	if( dwState & SV_STATE_MODIFIED )
	{
		SetLastModifiedTime();
		SetAutoSaveRequired(true);
	}
	CheckModeNotify();
	return true;
}

bool SVSVIMStateClass::RemoveState( DWORD dwState )
{
	m_SVIMState &= ~dwState;
	CheckModeNotify();

	return true;
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

void SVSVIMStateClass::setNotificationFunction(NotifyFunctor Notify)
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

void SVSVIMStateClass::SetLastModifiedTime()
{
	m_CurrentModifiedTime = ::_time32(nullptr);

	if (nullptr != m_pNotify)
	{
		(m_pNotify)(static_cast<long> (SvStl::NotificationType::lastModified), static_cast<long> (SVSVIMStateClass::getCurrentTime()), 0L, nullptr);
	}
}

