//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHandleManager.h
//* .File Name       : $Workfile:   SVHandleManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:56  $
//******************************************************************************
#ifndef SVHANDLEMANAGER_H
#define SVHANDLEMANAGER_H

#include <bitset>

template<long NumBits, typename HandleType, HandleType InvalidHandle>
class SVHandleManager
{
private:
	std::bitset<NumBits> m_handleList;

public:
	typedef HandleType HandleType;

	SVHandleManager();
	~SVHandleManager();

	HandleType AcquireHandle();
	void ReleaseHandle(HandleType handle);

	bool IsValidHandle(HandleType handle) const;
	bool IsHandleAcquired(HandleType handle) const;

	void clear();
};

#include "SVHandleManager.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVHandleManager.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:21:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 07:54:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

