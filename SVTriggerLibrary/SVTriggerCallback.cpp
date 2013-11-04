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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerCallback.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:57:04   bWalter
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
