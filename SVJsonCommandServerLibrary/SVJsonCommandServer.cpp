//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServer
//* .File Name       : $Workfile:   SVJsonCommandServer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Jun 2013 17:15:48  $
//******************************************************************************

#include "stdafx.h"
#include <boost/bind.hpp>
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

HRESULT SVJsonCommandServer::WriteJson(const std::string& data)
{
	HRESULT l_Status = S_OK;

	if( !( m_ServerSocket.Write( data ) ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVJsonCommandServer::OnClientAccepted()
{
	if( !( m_ClientAcceptFunction.empty() ) )
	{
		m_ClientAcceptFunction();
	}
}

void SVJsonCommandServer::OnClientJsonReceived(const std::string& data)
{
	std::string l_Response;

	if( !( m_ProcessCommandFunction.empty() ) )
	{
		m_ProcessCommandFunction( data, l_Response );
	}

	WriteJson( l_Response );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVJsonCommandServerLibrary\SVJsonCommandServer.cpp_v  $
 * 
 *    Rev 1.1   18 Jun 2013 17:15:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 May 2013 07:44:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  839
 * SCR Title:  Add Last Modified Notification to Remote Socket Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add the client accept functionality to the system to allow functioanlity to occur when the client connects to the socket.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:14:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
