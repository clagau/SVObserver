//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedProductStore
//* .File Name       : $Workfile:   SVSharedProductStore.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:00  $
//******************************************************************************
#pragma once
#include "SVSharedProduct.h"

namespace SeidenaderVision
{
	struct SVSharedProductStore
	{
		volatile long current_idx;
		SVSharedProductVector data;
		
		SVSharedProductStore(const void_allocator & allocator, size_t cache_size);
	};
	typedef boost::interprocess::allocator<SVSharedProductStore, segment_manager_t> ProductStoreAllocator;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedProductStore.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:10:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/