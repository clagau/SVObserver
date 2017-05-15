//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedRejectCache
//* .File Name       : $Workfile:   SVSharedRejectCache.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVSharedData.h"
#pragma endregion Includes

namespace SvSml
{
	struct SVSharedRejectCache
	{
		volatile long current_idx;
		SVSharedDataVector data;

	SVSharedRejectCache( const void_allocator& alloc, size_t cache_size, size_t num_images, size_t num_values );
	};

	typedef bip::allocator< SVSharedRejectCache, segment_manager_t > SVSharedRejectCacheAllocator;

} //namespace SvSml
