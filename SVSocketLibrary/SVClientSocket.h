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

namespace Seidenader { namespace SVSocketLibrary
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

			std::basic_string<TCHAR> ipAddr = hostAddr;
			if ( !boost::xpressive::regex_match(ipAddr, sMatchIP, e) )
			{
				hostent* pHost = gethostbyname(hostAddr);
				if ( nullptr != pHost )
				{
					ipAddr = inet_ntoa(*(struct in_addr *)*pHost->h_addr_list);
					// if loopback get real ip address
					if ( "127.0.0.1" == ipAddr )
					{
						pHost = gethostbyname("");
						if ( nullptr != pHost )
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
				inet_pton(AF_INET, ipAddr.c_str(), &m_peer.sin_addr.s_addr);
				m_peer.sin_port = htons(portNo);

				if ( API::connect( m_socket, reinterpret_cast< SOCKADDR* >( &m_peer ), sizeof(sockaddr_in) ) == SOCKET_ERROR )
				{
					// could return OperationInProgress for non blocking socket (Windows returns WouldBlock)
					error = SVSocketError::GetLastSocketError();
				}
				else
				{
					DisableDelay();
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
} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/

namespace SvSol = Seidenader::SVSocketLibrary;

#pragma warning (pop)

