//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspection
//* .File Name       : $Workfile:   SVSharedInspections.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:42  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVShared.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	struct SVSharedInspection
	{
		bip_string m_ShareName;
		long m_Index;

		void_allocator m_Allocator;

		SVSharedInspection(const char* inspectionShareName, long index, const void_allocator& rAlloc);
		SVSharedInspection(const void_allocator& rAlloc);
		SVSharedInspection(const SVSharedInspection& rData);

		const SVSharedInspection& operator=(const SVSharedInspection& rData);
	};

	typedef bip::allocator<SVSharedInspection, segment_manager_t> SVSharedInspectionAllocator;
	// The first item in the pair is the Inspection Share Name
	typedef std::pair<bip_string, SVSharedInspection> SVSharedInspectionPair; // non const key for flt map
	typedef bip::allocator<SVSharedInspectionPair, segment_manager_t> SVSharedInspectionPairAllocator;
	// The Key is the Inspection Share Name
	typedef bip::flat_map<bip_string, SVSharedInspection, std::less<bip_string>, SVSharedInspectionPairAllocator> SVSharedInspectionMap;
	typedef bip::allocator<SVSharedInspectionMap, segment_manager_t> SVSharedInspectionMapAllocator;

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
