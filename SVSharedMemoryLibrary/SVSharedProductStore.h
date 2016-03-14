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

