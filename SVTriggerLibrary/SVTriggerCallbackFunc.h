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
#ifndef SVTRIGGERCALLBACKFUNC_H
#define SVTRIGGERCALLBACKFUNC_H

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerCallbackFunc.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:57:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:00:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
