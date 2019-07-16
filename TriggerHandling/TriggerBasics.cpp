// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : TriggerDispatcher
// * .File Name       : $Workfile:   TriggerDispatcher.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:44  $
// ******************************************************************************

#include "stdafx.h"
#include "TriggerBasics.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace SvTh
{
	TriggerDispatcher::TriggerDispatcher(const SVTriggerCallbackPtr callback, const TriggerParameters &tp):
			m_pCallback(callback), m_TriggerParameters(tp),
				m_IsStarted(false)
	{
		//m_TriggerParameters is initialized by default
	}

	TriggerDispatcher::TriggerDispatcher( const TriggerDispatcher &rOriginal )
	{
		*this = rOriginal;
	}

	bool TriggerDispatcher::operator==(const TriggerDispatcher& rDispatcher) const 
	{
		if(m_pCallback == rDispatcher.m_pCallback)
		{
			if (m_TriggerParameters.m_Data == rDispatcher.m_TriggerParameters.m_Data &&
				m_TriggerParameters.m_pOwner == rDispatcher.m_TriggerParameters.m_pOwner)
			{
				return true;
			}
		}
		return false;
	}


	void TriggerDispatcher::Dispatch() const 
	{
		m_pCallback(m_TriggerParameters);
	}

	void TriggerDispatcher::DispatchIfPossible() const
	{
		if (m_IsStarted && m_pCallback)
		{
			Dispatch();
		}
	}

	void TriggerDispatcher::clear()
	{
		m_pCallback = nullptr;
		m_TriggerParameters = TriggerParameters();
	}

} //namespace SvTh

