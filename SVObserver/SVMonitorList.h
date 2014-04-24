//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorList
//* .File Name       : $Workfile:   SVMonitorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Apr 2014 09:24:22  $
//******************************************************************************
#pragma once
#include <set>
#include "SVutilityLibrary/SVString.h"

typedef std::set<SVString> SVMonitorItemList;

class SVMonitorList
{
	SVMonitorItemList m_dataList;				// for last inspected
	SVMonitorItemList m_imageList;				// for last inspected
	SVMonitorItemList m_rejectDataList;			// for rejects
	SVMonitorItemList m_rejectImageList;		// for rejects
	SVMonitorItemList m_conditionalDataList;	// for reject determination

public:
	SVMonitorList();
	SVMonitorList(const SVMonitorItemList& dataList, const SVMonitorItemList& imageList, const SVMonitorItemList& rejectDataList, const SVMonitorItemList& rejectImageList, const SVMonitorItemList& conditionalDataList);
	const SVMonitorItemList& GetImageList() const;
	const SVMonitorItemList& GetDataList() const;
	const SVMonitorItemList& GetRejectDataList() const;
	const SVMonitorItemList& GetRejectImageList() const;
	const SVMonitorItemList& GetConditionalDataList() const;
	void Clear();
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMonitorList.h_v  $
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
