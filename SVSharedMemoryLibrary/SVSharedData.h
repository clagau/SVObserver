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

//Moved to precompiled header: #include <memory>
#include "SVSharedImage.h"
#include "SVSharedValue.h"

struct SVSharedData
{
	mutable volatile long m_Flags;
	volatile long m_TriggerCount;
	SVSharedImageMap m_Images;
	SVSharedValueMap m_Values;

	void_allocator m_Allocator;

	SVSharedData( const void_allocator& p_rAlloc );
	SVSharedData( const SVSharedData& p_rData );

	const SVSharedData& operator=( const SVSharedData& p_rData );

	SeidenaderVision::SVValue FindValue(const std::string & name) const;
};

typedef boost::interprocess::allocator< SVSharedData, segment_manager_t > SVSharedDataAllocator;
typedef boost::interprocess::vector< SVSharedData, SVSharedDataAllocator > SVSharedDataVector;
typedef boost::interprocess::allocator< SVSharedDataVector, segment_manager_t > SVSharedDataVectorAllocator;

//typedef std::shared_ptr<SVSharedData> InspectionDataPtr;
typedef SVSharedData * InspectionDataPtr;
