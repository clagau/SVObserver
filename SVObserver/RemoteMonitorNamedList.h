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
//Moved to precompiled header: #include <deque>
#include "SVUtilityLibrary/SVGUID.h"

#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#pragma endregion Includes

///struct holds information for one entry in Monitorlist 
struct MonitoredObject
{
	SVGUID guid;
	bool isArray;
	bool wholeArray;
	long arrayIndex;

	MonitoredObject() : isArray(false), wholeArray(false), arrayIndex(-1) {}
	bool operator==(const MonitoredObject& rhs) { return (guid == rhs.guid && isArray == rhs.isArray && wholeArray == rhs.wholeArray && arrayIndex == rhs.arrayIndex); }
};

/// deque of MonitorObjects 
typedef std::deque<MonitoredObject> MonitoredObjectList; 
/// class encapsulate 4 MonitoredObjectList  reject product productImage and failstatus 
/// MonitorobjectList are basicly  lists with GUIDS 
class RemoteMonitorNamedList
{
#pragma region PublicMethods
public:
	RemoteMonitorNamedList();
	RemoteMonitorNamedList(const std::string& PPQName, const std::string& name);
	RemoteMonitorNamedList(const std::string& PPQName, const std::string& name, const MonitoredObjectList& productValuesList, const MonitoredObjectList& productImagesList, const MonitoredObjectList& rejectConditionList, const MonitoredObjectList& failStatusList, int rejectDepth);

	const SVGUID& GetPPQObjectID() const;
	const std::string& GetPPQName() const;
	void SetPPQName(const std::string& PPQName); // is a blank name allowed?

	const std::string& GetName() const;
	void SetName(const std::string& name); // is a blank name allowed?
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

	void SetProductFilter(SvSml::SVProductFilterEnum filter);
	SvSml::SVProductFilterEnum GetProductFilter() const;
	static int GetDefaultRejectQueueDepth();
	static int GetMaxRejectQueueDepth();

#pragma endregion PublicMethods

#pragma region PrivateMethods
private:
	void ResolveGuidForPPQName();
#pragma endregion PrivateMethods

#pragma region PublicData
public:
	const static int MinRejectQueueDepth =1;
#pragma endregion PublicData
#pragma region PrivateData
private:
	std::string m_name; // List Name
	std::string m_PPQName; // PPQ Name
	SVGUID m_PPQObjectID; // PPQ instance Guid
	int m_rejectQueueDepth;
	MonitoredObjectList m_productValuesList;
	MonitoredObjectList m_productImagesList;
	MonitoredObjectList m_rejectConditionList;
	MonitoredObjectList m_failStatusList;
	bool m_bActive;
	SvSml::SVProductFilterEnum m_filter;
#pragma endregion PrivateData


};

