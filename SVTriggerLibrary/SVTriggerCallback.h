//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallback
//* .File Name       : $Workfile:   SVTriggerCallback.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:57:20  $
//******************************************************************************
#ifndef SVTRIGGERCALLBACK_H
#define SVTRIGGERCALLBACK_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVTriggerCallbackFuncInterface.h"

class SVTriggerCallback
{
private:
	SVSharedPtr<SVTriggerCallbackFuncInterface> m_pFunc;

public:
	SVTriggerCallback(SVTriggerCallbackFuncInterface* pFunc);
	SVTriggerCallback(const SVTriggerCallback& rCallback);
	SVTriggerCallback& operator=(const SVTriggerCallback& rCallback);

	HRESULT Notify(unsigned long handle) const;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerCallback.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:57:20   bWalter
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
