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
#ifndef SVTIMERCALLBACKIMPL_H
#define SVTIMERCALLBACKIMPL_H

#include <boost/config.hpp>
#include <boost/function.hpp>
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
	virtual void Notify(const SVString& listenerTag) { if (m_pObj) m_func(m_pObj, listenerTag); }
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTimerLibrary\SVTimerCallbackImpl.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:36:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 09:55:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
