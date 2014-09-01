//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorNamedList
//* .File Name       : $Workfile:   RemoteMonitorNamedList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   29 Aug 2014 17:49:04  $
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
	bool operator==(const MonitoredObject& rhs) { return (guid == rhs.guid && isArray == rhs.isArray, wholeArray == rhs.wholeArray, arrayIndex == rhs.arrayIndex); }
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
	int m_rejectQueueDepth;
	MonitoredObjectList m_productValuesList;
	MonitoredObjectList m_productImagesList;
	MonitoredObjectList m_rejectConditionList;
	MonitoredObjectList m_failStatusList;
	bool m_bActive; // do not persist the active state
	SVProductFilterEnum m_filter;
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

	void Activate(bool bActivate);
	bool IsActive() const;

	void SetProductFilter(SVProductFilterEnum filter);
	SVProductFilterEnum GetProductFilter() const;

#pragma endregion PublicMethods
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorNamedList.h_v  $
 * 
 *    Rev 1.3   29 Aug 2014 17:49:04   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2014 09:04:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added MonitoredObject struct.
 * Revised MonitoredObjectList to contain MonitoredObject instead of SVGUID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Apr 2014 10:34:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added IsActive and Activate methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
