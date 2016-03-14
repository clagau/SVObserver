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

