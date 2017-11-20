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
//Moved to precompiled header: #include <list>
#include "SVSVIMStateClass.h"
#include "Definitions/GlobalConst.h"
#include "ObjectInterfaces/IRootObject.h"

#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)

long SVSVIMStateClass::m_SVIMState = SV_STATE_AVAILABLE;

bool SVSVIMStateClass::m_AutoSaveRequired = false; ///< should an autosave be performed at the next appropriate time?
__time32_t SVSVIMStateClass::m_LastModifiedTime = 0;
__time32_t SVSVIMStateClass::m_PrevModifiedTime = 0;
svModeEnum SVSVIMStateClass::m_prevMode = SVIM_MODE_UNKNOWN;
svModeEnum SVSVIMStateClass::m_lastMode = SVIM_MODE_UNKNOWN;
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
	svModeEnum mode = GetMode();
	CheckModeNotify(mode);
	setEnvironmentParameters(mode);
	return true;
}

bool SVSVIMStateClass::RemoveState( DWORD dwState )
{
	::_InterlockedAnd( &m_SVIMState, ~dwState );
	svModeEnum mode = GetMode();
	CheckModeNotify(mode);
	setEnvironmentParameters(mode);

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
	else if( SVSVIMStateClass::CheckState( SV_STATE_READY ) &&
		!SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
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

void SVSVIMStateClass::CheckModeNotify(svModeEnum mode)
{
	static svModeEnum currentMode = SVIM_MODE_UNKNOWN;
	if (mode != currentMode)
	{
		currentMode = mode;
		FireModeChanged(mode);
	}
}

void SVSVIMStateClass::setEnvironmentParameters(svModeEnum mode)
{
	long modeValue = static_cast<long>(mode);

	SvOi::setRootChildValue( SvDef::FqnEnvironmentModeValue, modeValue );
	SvOi::setRootChildValue( SvDef::FqnEnvironmentModeIsRun, ( SVIM_MODE_ONLINE == mode ) );
	SvOi::setRootChildValue( SvDef::FqnEnvironmentModeIsStop, ( SVIM_MODE_OFFLINE == mode ) );
	SvOi::setRootChildValue( SvDef::FqnEnvironmentModeIsRegressionTest, ( SVIM_MODE_REGRESSION == mode ) );
	SvOi::setRootChildValue( SvDef::FqnEnvironmentModeIsTest, ( SVIM_MODE_TEST == mode ) );
	SvOi::setRootChildValue( SvDef::FqnEnvironmentModeIsEdit, ( SVIM_MODE_EDIT == mode ) );
}


HRESULT SVSVIMStateClass::SetLastModifiedTime()
{
	HRESULT Result( S_OK );
	__time32_t l_LastModifiedTime = SVSVIMStateClass::m_LastModifiedTime;

	::InterlockedExchange(&SVSVIMStateClass::m_LastModifiedTime, ::_time32(nullptr));

	if (l_LastModifiedTime != SVSVIMStateClass::m_LastModifiedTime)
	{
		if (!m_Notify.empty())
		{
			//! Notify shall only signal the notify thread with Type=0
			Result = (m_Notify)(0, 0, nullptr);
		}
	}
	return Result;
}


HRESULT SVSVIMStateClass::FireModeChanged(svModeEnum mode)
{
	HRESULT Result(S_OK);

	::InterlockedExchange((long *)&m_lastMode, mode);

	if (!m_Notify.empty())
	{
		//! Notify shall only signal the notify thread with Type=0
		Result = (m_Notify)(0, 0, nullptr);
	}
	return Result;
}
