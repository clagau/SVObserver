//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedData
//* .File Name       : $Workfile:   SVSharedData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:32  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVSharedImageContainer.h"
#include "SVSharedValueContainer.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	struct SVSharedData
	{
		mutable volatile long m_Flags;
		volatile long m_TriggerCount;
		SVSharedImageContainer m_Images;
		SVSharedValueContainer m_Values;

		void_allocator m_Allocator;

		SVSharedData( const void_allocator& rAlloc, size_t numImages, size_t numValues );
		SVSharedData( const SVSharedData& rData );

		const SVSharedData& operator=( const SVSharedData& rData );

		SVValue FindValue(const std::string& name) const;
	};

	typedef boost::interprocess::allocator< SVSharedData, segment_manager_t > SVSharedDataAllocator;
	typedef boost::interprocess::vector< SVSharedData, SVSharedDataAllocator > SVSharedDataVector;
	typedef boost::interprocess::allocator< SVSharedDataVector, segment_manager_t > SVSharedDataVectorAllocator;

	//typedef std::shared_ptr<SVSharedData> InspectionDataPtr;
	typedef SVSharedData* InspectionDataPtr;

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
