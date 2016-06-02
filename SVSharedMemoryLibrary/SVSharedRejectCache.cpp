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
	SVSharedRejectCache::SVSharedRejectCache( const void_allocator & alloc, size_t cache_size )
	: current_idx( -1 ), data( cache_size, SVSharedData( alloc ), alloc ) 
	{
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
