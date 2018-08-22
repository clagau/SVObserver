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

#include "stdafx.h"
//Moved to precompiled header: #include <intrin.h>
#include "SVSVIMStateClass.h"
#include "Definitions/GlobalConst.h"

#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)

long SVSVIMStateClass::m_SVIMState = SV_STATE_AVAILABLE;

bool SVSVIMStateClass::m_AutoSaveRequired = false; ///< should an autosave be performed at the next appropriate time?
volatile __time32_t SVSVIMStateClass::m_PreviousModifiedTime{0L};
volatile __time32_t SVSVIMStateClass::m_CurrentModifiedTime{0L};
volatile svModeEnum SVSVIMStateClass::m_PreviousMode = SVIM_MODE_UNKNOWN;
volatile svModeEnum SVSVIMStateClass::m_CurrentMode = SVIM_MODE_UNKNOWN;
NotifyFunctor SVSVIMStateClass::m_Notify;

SVSVIMStateClass::SVSVIMStateClass()
{
}

SVSVIMStateClass::~SVSVIMStateClass()
{
}

bool SVSVIMStateClass::AddState( DWORD dwState )
{
	::_InterlockedOr( &m_SVIMState, dwState );

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
	::_InterlockedAnd( &m_SVIMState, ~dwState );
	CheckModeNotify();

	return true;
}

bool SVSVIMStateClass::CheckState( DWORD dwState )
{
	bool l_Status = ( dwState & m_SVIMState ) != 0;

	return l_Status;
}

svModeEnum SVSVIMStateClass::GetMode()
{
	svModeEnum retVal = SVIM_MODE_UNKNOWN;

	// Pending conditions...
	if( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING |
		SV_STATE_STARTING |
		SV_STATE_STOP_PENDING |
		SV_STATE_STOPING |
		SV_STATE_CREATING |
		SV_STATE_LOADING |
		SV_STATE_SAVING |
		SV_STATE_CLOSING |
		SV_STATE_EDITING ) )
	{
		retVal = SVIM_MODE_CHANGING;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		retVal = SVIM_MODE_EDIT;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		retVal = SVIM_MODE_ONLINE;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		retVal = SVIM_MODE_REGRESSION;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
	{
		retVal = SVIM_MODE_TEST;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_STOP ) )
	{
		retVal = SVIM_MODE_OFFLINE;
	}
	else
	{
		retVal = SVIM_MODE_UNKNOWN;
	}

	return retVal;
}

void SVSVIMStateClass::setNotificationFunction(NotifyFunctor Notify)
{
	m_Notify = Notify;
}

void SVSVIMStateClass::setPreviousToCurrentMode()
{
	svModeEnum NewMode {m_CurrentMode};
	::InterlockedExchange(reinterpret_cast<volatile long*> (&m_PreviousMode), NewMode);
}

void SVSVIMStateClass::setPreviousToCurrentTime()
{
	__time32_t NewTime {m_CurrentModifiedTime};
	::InterlockedExchange(&m_PreviousModifiedTime, NewTime);
}

void SVSVIMStateClass::CheckModeNotify()
{
	svModeEnum NewMode = GetMode();

	if (NewMode != m_CurrentMode)
	{
		::InterlockedExchange(reinterpret_cast<volatile long*> (&m_CurrentMode), NewMode);
		FireModeChanged();
	}
}

HRESULT SVSVIMStateClass::SetLastModifiedTime()
{
	HRESULT Result( S_OK );
	__time32_t LastModifiedTime = SVSVIMStateClass::m_CurrentModifiedTime;

	::InterlockedExchange(&SVSVIMStateClass::m_CurrentModifiedTime, ::_time32(nullptr));

	if (LastModifiedTime != SVSVIMStateClass::m_CurrentModifiedTime)
	{
		if (!m_Notify.empty())
		{
			//! Notify shall only signal the notify thread with Type=0
			Result = (m_Notify)(0, 0, nullptr);
		}
	}
	return Result;
}


HRESULT SVSVIMStateClass::FireModeChanged()
{
	HRESULT Result(S_OK);

	if (!m_Notify.empty())
	{
		//! Notify shall only signal the notify thread with Type=0
		Result = (m_Notify)(0, 0, nullptr);
	}
	return Result;
}
