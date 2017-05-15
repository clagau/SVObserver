//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVServerSocket
//* .File Name       : $Workfile:   SVServerSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   27 Oct 2014 09:45:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVSocket.h"
#pragma endregion Includes

namespace SvSol
{
	template<typename API>
	class SVServerSocket : public SVSocket<API>
	{
	public:
		SVSocketError::ErrorEnum Listen(unsigned short portNo);
		Socket_t Accept(sockaddr* addr, int* addrlen);
		bool ClientConnecting() const;
	};

	template<>
	inline typename Traits<TcpApi>::Socket_t SVServerSocket<TcpApi>::Accept(sockaddr* addr, int* addrlen)
	{
		return TcpApi::accept(m_socket, addr, addrlen);
	}

	template<typename API>
	inline bool SVServerSocket<API>::ClientConnecting() const
	{
		timeval timeout = {0, 200};
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(m_socket, &readset);
		int max_fd = 1;
		return (select(max_fd + 1, &readset, nullptr, nullptr, &timeout) == 1) ? true : false;
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
			std::basic_string<TCHAR> msg = "accept: ";
			Log(msg + SVSocketError::GetErrorText(error), true);
			m_socket = InvalidSock;
			return *this;
			//return InvalidSock;
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
			error = Bind(_T(" "), portNo); //localIP, portNo);
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
} //namespace SvSol
