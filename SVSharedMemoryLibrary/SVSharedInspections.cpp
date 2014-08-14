//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspections
//* .File Name       : $Workfile:   SVSharedInspections.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVSharedInspections.h"

SVSharedInspection::SVSharedInspection(const char* inspectionShareName, long index, const void_allocator& rAlloc)
: m_ShareName(inspectionShareName, rAlloc)
, m_Index(index)
, m_Allocator(rAlloc)
{
}

SVSharedInspection::SVSharedInspection(const void_allocator& rAlloc)
: m_ShareName("", rAlloc)
, m_Index(-1)
, m_Allocator(rAlloc)
{
}

SVSharedInspection::SVSharedInspection(const SVSharedInspection& rData)
: m_ShareName(rData.m_ShareName.c_str(), rData.m_Allocator)
, m_Index(rData.m_Index)
, m_Allocator(rData.m_Allocator)
{
}

const SVSharedInspection& SVSharedInspection::operator=(const SVSharedInspection& rData)
{
	if (this != &rData)
	{
		m_ShareName = rData.m_ShareName;
		m_Index = rData.m_Index;
	}
	return *this;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedInspections.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
