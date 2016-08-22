//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedRejectCache
//* .File Name       : $Workfile:   SVSharedRejectCache.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:02  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedRejectCache.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedRejectCache::SVSharedRejectCache( const void_allocator& rAlloc, size_t cache_size, size_t num_images, size_t num_values )
	: current_idx( -1 ), data( cache_size, SVSharedData( rAlloc, num_images, num_values ), rAlloc ) 
	{
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
