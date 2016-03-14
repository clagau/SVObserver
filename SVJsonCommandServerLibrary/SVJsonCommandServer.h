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

#ifndef SVJSONCOMMANDSERVER_H
#define SVJSONCOMMANDSERVER_H

#include <boost/function.hpp>
#include "SVJsonCommandServerSocket.h"

class SVJsonCommandServer
{
public:
	typedef boost::function<void ( )> SVClientAcceptFunction;
	typedef boost::function<void ( const std::string& p_rCommand, std::string& p_rResponse )> SVProcessCommandFunction;

	SVJsonCommandServer();
	virtual ~SVJsonCommandServer();

	void ListenForClients(unsigned short p_PortNumber, SVProcessCommandFunction p_ProcessCommandFunction, SVClientAcceptFunction p_ClientAcceptFunction = SVClientAcceptFunction() );
	void DisconnectFromClients();

	HRESULT WriteJson(const std::string& data);
	bool HasClient() const;

private:
	void OnClientAccepted();
	void OnClientJsonReceived(const std::string& data);

	SVClientAcceptFunction m_ClientAcceptFunction;
	SVProcessCommandFunction m_ProcessCommandFunction;
	SVJsonCommandServerSocket m_ServerSocket;

};

#endif

