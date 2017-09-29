//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallback
//* .File Name       : $Workfile:   SVTriggerCallback.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:57:04  $
//******************************************************************************
#include "stdafx.h"
#include "SVTriggerCallback.h"

namespace SvTh
{
	SVTriggerCallback::SVTriggerCallback(SVTriggerCallbackFuncInterface* pFunc)
	: m_pFunc(pFunc)
	{
	}

	SVTriggerCallback::SVTriggerCallback(const SVTriggerCallback& rCallback)
	: m_pFunc(rCallback.m_pFunc)
	{
	}

	SVTriggerCallback& SVTriggerCallback::operator=(const SVTriggerCallback& rCallback)
	{
		m_pFunc = rCallback.m_pFunc;
		return *this;
	}

	HRESULT SVTriggerCallback::Notify(unsigned long handle) const
	{
		return (*m_pFunc)(handle);
	}
} //namespace SvTh
