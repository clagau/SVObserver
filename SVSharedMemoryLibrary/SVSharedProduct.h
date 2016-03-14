//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedProduct
//* .File Name       : $Workfile:   SVSharedProduct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:58  $
//******************************************************************************
#pragma once

#include "SVShared.h"
#include "SVSharedInspections.h"

namespace SeidenaderVision
{
	struct SVSharedProduct
	{
		mutable volatile long m_Flags;
		volatile long m_TriggerCount;
		SVSharedInspectionMap m_Inspections;
		long InspectionSlotIndex(const std::string & inspectionShareName) const;
		void_allocator m_Allocator;

		SVSharedProduct(const void_allocator& rAlloc);
		SVSharedProduct(const SVSharedProduct& rData);

		const SVSharedProduct& operator=(const SVSharedProduct& rData);
	};
	typedef boost::interprocess::allocator<SVSharedProduct, segment_manager_t> SVSharedProductAllocator;
	typedef boost::interprocess::vector<SVSharedProduct, SVSharedProductAllocator> SVSharedProductVector;
	typedef boost::interprocess::allocator<SVSharedProductVector, segment_manager_t> SVSharedProductVectorAllocator;
}

