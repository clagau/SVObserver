//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVfailStatusStreamManager
//* .File Name       : $Workfile:   SVFailStatusStreamManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Feb 2015 10:51:10  $
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include <boost/bind.hpp>
#include "SVFailStatusStreamManager.h"
#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVObjectCommandDataJson.h"
#include "SVPPQObject.h"
#include "RemoteMonitorList.h"
#pragma endregion

#pragma region Constants
static const std::string scSVPPQObjectTag = "SVPPQObject";
static const std::string scStreamNameTag = "StreamName";
static const std::string scStartStreamTag = "StartStream";
static const std::string scCommandTag = "Command";
static const std::string scResponseTag = "Response";
static const std::string scStatusTag = "Status";
#pragma endregion

SVFailStatusStreamManager& SVFailStatusStreamManager::Instance()
{
	static SVFailStatusStreamManager l_Object;
	return l_Object;
}

SVFailStatusStreamManager::~SVFailStatusStreamManager()
{
	Shutdown();
}

// this is called when SVObserver first starts
void SVFailStatusStreamManager::Startup(unsigned short p_PortNumber)
{
	m_SocketServer.ListenForClients(p_PortNumber, boost::bind(&SVFailStatusStreamManager::ProcessJsonCommand, this, _1, _2));
	m_cookie = 0;
	SVObserverNotificationFunctorPtr ptr(SVSharedPtr<SVObserverNotificationFunctor>(this, SVNullDeleter()));
	SVObjectManagerClass::Instance().InsertObserver(ptr, m_cookie);
}

// this is called when SVObserver is closing
void SVFailStatusStreamManager::Shutdown()
{
	RemovePPQObservers();
	m_observers.clear();
	m_SocketServer.DisconnectFromClients();
	m_streams.clear();
	if (m_cookie)
	{
		SVObjectManagerClass::Instance().EraseObserver(m_cookie);
		m_cookie = 0;
	}
}

// call this anytime the remote monitor list changes
HRESULT SVFailStatusStreamManager::ManageFailStatus(const RemoteMonitorList& rList)
{
	HRESULT hr = S_OK;

	for (RemoteMonitorList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const SVString& ppqName = it->second.GetPPQName();
		const SVString& listName = it->second.GetName();
		const MonitoredObjectList& rMonitoredObjectList = it->second.GetFailStatusList();
		// Get Guid for PPQ
		SVGUID ppqID;
		SVObjectManagerClass::Instance().GetObjectByDottedName(ppqName, ppqID);
		if (!ppqID.empty())// && it->second.IsActive())
		{
			FailStatusList failStatus;
			failStatus.name = listName;

			// check if there is a list associated with the PPQ
			FailStatusListCollection::iterator it = m_streams.find(ppqID);
			if (it != m_streams.end())
			{
				// check if this list already exists
				FailStatusLists::iterator listIt = it->second.find(listName);
				if (listIt != it->second.end())
				{
					failStatus.bStarted = listIt->second.bStarted; // keep the current state
					listIt->second.stream.Clear(); // remove the items
				}
				// Build fail status value list
				hr = failStatus.stream.Add(listName, rMonitoredObjectList);
			}
			m_streams[ppqID][listName] = failStatus;
		}
	}
	return hr;
}

// call this before going online for the active monitor list(s)
HRESULT SVFailStatusStreamManager::AttachPPQObservers(const RemoteMonitorList& rList)
{
	HRESULT hr = S_OK;
	
	RemovePPQObservers();

	for (RemoteMonitorList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const SVString& ppqName = it->second.GetPPQName();
		const SVString& listName = it->second.GetName();
		const MonitoredObjectList& rMonitoredObjectList = it->second.GetFailStatusList();
		// Get Guid for PPQ
		SVGUID ppqID;
		SVObjectManagerClass::Instance().GetObjectByDottedName(ppqName, ppqID);
		if (!ppqID.empty())// && it->second.IsActive())
		{
			FailStatusList failStatus;
			failStatus.name = listName;

			// check if there is a list associated with the PPQ
			FailStatusListCollection::iterator it = m_streams.find(ppqID);
			if (it != m_streams.end())
			{
				// check if this list already exists
				FailStatusLists::iterator listIt = it->second.find(listName);
				if (listIt != it->second.end())
				{
					failStatus.bStarted = listIt->second.bStarted; // keep the current state
					listIt->second.stream.Clear(); // remove the items
				}
				// Build fail status value list
				hr = failStatus.stream.Add(listName, rMonitoredObjectList);
			}
			m_streams[ppqID][listName] = failStatus;
			// Keep track of Observers by PPQ and associated cookie
			hr = SVObjectManagerClass::Instance().AttachObserver(scSVPPQObjectTag.c_str(), ppqID, m_cookie);
			if (S_OK == hr)
			{
				m_observers[ppqID] = m_cookie;
			}
		}
	}
	return hr;
}

