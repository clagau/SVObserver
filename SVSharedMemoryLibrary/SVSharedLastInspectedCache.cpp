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

namespace SvSml
{
	SVSharedLastInspectedCache::SVSharedLastInspectedCache( const void_allocator& rAlloc, size_t cache_size, size_t num_images, size_t num_values )
	: current_idx( -1 ), data( cache_size, SVSharedData( rAlloc, num_images, num_values ), rAlloc ) 
	{
	}
} //namespace SvSml

