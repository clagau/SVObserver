//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServer
//* .File Name       : $Workfile:   SVJsonCommandServer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Feb 2015 11:00:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVJsonCommandServerSocket.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVJsonCommandServer
{
public:
	typedef boost::function<void ( )> SVClientAcceptFunction;
	typedef boost::function<void ( const SVString& rCommand, SVString& rResponse )> SVProcessCommandFunction;

	SVJsonCommandServer();
	virtual ~SVJsonCommandServer();

	void ListenForClients(unsigned short p_PortNumber, SVProcessCommandFunction ProcessCommandFunction, SVClientAcceptFunction ClientAcceptFunction = SVClientAcceptFunction() );
	void DisconnectFromClients();

	HRESULT WriteJson(const SVString& rData);
	bool HasClient() const;

private:
	void OnClientAccepted();
	void OnClientJsonReceived(const SVString& rData);

	SVClientAcceptFunction m_ClientAcceptFunction;
	SVProcessCommandFunction m_ProcessCommandFunction;
	SVJsonCommandServerSocket m_ServerSocket;

};

