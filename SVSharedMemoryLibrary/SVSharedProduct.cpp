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
#include "StdAfx.h"
#include "SVSharedProduct.h"

using namespace SeidenaderVision;

SVSharedProduct::SVSharedProduct(const void_allocator & allocator)
: m_Flags(ds::none)
, m_Inspections(std::less< char_string >(), allocator) 
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

long SVSharedProduct::InspectionSlotIndex(const std::string & inspectionShareName) const
{
	char_string key(inspectionShareName.c_str(), m_Allocator);
	SVSharedInspectionMap::const_iterator it = m_Inspections.find(key);
	if (it != m_Inspections.end())
	{
		return it->second.m_Index;
	}
	throw std::exception("Key not found");
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedProduct.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:09:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/