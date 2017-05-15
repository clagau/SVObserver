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
#pragma region Includes
#include "StdAfx.h"
#include "SVSharedProductStore.h"
#pragma endregion Includes

namespace SvSml
{
	SVSharedProductStore::SVSharedProductStore(const void_allocator& rAllocator, size_t cache_size)
	: current_idx(-1), data(cache_size, SVSharedProduct(rAllocator), rAllocator)
	{
	}
} //namespace SvSml
