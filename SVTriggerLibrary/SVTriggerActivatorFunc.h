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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerActivatorFunc.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:56:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 09:59:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
