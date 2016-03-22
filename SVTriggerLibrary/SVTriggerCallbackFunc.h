//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallbackFunc
//* .File Name       : $Workfile:   SVTriggerCallbackFunc.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:57:36  $
//******************************************************************************

#pragma once

#include "SVTriggerCallbackFuncInterface.h"

template <typename T>
class SVTriggerCallbackFunc : public SVTriggerCallbackFuncInterface
{
private:
	typedef HRESULT (T::* MemFunc)(unsigned long);
	mutable T* m_pObject;
	MemFunc m_func;

public:
	SVTriggerCallbackFunc(T* pObj, MemFunc func) : m_pObject(pObj), m_func(func) {}
	SVTriggerCallbackFunc(const SVTriggerCallbackFunc& rFunc)
	: m_pObject(rFunc.m_pObject), m_func(rFunc.m_func)
	{
	}
	SVTriggerCallbackFunc& operator=(const SVTriggerCallbackFunc& rFunc)
	{
		m_pObject = rFunc.m_pObject;
		m_func = rFunc.m_func;
	}
	virtual ~SVTriggerCallbackFunc() {}

	virtual HRESULT operator()(unsigned long handle) const 
	{
		HRESULT hr = S_FALSE;
		if (m_pObject && m_func)
			hr = (m_pObject->*m_func)(handle);
		return hr;
	}
};

