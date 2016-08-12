// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : TriggerCallbackInformation
// * .File Name       : $Workfile:   TriggerCallbackInformation.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:44  $
// ******************************************************************************

#include "stdafx.h"
#include "TriggerBasics.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace Seidenader { namespace TriggerHandling {
		TriggerCallbackInformation::TriggerCallbackInformation() : m_pCallback(nullptr),m_IsStarted(false)
	{
		//m_TriggerParameters is initialized by default
	}

	TriggerCallbackInformation::TriggerCallbackInformation( const TriggerCallbackInformation &rOriginal )
	{
		*this = rOriginal;
	}

	void TriggerCallbackInformation::DispatchIfPossible() const
	{
		if (m_IsStarted && m_pCallback)
		{
			m_pCallback(m_TriggerParameters);
		}
	}

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
