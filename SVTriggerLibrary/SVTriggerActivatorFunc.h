//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerActivatorFunc
//* .File Name       : $Workfile:   SVTriggerActivatorFunc.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:56:34  $
//******************************************************************************
#ifndef SVTRIGGERACTIVATORFUNC_H
#define SVTRIGGERACTIVATORFUNC_H

#include "SVTriggerActivatorFuncInterface.h"

template <typename T>
class SVTriggerActivatorFunc : public SVTriggerActivatorFuncInterface
{
private:
	typedef HRESULT (T::* MemFunc)(unsigned long);
	mutable T* m_pObject;
	MemFunc m_func;

public:
	SVTriggerActivatorFunc(T* pObj, MemFunc func) : m_pObject(pObj), m_func(func) {}
	SVTriggerActivatorFunc(const SVTriggerActivatorFunc& rFunc)
	: m_pObject(rFunc.m_pObject), m_func(rFunc.m_func)
	{
	}

	SVTriggerActivatorFunc& operator=(const SVTriggerActivatorFunc& rFunc)
	{
		m_pObject = rFunc.m_pObject;
		m_func = rFunc.m_func;
	}

	virtual ~SVTriggerActivatorFunc() {}

	virtual HRESULT operator()(unsigned long handle) const
	{
		HRESULT hr = S_FALSE;
		if (m_pObject && m_func)
			hr = (m_pObject->*m_func)(handle);
		return hr;
	}
};

#endif

