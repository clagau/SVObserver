//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClientSocket
//* .File Name       : $Workfile:   SVClientSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 Aug 2014 14:12:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVSocket.h"
#pragma warning (push)
#pragma warning (disable: 4996)

#include <boost\xpressive\xpressive.hpp>
#pragma endregion Includes

#pragma region Declarations
typedef boost::xpressive::sregex regrex;
typedef boost::xpressive::smatch match;
#pragma endregion Declarations

namespace Seidenader
{
	namespace Socket
	{
		template<typename API>
		class SVClientSocket : public SVSocket<API>
		{
		public:
			SVSocketError::ErrorEnum Connect(const char* ipAddr, unsigned short portNo);
			void Disconnect();
			bool IsConnected() const { return m_isConnected; }
		};

		template<typename API>
		inline SVSocketError::ErrorEnum SVClientSocket<API>::Connect(const char* hostAddr, unsigned short portNo)
		{
			SVSocketError::ErrorEnum error = SVSocketError::Success;
			if (!IsValidSocket())
			{
				Create();
			}

			bool validSocket = IsValidSocket();
			if ( validSocket )
			{
				// Check HostAddr for Alpha and get canonical address
				regrex e = boost::xpressive::sregex::compile("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
				match sMatchIP;

				std::string ipAddr = hostAddr;
				if ( !boost::xpressive::regex_match(ipAddr, sMatchIP, e) )
				{
					hostent* pHost = gethostbyname(hostAddr);
					if ( pHost != nullptr )
					{
						ipAddr = inet_ntoa(*(struct in_addr *)*pHost->h_addr_list);
						// if loopback get real ip address
						if ( "127.0.0.1" == ipAddr )
						{
							pHost = gethostbyname("");
							if ( pHost != nullptr )
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
				if ( SVSocketError::Success == error )
				{
					m_peer.sin_family = AF_INET;
					m_peer.sin_addr.s_addr = inet_addr(ipAddr.c_str());
					m_peer.sin_port = htons(portNo);

					if ( API::connect( m_socket, reinterpret_cast< SOCKADDR* >( &m_peer ), sizeof(sockaddr_in) ) == SOCKET_ERROR )
					{
						// could return OperationInProgress for non blocking socket (Windows returns WouldBlock)
						error = SVSocketError::GetLastSocketError();
					}
					else
					{
						m_isConnected = true;
					}
				}
			}
			else
			{
				error = SVSocketError::NotASocket;
			}
			return error;
		}

		template<typename API>
		inline void SVClientSocket<API>::Disconnect()
		{
			Destroy();
			m_isConnected = false;
		}
	}
}

#pragma warning (pop)

typedef Seidenader::Socket::SVClientSocket<TcpApi> SVClientSocket;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVClientSocket.h_v  $
 * 
 *    Rev 1.2   15 Aug 2014 14:12:34   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Syncing with svrc code base
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jul 2014 15:44:58   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Synchronizing with svn
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   05 Jun 2014 11:40:12   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Code clean-up, minor bug fixes.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   24 Mar 2014 10:58:14   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Changes for run/reject server.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   25 Apr 2013 17:14:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
