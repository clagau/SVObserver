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
//#include "SVSharedImageContainer.h"
//#include "SVSharedValueContainer.h"
#include "SVSharedImage.h"
#include "SVSharedValue.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	///struct used in InspectionShare  ImagePathes  and values for one triggercount 
	struct SVSharedData
	{
		mutable volatile long m_Flags;
		volatile long m_TriggerCount;
		SVSharedImageVector m_Images;
		SVSharedValueVector m_Values;

		void_allocator m_Allocator;

		SVSharedData( const void_allocator& rAlloc, size_t numImages, size_t numValues );
		SVSharedData( const SVSharedData& rData );

		const SVSharedData& operator=( const SVSharedData& rData );

		SVValue FindValue(const SVString& name) const;
	};

	typedef bip::allocator< SVSharedData, segment_manager_t > SVSharedDataAllocator;
	typedef bip::vector< SVSharedData, SVSharedDataAllocator > SVSharedDataVector;
	typedef bip::allocator< SVSharedDataVector, segment_manager_t > SVSharedDataVectorAllocator;

} //namespace SvSml
