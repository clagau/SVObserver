//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorNamedList
//* .File Name       : $Workfile:   RemoteMonitorNamedList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   29 Aug 2014 17:49:02  $
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#include "RemoteMonitorNamedList.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVLibrary/SVOIniClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

const TCHAR* SharedMemorySectionKey = _T("SharedMemory");
const TCHAR* DefaultRejectQeueDepthKey = _T("DefaultRejectQueueDepth");
const TCHAR* MaxRejectQeueDepthKey = _T("MaxRejectQueueDepth");

RemoteMonitorNamedList::RemoteMonitorNamedList(): m_bActive(false)
{
  m_rejectQueueDepth = GetDefaultRejectQueueDepth();
}

RemoteMonitorNamedList::RemoteMonitorNamedList(const SVString& PPQName, const SVString& name)
: m_PPQName(PPQName)
, m_name(name)
, m_bActive(false)
{
	 m_rejectQueueDepth = GetDefaultRejectQueueDepth();
	ResolveGuidForPPQName();
}

RemoteMonitorNamedList::RemoteMonitorNamedList(const SVString& PPQName, const SVString& name, const MonitoredObjectList& productValuesList, const MonitoredObjectList& productImageList, const MonitoredObjectList& rejectConditionList, const MonitoredObjectList& failStatusList, int rejectDepth)
: m_PPQName(PPQName)
, m_name(name)
, m_productValuesList(productValuesList)
, m_productImagesList(productImageList)
, m_rejectConditionList(rejectConditionList)
, m_failStatusList(failStatusList)
, m_rejectQueueDepth(rejectDepth)
, m_bActive(false)
{
	ResolveGuidForPPQName();
}

const SVGUID& RemoteMonitorNamedList::GetPPQObjectID() const
{
	return m_PPQObjectID;
}

const SVString& RemoteMonitorNamedList::GetPPQName() const
{
	return m_PPQName;
}

void RemoteMonitorNamedList::SetPPQName(const SVString& PPQName)
{
	m_PPQName = PPQName;
	ResolveGuidForPPQName();
}

const SVString& RemoteMonitorNamedList::GetName() const
{
	return m_name;
}

void RemoteMonitorNamedList::SetName(const SVString& name)
{
	m_name = name;
}

int RemoteMonitorNamedList::GetRejectDepthQueue() const
{
	return m_rejectQueueDepth;
}

void RemoteMonitorNamedList::SetRejectDepthQueue(int depth)
{
	m_rejectQueueDepth = depth;
}

const MonitoredObjectList& RemoteMonitorNamedList::GetProductValuesList() const
{
	return m_productValuesList;
}

void RemoteMonitorNamedList::SetProductValuesList(const MonitoredObjectList& list)
{
	m_productValuesList = list;
}

const MonitoredObjectList& RemoteMonitorNamedList::GetProductImagesList() const
{
	return m_productImagesList;
}

void RemoteMonitorNamedList::SetProductImagesList(const MonitoredObjectList& list)
{
	m_productImagesList = list;
}

const MonitoredObjectList& RemoteMonitorNamedList::GetRejectConditionList() const
{
	return m_rejectConditionList;
}

void RemoteMonitorNamedList::SetRejectConditionList(const MonitoredObjectList& list)
{
	m_rejectConditionList = list;
}

const MonitoredObjectList& RemoteMonitorNamedList::GetFailStatusList() const
{
	return m_failStatusList;
}

void RemoteMonitorNamedList::SetFailStatusList(const MonitoredObjectList& list)
{
	m_failStatusList = list;
}

void RemoteMonitorNamedList::Activate(bool bActivate)
{
	m_bActive = bActivate;
}

bool RemoteMonitorNamedList::IsActive() const
{
	return m_bActive;
}

void RemoteMonitorNamedList::SetProductFilter(SvSml::SVProductFilterEnum filter)
{
	m_filter = filter;
}

SvSml::SVProductFilterEnum RemoteMonitorNamedList::GetProductFilter() const
{
	return m_filter;
}

void RemoteMonitorNamedList::ResolveGuidForPPQName()
{
	m_PPQObjectID = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(m_PPQName.c_str());
}



int  RemoteMonitorNamedList::GetDefaultRejectQueueDepth()
{
	static int DefaultRejectQueueDepth = -1;
	if(DefaultRejectQueueDepth < 0)
	{
		SvLib::SVOINIClass reader(SvStl::GlobalPath::Inst().GetSVIMIniPath());
		DefaultRejectQueueDepth= reader.GetValueInt(SharedMemorySectionKey, DefaultRejectQeueDepthKey, 10);
		DefaultRejectQueueDepth  = std::max(DefaultRejectQueueDepth,RemoteMonitorNamedList::MinRejectQueueDepth);
		DefaultRejectQueueDepth = std::min(DefaultRejectQueueDepth, GetMaxRejectQueueDepth());
	}
	return DefaultRejectQueueDepth;
}

int  RemoteMonitorNamedList::GetMaxRejectQueueDepth()
{
	static int MaxRejectQueueDepth = -1;
	if(MaxRejectQueueDepth < 0)
	{
		SvLib::SVOINIClass reader(SvStl::GlobalPath::Inst().GetSVIMIniPath());
		MaxRejectQueueDepth= reader.GetValueInt(SharedMemorySectionKey, MaxRejectQeueDepthKey, 50);
		MaxRejectQueueDepth = std::max(MaxRejectQueueDepth, RemoteMonitorNamedList::MinRejectQueueDepth ); 
	}
	return MaxRejectQueueDepth;
}

