//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorNamedList
//* .File Name       : $Workfile:   RemoteMonitorNamedList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:26:48  $
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#include "RemoteMonitorNamedList.h"
#pragma endregion Includes

RemoteMonitorNamedList::RemoteMonitorNamedList()
: m_rejectQueueDepth(DefaultRejectQueueDepth)
{
}

RemoteMonitorNamedList::RemoteMonitorNamedList(const SVString& PPQName, const SVString& name)
: m_PPQName(PPQName)
, m_name(name)
, m_rejectQueueDepth(DefaultRejectQueueDepth)
{
}

RemoteMonitorNamedList::RemoteMonitorNamedList(const SVString& PPQName, const SVString& name, const MonitoredObjectList& productValuesList, const MonitoredObjectList& productImageList, const MonitoredObjectList& rejectConditionList, const MonitoredObjectList& failStatusList, int rejectDepth)
: m_PPQName(PPQName)
, m_name(name)
, m_productValuesList(productValuesList)
, m_productImagesList(productImageList)
, m_rejectConditionList(rejectConditionList)
, m_failStatusList(failStatusList)
, m_rejectQueueDepth(rejectDepth)
{
}

const SVString& RemoteMonitorNamedList::GetPPQName() const
{
	return m_PPQName;
}

void RemoteMonitorNamedList::SetPPQName(const SVString& PPQName)
{
	m_PPQName = PPQName;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorNamedList.cpp_v  $
 * 
 *    Rev 1.0   17 Apr 2014 16:26:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
