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

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	struct SVSharedRejectCache
	{
		volatile long current_idx;
		SVSharedDataVector data;

		SVSharedRejectCache( const void_allocator & alloc, size_t cache_size );
	};

	typedef boost::interprocess::allocator< SVSharedRejectCache, segment_manager_t > SVSharedRejectCacheAllocator;

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
