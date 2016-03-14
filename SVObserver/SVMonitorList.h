//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorList
//* .File Name       : $Workfile:   SVMonitorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Jun 2014 17:36:58  $
//******************************************************************************
#pragma once
#include <set>
#include <map>
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

struct RejectDepthAndMonitorList
{
	int rejectDepth;
	SVMonitorList monitorList;
};
typedef std::pair<bool, RejectDepthAndMonitorList> ActiveMonitorList;
typedef std::map<SVString, ActiveMonitorList> PPQMonitorList;
