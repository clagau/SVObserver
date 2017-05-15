//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedProduct
//* .File Name       : $Workfile:   SVSharedProduct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:56  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVSharedProduct.h"
#pragma endregion Includes

namespace SvSml
{
	SVSharedProduct::SVSharedProduct(const void_allocator & allocator)
	: m_Flags(ds::none)
	, m_Inspections(std::less< bip_string >(), allocator) 
	, m_TriggerCount(-1)
	, m_Allocator(allocator)
	{
	}

	SVSharedProduct::SVSharedProduct(const SVSharedProduct& rData)
	: m_Flags(rData.m_Flags)
	, m_Inspections(rData.m_Inspections)
	, m_TriggerCount(rData.m_TriggerCount)
	, m_Allocator(rData.m_Allocator)
	{
	}

	const SVSharedProduct& SVSharedProduct::operator=(const SVSharedProduct& rData)
	{
		if (this != &rData)
		{
			m_Flags = rData.m_Flags;
			m_Inspections = rData.m_Inspections;
			m_TriggerCount = rData.m_TriggerCount;
		}
		return *this;
	}

	long SVSharedProduct::InspectionSlotIndex(const SVString & inspectionShareName) const
	{
		bip_string key(inspectionShareName.c_str(), m_Allocator);
		SVSharedInspectionMap::const_iterator it = m_Inspections.find(key);
		if (it != m_Inspections.end())
		{
			return it->second.m_Index;
		}
		throw std::exception("Key not found");
	}
} //namespace SvSml
