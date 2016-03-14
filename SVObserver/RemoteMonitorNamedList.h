//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorNamedList
//* .File Name       : $Workfile:   RemoteMonitorNamedList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   17 Oct 2014 13:35:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include <deque>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#pragma endregion Includes

struct MonitoredObject
{
	SVGUID guid;
	bool isArray;
	bool wholeArray;
	long arrayIndex;

	MonitoredObject() : isArray(false), wholeArray(false), arrayIndex(-1) {}
	bool operator==(const MonitoredObject& rhs) { return (guid == rhs.guid && isArray == rhs.isArray && wholeArray == rhs.wholeArray && arrayIndex == rhs.arrayIndex); }
};

typedef std::deque<MonitoredObject> MonitoredObjectList;

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
	SVGUID m_PPQObjectID; // PPQ instance Guid
	int m_rejectQueueDepth;
	MonitoredObjectList m_productValuesList;
	MonitoredObjectList m_productImagesList;
	MonitoredObjectList m_rejectConditionList;
	MonitoredObjectList m_failStatusList;
	bool m_bActive; // do not persist the active state
	SVProductFilterEnum m_filter;
#pragma endregion PrivateData

#pragma region PrivateMethods
	void ResolveGuidForPPQName();
#pragma endregion PrivateMethods

#pragma region PublicMethods
public:
	RemoteMonitorNamedList();
	RemoteMonitorNamedList(const SVString& PPQName, const SVString& name);
	RemoteMonitorNamedList(const SVString& PPQName, const SVString& name, const MonitoredObjectList& productValuesList, const MonitoredObjectList& productImagesList, const MonitoredObjectList& rejectConditionList, const MonitoredObjectList& failStatusList, int rejectDepth);
	
	const SVGUID& GetPPQObjectID() const;
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

	void Activate(bool bActivate);
	bool IsActive() const;

	void SetProductFilter(SVProductFilterEnum filter);
	SVProductFilterEnum GetProductFilter() const;

#pragma endregion PublicMethods
};

