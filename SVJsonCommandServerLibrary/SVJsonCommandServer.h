//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServer
//* .File Name       : $Workfile:   SVJsonCommandServer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Jun 2013 17:16:06  $
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

private:
	void OnClientAccepted();
	void OnClientJsonReceived(const std::string& data);

	SVClientAcceptFunction m_ClientAcceptFunction;
	SVProcessCommandFunction m_ProcessCommandFunction;
	SVJsonCommandServerSocket m_ServerSocket;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVJsonCommandServerLibrary\SVJsonCommandServer.h_v  $
 * 
 *    Rev 1.1   18 Jun 2013 17:16:06   bwalter
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
 *    Rev 1.0   22 Apr 2013 13:14:44   bWalter
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
