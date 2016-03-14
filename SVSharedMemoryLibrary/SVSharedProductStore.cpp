//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedProductStore
//* .File Name       : $Workfile:   SVSharedProductStore.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:00  $
//******************************************************************************
#include "StdAfx.h"
#include "SVSharedProductStore.h"

SeidenaderVision::SVSharedProductStore::SVSharedProductStore(const void_allocator & allocator, size_t cache_size ): 
	current_idx(-1), data(cache_size, SVSharedProduct(allocator), allocator)
{
}

