//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedLastInspectedCache
//* .File Name       : $Workfile:   SVSharedLastInspectedCache.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:46  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedLastInspectedCache.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedLastInspectedCache::SVSharedLastInspectedCache( const void_allocator & alloc, size_t cache_size, size_t num_images, size_t num_values )
	: current_idx( -1 ), data( cache_size, SVSharedData( alloc, num_images, num_values ), alloc ) 
	{
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

