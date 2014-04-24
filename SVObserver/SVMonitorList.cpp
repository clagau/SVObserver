//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorList
//* .File Name       : $Workfile:   SVMonitorList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Apr 2014 09:24:22  $
//******************************************************************************
#include "stdafx.h"

#include <string>
#include <deque>
#include "SVMonitorList.h"

SVMonitorList::SVMonitorList()
{
}

SVMonitorList::SVMonitorList(const SVMonitorItemList& dataList, const SVMonitorItemList& imageList, const SVMonitorItemList& rejectDataList, const SVMonitorItemList& rejectImageList, const SVMonitorItemList& conditionalDataList)
: m_dataList(dataList)
, m_imageList(imageList)
, m_rejectDataList(rejectDataList)
, m_rejectImageList(rejectImageList)
, m_conditionalDataList(conditionalDataList)
{
}

// This is for Last Inspected
const SVMonitorItemList& SVMonitorList::GetDataList() const
{
	return m_dataList;
}

// This is for Last Inspected
const SVMonitorItemList& SVMonitorList::GetImageList() const
{
	return m_imageList;
}

// This is for Rejects
const SVMonitorItemList& SVMonitorList::GetRejectDataList() const
{
	return m_rejectDataList;
}

// This is for Rejects
const SVMonitorItemList& SVMonitorList::GetRejectImageList() const
{
	return m_rejectImageList;
}

// This is for Reject Determination
const SVMonitorItemList& SVMonitorList::GetConditionalDataList() const
{
	return m_conditionalDataList;
}

void SVMonitorList::Clear()
{
	m_imageList.clear();
	m_dataList.clear();
	
	m_rejectDataList.clear();
	m_rejectImageList.clear();

	m_conditionalDataList.clear();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMonitorList.cpp_v  $
 * 
 *    Rev 1.1   22 Apr 2014 09:24:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to correct issue with PVCS log section.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
