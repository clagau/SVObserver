//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClientSocket
//* .File Name       : $Workfile:   SVClientSocket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:14:08  $
//******************************************************************************
#include "stdafx.h"
#include "SVClientSocket.h"

// Create must be called prior to calling connect
SVSocketError::ErrorEnum SVClientSocket::Connect(const char* hostAddr, unsigned short portNo)
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	if (IsValidSocket())
	{
		// Check HostAddr for Alpha and get canonical address
		std::string ipAddr = hostAddr;
		if (isalpha(hostAddr[0]))
		{
			hostent* pHost = gethostbyname(hostAddr);
			if (pHost != NULL)
			{
				ipAddr = inet_ntoa(*(struct in_addr *)*pHost->h_addr_list);
				// if loopback get real ip address
				if (ipAddr == "127.0.0.1") 
				{
					pHost = gethostbyname("");
					if (pHost != NULL)
					{
						ipAddr = inet_ntoa(*(struct in_addr *)*pHost->h_addr_list);
					}
					else // Host name not resolved
					{
						error = SVSocketError::HostNotFound;
					}
				}
			}
			else // Host name not resolved
			{
				error = SVSocketError::HostNotFound;
			}
		}
		if (error == SVSocketError::Success)
		{
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
			addr.sin_port = htons(portNo);
			
			if (connect(m_socket, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				// could return OperationInProgress for non blocking socket (Windows returns WouldBlock)
				error = SVSocketError::GetLastSocketError();
			}
		}
	}
	else
	{
		error = SVSocketError::NotASocket;
	}
	return error;
}

void SVClientSocket::Disconnect()
{
	Destroy();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSocketLibrary\SVClientSocket.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:14:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 14:20:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with partial packets and transfer issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:27:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
