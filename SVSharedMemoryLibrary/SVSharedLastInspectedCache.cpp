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

#include "stdafx.h"
#include "SVSharedLastInspectedCache.h"

SVSharedLastInspectedCache::SVSharedLastInspectedCache( const void_allocator & alloc, size_t cache_size )
: current_idx( -1 ), data( cache_size, SVSharedData( alloc ), alloc ) 
{
}

