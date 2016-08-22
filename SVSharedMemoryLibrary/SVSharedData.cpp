//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedData
//* .File Name       : $Workfile:   SVSharedData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:32  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedData.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedData::SVSharedData( const void_allocator& rAlloc, size_t numImages, size_t numValues )
	: m_Flags( ds::none )
	, m_TriggerCount( 0 )
	, m_Images( numImages, SVSharedImage(rAlloc), rAlloc )
	, m_Values( numValues, SVSharedValue(rAlloc), rAlloc )
	, m_Allocator( rAlloc )
	{
	}

	SVSharedData::SVSharedData( const SVSharedData& rData )
	: m_Flags( rData.m_Flags )
	, m_TriggerCount( rData.m_TriggerCount )
	, m_Images( rData.m_Images )
	, m_Values( rData.m_Values )
	, m_Allocator( rData.m_Allocator )
	{
	}

	const SVSharedData& SVSharedData::operator=( const SVSharedData& p_rData )
	{
		if (this != &p_rData)
		{
			m_Flags = p_rData.m_Flags;
			m_TriggerCount = p_rData.m_TriggerCount;
			m_Images = p_rData.m_Images;
			m_Values = p_rData.m_Values;
		}
		return *this;
	}

	SVValue SVSharedData::FindValue(const std::string& name) const
	{
		SVSharedValueContainer::const_iterator it = std::find_if(m_Values.begin(), m_Values.end(),
			[&name](const SVSharedValue& value)
			{
				return name == value.m_ElementName.c_str();
			}
		);
		if (it != m_Values.end()) 
		{
			return SVValue(it->m_ElementName.c_str(), 0, it->m_Status, it->m_Result);
		}
		throw std::exception((name + " not found.").c_str());
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
