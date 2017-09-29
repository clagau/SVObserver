//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerObj
//* .File Name       : $Workfile:   SVOTriggerObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:14:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOTriggerObj.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace SvTi
{
	SVOTriggerObj::SVOTriggerObj(const SVString& name, int nDig)
	: m_sTriggerDisplayName(name)
	, m_iDigNumber(nDig)
	, m_timerPeriod(SvTi::TimerPeriod)
	, m_bSoftwareTrigger(false)
	{
	}

	SVOTriggerObj::~SVOTriggerObj()
	{
	}

	LPCTSTR SVOTriggerObj::GetTriggerDisplayName() const
	{
		return m_sTriggerDisplayName.c_str();
	}

	int SVOTriggerObj::GetTriggerDigNumber() const
	{
		return m_iDigNumber;
	}

	// For Software trigger
	void SVOTriggerObj::SetTimerPeriod(long lPeriod)
	{
		m_timerPeriod = lPeriod;
	}

	// For Software trigger
	long SVOTriggerObj::GetTimerPeriod() const
	{
		return m_timerPeriod;
	}

	bool SVOTriggerObj::IsSoftwareTrigger() const
	{
		return m_bSoftwareTrigger;
	}

	void SVOTriggerObj::SetSoftwareTrigger(bool bSoftwareTrigger)
	{
		m_bSoftwareTrigger = bSoftwareTrigger;
	}

	bool SVOTriggerObj::IsAcquisitionTrigger() const
	{
		bool bRet = ( 0 ==m_sTriggerDisplayName.find(SvTi::CameraTriggerName) );
		return bRet;
	}
} //namespace SvTi
