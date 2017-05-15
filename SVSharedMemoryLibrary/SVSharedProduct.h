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

#pragma region Includes
#include "SVShared.h"
#include "SVSharedInspections.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	struct SVSharedProduct
	{
		mutable volatile long m_Flags;
		volatile long m_TriggerCount;
		SVSharedInspectionMap m_Inspections;
		long InspectionSlotIndex(const SVString& inspectionShareName) const;
		void_allocator m_Allocator;

		SVSharedProduct(const void_allocator& rAlloc);
		SVSharedProduct(const SVSharedProduct& rData);

		const SVSharedProduct& operator=(const SVSharedProduct& rData);
	};
	typedef bip::allocator<SVSharedProduct, segment_manager_t> SVSharedProductAllocator;
	typedef bip::vector<SVSharedProduct, SVSharedProductAllocator> SVSharedProductVector;
	typedef bip::allocator<SVSharedProductVector, segment_manager_t> SVSharedProductVectorAllocator;
} //namespace SvSml

