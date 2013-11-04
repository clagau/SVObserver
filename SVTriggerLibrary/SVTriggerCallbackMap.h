//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallbackMap
//* .File Name       : $Workfile:   SVTriggerCallbackMap.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:22  $
//******************************************************************************
#ifndef SVTRIGGERCALLBACKMAP_H
#define SVTRIGGERCALLBACKMAP_H

#include <map>
#include "SVTriggerCallbackStruct.h"

typedef std::map<unsigned long, SVTriggerCallbackList> TriggerCallbackMap;

class SVTriggerCallbackMap
{
private:
	TriggerCallbackMap m_triggerCallbacks;

public:
	void Add(unsigned long p_ulIndex, SVTriggerCallbackStruct& callback);
	void Remove(unsigned long p_ulIndex, SVTriggerCallbackStruct& callback);
	void RemoveAll(unsigned long p_ulIndex);

	HRESULT Dispatch(unsigned long p_ulIndex);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerCallbackMap.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:58:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:00:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
