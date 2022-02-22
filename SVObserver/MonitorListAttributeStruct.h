//******************************************************************************
/// \copyright COPYRIGHT (c) 2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MonitorListAttributeStruct.h
/// All Rights Reserved
//*****************************************************************************
/// Struct for the attribute of the monitorlist and a PPQMonitor-Map Define.
//******************************************************************************
#pragma once

//Moved to precompiled header: #include <map>


struct MonitorListAttributeStruct
{
	bool m_isActive = false;
	int m_rejectDepth = 1;

	MonitorListAttributeStruct(bool isActive, int rejectDepth) : m_isActive(isActive), m_rejectDepth(rejectDepth) {};
	MonitorListAttributeStruct() = default;
	~MonitorListAttributeStruct() = default;
};
typedef std::map<std::string, MonitorListAttributeStruct> PPQMonitorList;
