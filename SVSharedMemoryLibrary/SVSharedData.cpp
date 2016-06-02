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
	SVSharedData::SVSharedData( const void_allocator& p_rAlloc )
	: m_Flags( ds::none )
	, m_TriggerCount( 0 )
	, m_Images( std::less< char_string >(), p_rAlloc )
	, m_Values( std::less< char_string >(), p_rAlloc )
	, m_Allocator( p_rAlloc )
	{
	}

	SVSharedData::SVSharedData( const SVSharedData& p_rData )
	: m_Flags( p_rData.m_Flags )
	, m_TriggerCount( p_rData.m_TriggerCount )
	, m_Images( p_rData.m_Images )
	, m_Values( p_rData.m_Values )
	, m_Allocator( p_rData.m_Allocator )
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

	SVValue SVSharedData::FindValue(const std::string & name) const
	{
		SVSharedValueMap::const_iterator it = std::find_if(m_Values.begin(), m_Values.end(),
			[&name](const std::pair<char_string, SVSharedValue> & pair)
			{
				return name == pair.first.c_str();
			}
		);
		if (it != m_Values.end()) 
		{
			return SVValue(
				it->first.c_str(), 
				0,
				it->second.m_Status,
				it->second.m_Result.c_str()
				);
		}
		throw std::exception((name + " not found.").c_str());
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