// call this when going offline after tearing down shared memory and when closing the application
void SVFailStatusStreamManager::RemovePPQObservers()
{
	for (PPQObservers::const_iterator it = m_observers.begin();it != m_observers.end();++it)
	{
		SVObjectManagerClass::Instance().DetachObserver(scSVPPQObjectTag.c_str(), it->first, m_cookie);
	}
	m_observers.clear();
}

void SVFailStatusStreamManager::Clear()
{
	RemovePPQObservers();
	m_streams.clear();
}

HRESULT SVFailStatusStreamManager::ObserverUpdate(const SVProductInfoStruct& rData)
{
	HRESULT hr = S_OK;
	// get PPQ and process value objects into Json and send
	SVObjectClass* pObj = dynamic_cast<SVObjectClass *>(rData.oPPQInfo.pPPQ);
	if (pObj && m_SocketServer.HasClient())
	{
		SVGUID ppqId = pObj->GetUniqueObjectID();
		if (!ppqId.empty())
		{
			FailStatusListCollection::const_iterator it = m_streams.find(ppqId);
			if (it != m_streams.end())
			{
				for (FailStatusLists::const_iterator listIt = it->second.begin(); listIt != it->second.end();++listIt)
				{
					if (listIt->second.bStarted)
					{
						hr = listIt->second.stream.BuildJsonStream(rData, m_SocketServer);
					}
				}
			}
		}
	}
	return hr;
}

// Process Start/Stop Command
HRESULT SVFailStatusStreamManager::ProcessJsonCommand(const std::string& p_rJsonCommand, std::string& p_rJsonResults)
{
	HRESULT l_Status = E_FAIL;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;
	SVString l_StreamName;
	SVString l_CmdName;

	if (l_Reader.parse(p_rJsonCommand, l_JsonValues, false))
	{
		if (l_JsonValues.isObject())
		{
			Json::Value l_JsonCommand = l_JsonValues[scStreamNameTag.c_str()];

			if (l_JsonCommand.isString())
			{
				l_StreamName = l_JsonCommand.asString();
				l_JsonCommand = l_JsonValues[scCommandTag.c_str()];
				if (l_JsonCommand.isString())
				{
					l_CmdName = l_JsonCommand.asString(); 
					bool bStart = l_CmdName == scStartStreamTag.c_str();
					l_Status = ProcessStartStopCommand(l_StreamName.ToString(), bStart);
				}
			}
		}
	}
	//Build Response
	Json::FastWriter l_Writer;
	Json::Value value(Json::objectValue);

	value[scResponseTag.c_str()] = l_CmdName.c_str();
	value[scStreamNameTag.c_str()] = l_StreamName.c_str();
	value[scStatusTag.c_str()] = l_Status;

	p_rJsonResults = l_Writer.write(value).c_str();
	return l_Status;
}

HRESULT SVFailStatusStreamManager::ProcessStartStopCommand(const SVString& name, bool bStart)
{
	HRESULT hr = E_INVALIDARG;
	
	for (FailStatusListCollection::iterator it = m_streams.begin();it != m_streams.end() && S_OK != hr;++it)
	{
		FailStatusLists::iterator listIt = it->second.find(name);
		if (listIt != it->second.end())
		{
			listIt->second.bStarted = bStart;
			hr = S_OK;
		}
	}
	return hr;
}

SVFailStatusStreamManager::SVFailStatusStreamManager()
: m_SocketServer()
, m_cookie(0)
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFailStatusStreamManager.cpp_v  $
 * 
 *    Rev 1.0   18 Feb 2015 10:51:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  975
 * SCR Title:  Add Fail Status Stream (SVO-354)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
