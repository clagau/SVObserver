//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerHandler
//* .File Name       : $Workfile:   SVTriggerHandler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:59:36  $
//******************************************************************************
#ifndef SVTRIGGERHANDLER_H
#define SVTRIGGERHANDLER_H

#include <map>
#include "SVTriggerActivator.h"
#include "SVTriggerCallback.h"

class SVTriggerHandler
{
private:
	unsigned long m_handle;
	bool m_bFired;
	SVTriggerActivator m_activator;
	SVTriggerCallback m_callback;

public:
	SVTriggerHandler(unsigned long handle, SVTriggerActivator& activator, SVTriggerCallback& callback);
	SVTriggerHandler(const SVTriggerHandler& rHandler);
	SVTriggerHandler& operator=(const SVTriggerHandler& rHandler);
	
	HRESULT Fire();
	void SetFired(bool bFireState);
	bool HasFired() const;

	void Notify();
};

typedef std::map<unsigned long, SVTriggerHandler> SVTriggerHandlerList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerHandler.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:59:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:03:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
