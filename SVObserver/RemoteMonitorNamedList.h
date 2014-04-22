//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorNamedList
//* .File Name       : $Workfile:   RemoteMonitorNamedList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:26:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include <deque>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

typedef std::deque<SVGUID> MonitoredObjectList;

enum RejectQueueDepthConstraints
{
	MinRejectQueueDepth = 0,
	DefaultRejectQueueDepth = 10,
	MaxRejectQueueDepth = 50
};

class RemoteMonitorNamedList
{
#pragma region PrivateData
private:
	SVString m_name; // List Name
	SVString m_PPQName; // PPQ Name
	int m_rejectQueueDepth;
	MonitoredObjectList m_productValuesList;
	MonitoredObjectList m_productImagesList;
	MonitoredObjectList m_rejectConditionList;
	MonitoredObjectList m_failStatusList;
#pragma endregion PrivateData

#pragma region PublicMethods
public:
	RemoteMonitorNamedList();
	RemoteMonitorNamedList(const SVString& PPQName, const SVString& name);
	RemoteMonitorNamedList(const SVString& PPQName, const SVString& name, const MonitoredObjectList& productValuesList, const MonitoredObjectList& productImagesList, const MonitoredObjectList& rejectConditionList, const MonitoredObjectList& failStatusList, int rejectDepth);
	
	const SVString& GetPPQName() const;
	void SetPPQName(const SVString& PPQName); // is a blank name allowed?

	const SVString& GetName() const;
	void SetName(const SVString& name); // is a blank name allowed?
	
	int GetRejectDepthQueue() const;
	void SetRejectDepthQueue(int depth);

	const MonitoredObjectList& GetProductValuesList() const;
	void SetProductValuesList(const MonitoredObjectList& list);

	const MonitoredObjectList& GetProductImagesList() const;
	void SetProductImagesList(const MonitoredObjectList& list);

	const MonitoredObjectList& GetRejectConditionList() const;
	void SetRejectConditionList(const MonitoredObjectList& list);

	const MonitoredObjectList& GetFailStatusList() const;
	void SetFailStatusList(const MonitoredObjectList& list);
#pragma endregion PublicMethods
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorNamedList.h_v  $
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
