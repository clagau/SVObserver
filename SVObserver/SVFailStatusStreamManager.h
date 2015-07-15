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
#include <string>
#include <map>

#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVObjectLibrary/SVObserverNotificationFunctor.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVJsonCommandServerLibrary/SVJsonCommandServer.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "RemoteMonitorList.h"
#include "SVValueObjectReference.h"
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
	HRESULT ManageFailStatus(const RemoteMonitorList& rList);

	//**********************************************
	/// AttachPPQObservers adds the listeners to the PPQ(s) for the fail status streams
	/// \param rList [in] - List of the items to Stream and associated PPQ(s)
	/// \return HRESULT - S_OK for success
	//**********************************************
	HRESULT AttachPPQObservers(const RemoteMonitorList& rList);
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
	/// ProcessStartStopCommand - processes the Start/Stop Stream comamnd
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
		bool bStarted; // state (started/stopped)
		SVString name; // name of the list
		SVFailStatusStream stream; // stream (Json) builder
		FailStatusList() : bStarted(false) {}
	};
	typedef std::map<SVString, FailStatusList> FailStatusLists;
	// Key is PPQ UniqueObjectID
	typedef std::map<SVGUID, FailStatusLists> FailStatusListCollection;
	typedef std::map<SVGUID, long> PPQObservers; // long is cookie from register observer
	FailStatusListCollection m_streams;
	PPQObservers m_observers;
	#pragma endregion
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFailStatusStreamManager.h_v  $
 * 
 *    Rev 1.0   18 Feb 2015 10:51:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  975
 * SCR Title:  Add Fail Status Stream (SVO-354)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
