//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVServerSocket
//* .File Name       : $Workfile:   SVServerSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Jun 2014 15:48:04  $
//******************************************************************************

#pragma once

#include "SVSocket.h"

namespace Seidenader
{
	namespace Socket
	{
		template<typename API>
		class SVServerSocket : public SVSocket<API>
		{
		public:
			SVSocketError::ErrorEnum Listen(unsigned short portNo);
			Socket_t Accept(sockaddr* addr, int* addrlen);
		};

		template<>
		inline typename Traits<TcpApi>::Socket_t SVServerSocket<TcpApi>::Accept(sockaddr* addr, int* addrlen)
		{
			return TcpApi::accept(m_socket, addr, addrlen);
		}

		template<>
		inline typename Traits<UdpApi>::Socket_t SVServerSocket<UdpApi>::Accept(sockaddr* addr, int* addrlen)
		{
			// TODO: negotiate buffer size. For now assume 48K.
			size_t bufsz = 1024*48;
			unsigned char buf[1024*48];
			sockaddr_in * addr_in = reinterpret_cast<sockaddr_in *>(addr);
			if (!IsValidSocket())
			{
				Create(); // recreate
				Listen(addr_in->sin_port); // rebind. Kludge!
			}
			size_t len = *addrlen;
			Err error;
			if (SVSocketError::Success != (error = PeekFrom(buf, bufsz, bufsz, *addr_in, len)))
			{
				std::string msg = "accept: ";
				Log(msg + SVSocketError::GetErrorText(error), true);
				return InvalidSock;
			}
			m_peer = *addr_in;
			return *this; // Should create a new socket?
		}

		template<typename API>
		inline SVSocketError::ErrorEnum SVServerSocket<API>::Listen(unsigned short portNo)
		{
			SVSocketError::ErrorEnum error = SVSocketError::Success;
			if (IsValidSocket())
			{
				// bind to INADDR_ANY
				error = Bind(" ", portNo); //localIP, portNo);
				if (error == SVSocketError::Success)
				{
					if (API::listen(m_socket, 1) == SOCKET_ERROR)
					{
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
	}
}

typedef Seidenader::Socket::SVServerSocket<TcpApi> SVServerSocket;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVServerSocket.h_v  $
 * 
 *    Rev 1.1   19 Jun 2014 15:48:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged changes from SVRemoteControl project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:15:20   bWalter
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
