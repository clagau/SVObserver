//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTimerCallbackImpl
//* .File Name       : $Workfile:   SVTimerCallbackImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:36:40  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVTimerCallback.h"

template <typename T, typename R>
class SVTimerCallbackImpl : public SVTimerCallback
{
public:
	typedef boost::function<R (T*, const SVString&)> SVFunc;
	typedef R (T::* MemFunc)(const SVString&);

private:
	SVFunc m_func;
	T* m_pObj;

public:
	SVTimerCallbackImpl() : m_pObj(0) {}
	SVTimerCallbackImpl(T* pObj, MemFunc func) : m_func(func), m_pObj(pObj) {}
	virtual ~SVTimerCallbackImpl() {}
	
	void Bind(T* pObj, MemFunc func) { m_func = func; m_pObj = pObj; }
	virtual void Notify(const SVString& listenerTag) override { if (m_pObj) m_func(m_pObj, listenerTag); }
};

