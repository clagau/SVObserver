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

#include "stdafx.h"
#include "SVOTriggerObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVOTriggerObj::SVOTriggerObj(const SVString& name, int nDig)
: m_sTriggerDisplayName(name)
, m_iDigNumber(nDig)
, m_timerPeriod(SvTh::TimerPeriod)
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
	bool bRet = (m_sTriggerDisplayName.find(SvTh::CameraTriggerName) == 0);
	return bRet;
}

