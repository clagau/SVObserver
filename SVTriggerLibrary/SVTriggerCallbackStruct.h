//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallbackStruct
//* .File Name       : $Workfile:   SVTriggerCallbackStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:50  $
//******************************************************************************

#ifndef SVTRIGGERCALLBACKSTRUCT_H
#define SVTRIGGERCALLBACKSTRUCT_H

#include <map>
#include <deque>
#include "SVTriggerCallbackPtr.h"

struct SVTriggerCallbackStruct
{
	SVTriggerCallbackPtr pCallback;
	void* pOwner;
	void* pData;

	SVTriggerCallbackStruct() : pCallback(0), pOwner(0), pData(0) {}
};

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
typedef std::deque<SVTriggerCallbackStruct> SVTriggerCallbackList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerCallbackStruct.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:58:50   bWalter
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
