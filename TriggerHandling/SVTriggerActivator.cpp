//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerActivator
//* .File Name       : $Workfile:   SVTriggerActivator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:56:04  $
//******************************************************************************
#include "stdafx.h"
#include "SVTriggerActivator.h"

namespace SvTh
{
	SVTriggerActivator::SVTriggerActivator(SVTriggerActivatorFuncInterface* pFunc)
		: m_pFunc(pFunc)
	{
	}

	SVTriggerActivator::SVTriggerActivator(const SVTriggerActivator& rActivator)
		: m_pFunc(rActivator.m_pFunc)
	{
	}

	SVTriggerActivator& SVTriggerActivator::operator=(const SVTriggerActivator& rActivator)
	{
		m_pFunc = rActivator.m_pFunc;
		return *this;
	}

	HRESULT SVTriggerActivator::Exec(unsigned long handle) const
	{
		HRESULT hr = (*m_pFunc)(handle);
		return hr;
	}

}//namespace SvTh