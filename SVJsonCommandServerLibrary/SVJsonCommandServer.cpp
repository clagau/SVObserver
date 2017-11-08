//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServer
//* .File Name       : $Workfile:   SVJsonCommandServer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   18 Feb 2015 11:00:08  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVJsonCommandServer.h"

SVJsonCommandServer::SVJsonCommandServer()
: m_ClientAcceptFunction()
, m_ProcessCommandFunction()
, m_ServerSocket()
{
}

SVJsonCommandServer::~SVJsonCommandServer()
{
	DisconnectFromClients();
}

void SVJsonCommandServer::ListenForClients(unsigned short p_PortNumber, SVProcessCommandFunction p_ProcessCommandFunction, SVClientAcceptFunction p_ClientAcceptFunction )
{
	if( m_ServerSocket.HasClient() || !( m_ProcessCommandFunction.empty() ) || !( m_ClientAcceptFunction.empty() ) )
	{
		DisconnectFromClients();
	}

	if( !( p_ProcessCommandFunction.empty() ) )
	{
		m_ClientAcceptFunction = p_ClientAcceptFunction;
		m_ProcessCommandFunction = p_ProcessCommandFunction;

		m_ServerSocket.SetConnectionAcceptedCallback(boost::bind(&SVJsonCommandServer::OnClientAccepted, this));
		m_ServerSocket.SetDataReceivedCallback(boost::bind(&SVJsonCommandServer::OnClientJsonReceived, this, _1));

		m_ServerSocket.Start( p_PortNumber );
	}
}

void SVJsonCommandServer::DisconnectFromClients()
{
	m_ServerSocket.Stop();

	m_ClientAcceptFunction.clear();
	m_ProcessCommandFunction.clear();
}

HRESULT SVJsonCommandServer::WriteJson(const std::string& rData)
{
	HRESULT l_Status = S_OK;

	if( rData.empty() )
	{
		l_Status = E_INVALIDARG;
	}
	else
	{
		if( !( m_ServerSocket.Write( rData ) ) )
		{
			l_Status = E_FAIL;
		}
	}
	return l_Status;
}

bool SVJsonCommandServer::HasClient() const
{
	return m_ServerSocket.HasClient();
}

void SVJsonCommandServer::OnClientAccepted()
{
	if( !( m_ClientAcceptFunction.empty() ) )
	{
		m_ClientAcceptFunction();
	}
}

void SVJsonCommandServer::OnClientJsonReceived(const std::string& rData)
{
	std::string Response;

	if( !( m_ProcessCommandFunction.empty() ) )
	{
		m_ProcessCommandFunction( rData, Response );
	}

	WriteJson( Response );
}

