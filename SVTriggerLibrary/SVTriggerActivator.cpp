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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerActivator.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:56:04   bWalter
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
