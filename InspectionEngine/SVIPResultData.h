//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultData
//* .File Name       : $Workfile:   SVIPResultData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:54  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVTimerLibrary\SVClock.h"
#include "SVIPResultItemData.h"
#include "SVIPResultItemDefinition.h"
#pragma endregion Includes

struct IPResultTableData
{
	std::string m_columnName;
	SvTl::SVTimeStamp m_LastUpdateTimeStamp;
	std::vector<_variant_t> m_rowData;
};

struct SVIPResultData
{
	typedef std::map< SVIPResultItemDefinition, SVIPResultItemData > SVResultDataMap;

	SVIPResultData();
	SVIPResultData( const SVResultDataMap& p_rResultData, const std::vector <IPResultTableData>& rResultTableData, double p_ToolSetEndTime, double p_ToolSetAvgTime, double p_TriggerDistance, double p_ToolSetTime );
	SVIPResultData( const SVIPResultData& p_rObject );

	void clear();

	SVResultDataMap m_ResultData;
	std::vector <IPResultTableData> m_ResultTableData;

	double m_ToolSetEndTime;
	double m_ToolSetAvgTime;

	double m_TriggerDistance;
	double m_ToolSetTime;
};
