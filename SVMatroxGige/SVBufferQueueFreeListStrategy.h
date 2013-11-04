//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBufferQueueFreeListStrategy
//* .File Name       : $Workfile:   SVBufferQueueFreeListStrategy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:36  $
//******************************************************************************
#ifndef SVBUFFERQUEUEFREELISTSTRATEGY_H
#define SVBUFFERQUEUEFREELISTSTRATEGY_H

#include "SVOLibrary/SVQueueObject.h"

class SVBufferQueueFreeListStrategy
{
	SVQueueObject m_freeList;

public:
	bool Init();
	bool Destroy();
	bool SetInUse(unsigned long& handle);
	bool SetAvailable(long handle);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVBufferQueueFreeListStrategy.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:36:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:17:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

