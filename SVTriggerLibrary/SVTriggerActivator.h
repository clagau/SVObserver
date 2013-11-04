//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerActivator
//* .File Name       : $Workfile:   SVTriggerActivator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:56:20  $
//******************************************************************************
#ifndef SVTRIGGERACTIVATOR_H
#define SVTRIGGERACTIVATOR_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVTriggerActivatorFuncInterface.h"

class SVTriggerActivator
{
private:
	SVSharedPtr<SVTriggerActivatorFuncInterface> m_pFunc;

public:
	SVTriggerActivator(SVTriggerActivatorFuncInterface* pFunc);
	SVTriggerActivator(const SVTriggerActivator& rActivator);
	SVTriggerActivator& operator=(const SVTriggerActivator& rActivator);

	HRESULT Exec(unsigned long handle) const;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerActivator.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:56:20   bWalter
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


