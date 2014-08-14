//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedLastInspectedCache
//* .File Name       : $Workfile:   SVSharedLastInspectedCache.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:30  $
//******************************************************************************
#pragma once
#include "SVSharedData.h"

struct SVSharedLastInspectedCache
{
	volatile long current_idx;
	SVSharedDataVector data;

	SVSharedLastInspectedCache(const void_allocator & alloc, size_t cache_size);
};

typedef boost::interprocess::allocator<SVSharedLastInspectedCache, segment_manager_t> SVSharedLastInspectedCacheAllocator;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedLastInspectedCache.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:39:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
