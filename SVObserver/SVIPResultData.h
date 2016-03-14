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

#ifndef SVIPRESULTDATA_H
#define SVIPRESULTDATA_H

#include <map>

#include "SVIPResultItemData.h"
#include "SVIPResultItemDefinition.h"

struct SVIPResultData
{
	typedef std::map< SVIPResultItemDefinition, SVIPResultItemData > SVResultDataMap;

	SVIPResultData();
	SVIPResultData( const SVResultDataMap& p_rResultData, double p_ToolSetEndTime, double p_ToolSetAvgTime, double p_TriggerDistance, double p_ToolSetTime );
	SVIPResultData( const SVIPResultData& p_rObject );

	void clear();

	SVResultDataMap m_ResultData;

	double m_ToolSetEndTime;
	double m_ToolSetAvgTime;

	double m_TriggerDistance;
	double m_ToolSetTime;

};

#endif

