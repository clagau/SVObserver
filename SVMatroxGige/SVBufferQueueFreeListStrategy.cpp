//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBufferQueueFreeListStrategy
//* .File Name       : $Workfile:   SVBufferQueueFreeListStrategy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:30  $
//******************************************************************************
#include "Stdafx.h"
#include "SVBufferQueueFreeListStrategy.h"

bool SVBufferQueueFreeListStrategy::Init()
{
	return m_freeList.Create() ? true : false;
}

bool SVBufferQueueFreeListStrategy::Destroy()
{
	return m_freeList.Destroy() ? true : false;
}

bool SVBufferQueueFreeListStrategy::SetInUse(unsigned long& rHandle)
{
	bool bRetVal = false;

	if (m_freeList.IsCreated())
	{
		long l_lSize = 0;

		// 
		if (m_freeList.GetSize( l_lSize ) && 0 < l_lSize)
		{
			unsigned long l_ulTemp = 0;

			if (m_freeList.RemoveHead((void **)&l_ulTemp))
			{
				rHandle = l_ulTemp;
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

bool SVBufferQueueFreeListStrategy::SetAvailable(long handle)
{
	bool bRetVal = false;

	if (m_freeList.IsCreated())
	{
		m_freeList.AddTail( (void *)handle);
		bRetVal = true;
	}
	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVBufferQueueFreeListStrategy.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:36:30   bWalter
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
