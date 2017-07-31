//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFailStatusStreamManager
//* .File Name       : $Workfile:   SVFailStatusStreamManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Feb 2015 10:51:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <map>

#include "SVObjectLibrary/SVObserverNotificationFunctor.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "RemoteMonitorList.h"
#include "SVFailStatusStream.h"
#include "SVInfoStructs.h"
#pragma endregion

class SVFailStatusStreamManager : public SVObserverTemplate<SVProductInfoStruct>, public SVObserverNotificationFunctor
{
public:
	#pragma region Public Methods
	static SVFailStatusStreamManager& Instance();
	virtual ~SVFailStatusStreamManager();

	//**********************************************
	/// Startup initializes and opens the socket for listening
	/// \param portNumber [in] - Port Number to listen on
	//**********************************************
	void Startup(unsigned short portNumber);
	void Shutdown();

	//**********************************************
	/// ManageFailStatus adds the fail status streams for the PPQ(s)
	/// \param rList [in] - List of the items to Stream and associated PPQ(s)
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT ManageFailStatus(const RemoteMonitorListMap& rList);

	//**********************************************
	/// AttachPPQObservers adds the listeners to the PPQ(s) for the fail status streams
	/// \param rList [in] - List of the items to Stream and associated PPQ(s)
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT AttachPPQObservers(const RemoteMonitorListMap& rList);
	void RemovePPQObservers();

	void Clear();

	//**********************************************
	/// ObserverUpdate is the notification callback from the PPQ(s) on Product Conmplete
	/// \param rData [in] - The current ProductInfo struct
	/// \return HRESULT - S_OK for success
	//**********************************************
	virtual HRESULT ObserverUpdate(const SVProductInfoStruct& rData) override;
	#pragma endregion

private:
	#pragma region Constructor
	SVFailStatusStreamManager();
	#pragma endregion

	#pragma region Private Methods
	//**********************************************
	/// EnableStream enables/disables the Fail Status Stream
	/// \param rName [in] - name of the Fail Status Stream to enable/disable
	/// \param bEnable [in] - boolean enable/disable
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT EnableStream(const std::string& rName, bool bEnable);

	//**********************************************
	/// ProcessJsonCommand - processes the received JSON command
	/// \param rJsonCommand [in] - JSON command received
	/// \param rJsonResults [out] - response to the JSON command
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT ProcessJsonCommand(const std::string& rJsonCommand, std::string& rJsonResults);

	//**********************************************
	/// ProcessStartStopCommand - processes the Start/Stop Stream command
	/// \param name [in] - name of Fail Status Stream to start/stop
	/// \param bStart [in] - boolean start/stop
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT ProcessStartStopCommand(const SVString& name, bool bStart);
	#pragma endregion

	#pragma region Private Members
	SVJsonCommandServer m_SocketServer;
	long m_cookie; // for the observer pattern

	struct FailStatusList
	{
		bool m_IsStarted; // state (started/stopped)
		SVString name; // name of the list
		SVFailStatusStream stream; // stream (Json) builder
		FailStatusList() : m_IsStarted(false) {}
	};
	typedef std::map<SVString, FailStatusList> FailStatusLists;
	// Key is PPQ UniqueObjectID
	typedef std::map<SVGUID, FailStatusLists> FailStatusListCollection;
	typedef std::map<SVGUID, long> PPQObservers; // long is cookie from register observer
	FailStatusListCollection m_streams;
	PPQObservers m_observers;
	#pragma endregion
};

