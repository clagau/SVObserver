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
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVFailStatusStreamManager.h"
#include "SVRemoteControlConstants.h"
#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectCommandDataJson.h"
#include "SVPPQObject.h"
#include "RemoteMonitorList.h"
#include "SVUtilityLibrary/SVNullDeleter.h"
#pragma endregion

#pragma region Constants
static const std::string scSVPPQObjectTag = "SVPPQObject";
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
	SVObserverNotificationFunctorPtr ptr(std::shared_ptr<SVObserverNotificationFunctor>(this, SVNullDeleter()));
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
HRESULT SVFailStatusStreamManager::ManageFailStatus(const RemoteMonitorListMap& rList)
{
	HRESULT hr = S_OK;

	// Remove items from stream if removed from MonitorList
	for (FailStatusListCollection::iterator it = m_streams.begin();it != m_streams.end();++it)
	{
		FailStatusLists& list = it->second;
		for (FailStatusLists::iterator listIt = list.begin();listIt != list.end();)
		{
			RemoteMonitorListMap::const_iterator rListIt = rList.find(listIt->first);
			if (rListIt == rList.end())
			{
				listIt = list.erase(listIt);
			}
			else
			{
				// check if PPQID changed for the list name...
				const std::string& ppqName = rListIt->second.GetPPQName();
				SVGUID ppqID;
				SVObjectManagerClass::Instance().GetObjectByDottedName(ppqName, ppqID);
				if (ppqID != it->first)
				{
					listIt = list.erase(listIt);
				}
				else
				{
					++listIt;
				}
			}
		}
	}

	for (RemoteMonitorListMap::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const std::string& ppqName = it->second.GetPPQName();
		const std::string& listName = it->second.GetName();
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
					failStatus.m_IsStarted = listIt->second.m_IsStarted; // keep the current state
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
HRESULT SVFailStatusStreamManager::AttachPPQObservers(const RemoteMonitorListMap& rList)
{
	HRESULT hr = S_OK;
	
	RemovePPQObservers();

	for (RemoteMonitorListMap::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const std::string& ppqName = it->second.GetPPQName();
		const std::string& listName = it->second.GetName();
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
					failStatus.m_IsStarted = listIt->second.m_IsStarted; // keep the current state
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
					if (listIt->second.m_IsStarted)
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
HRESULT SVFailStatusStreamManager::ProcessJsonCommand(const std::string& rJsonCommand, std::string& rJsonResults)
{
	HRESULT hr = E_INVALIDARG;

	Json::Reader Reader;
	Json::Value JsonValues;
	std::string StreamName;
	std::string CmdName;

	if (Reader.parse(rJsonCommand, JsonValues, false))
	{
		if (JsonValues.isObject())
		{
			Json::Value JsonCommand = JsonValues[SVRC::stream::streamName];
			if (JsonCommand.isString())
			{
				StreamName = JsonCommand.asString();
				JsonCommand = JsonValues[SVRC::stream::command];
				if (JsonCommand.isString())
				{
					CmdName = JsonCommand.asString(); 
					bool bStart = CmdName == SVRC::stream::startStream;
					hr = ProcessStartStopCommand(StreamName, bStart);
				}
			}
		}
	}
	//Build Response
	Json::FastWriter Writer;
	Json::Value value(Json::objectValue);

	value[SVRC::stream::response] = CmdName.c_str();
	value[SVRC::stream::streamName] = StreamName.c_str();
	value[SVRC::stream::status] = hr;

	rJsonResults = Writer.write(value).c_str();
	return hr;
}

HRESULT SVFailStatusStreamManager::ProcessStartStopCommand(const std::string& rName, bool bStart)
{
	HRESULT hr = E_INVALIDARG;

	for (FailStatusListCollection::iterator it = m_streams.begin();it != m_streams.end() && S_OK != hr;++it)
	{
		FailStatusLists::iterator listIt = it->second.find(rName);
		if (listIt != it->second.end())
		{
			listIt->second.m_IsStarted = bStart;
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

