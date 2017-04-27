//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocket
//* .File Name       : $Workfile:   SVSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   27 Oct 2014 09:43:22  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <cstdio>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <winsock2.h>
//Moved to precompiled header: #include <ws2tcpip.h>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
//Moved to precompiled header: #include <boost/shared_array.hpp>
#include "TcpApi.h"
#include "UdpApi.h"
#include "SVSocketError.h"
#include "SocketTraits.h"
#pragma endregion Includes


typedef boost::shared_array<BYTE> bytes;

extern volatile long g_sn;

namespace Seidenader { namespace SVSocketLibrary
{
	typedef SVSocketError::ErrorEnum Err;

	template<typename API>
	class SVSocket
	{
	public:
		typedef typename Traits<API>::Socket_t Socket_t;
		typedef typename Traits<API>::FdSet_t FdSet_t;
		static const Socket_t InvalidSock = Traits<API>::InvalidSock; // m_ is not used here because this is a constant.
		size_t m_buff_sz;
	protected:
		Socket_t m_socket;
		sockaddr_in m_addr; // this socket address
		sockaddr_in m_peer; // peer address (if connected)
		bool m_isConnected;
		bool m_hasOwner;
		int m_sockNum;
		char * dupWithHeader(const unsigned char * src, std::vector<char> & vec, size_t len) const;
	public:
		SVSocket(Socket_t sok = InvalidSock)
			: m_socket(sok), m_hasOwner(false), m_isConnected(false), m_buff_sz(Traits<API>::buffer_size)
		{
			m_sockNum = InterlockedIncrement(&g_sn);
			int namelen = sizeof(sockaddr_in);
			if (m_socket != InvalidSock && 
				API::getsockname(m_socket, reinterpret_cast< sockaddr* >( &m_addr ), &namelen) != SVSocketError::Success)
			{
				m_socket = InvalidSock;
			}
			memset(&m_peer, 0, sizeof(sockaddr_in));
		}
		SVSocket(bool isConnected, bool bHasOwner, Socket_t sok)
			: m_socket(sok), m_hasOwner(bHasOwner), m_isConnected(isConnected), m_buff_sz(Traits<API>::buffer_size)
		{
			m_sockNum = InterlockedIncrement(&g_sn);
			int namelen = sizeof(sockaddr_in);
			if (m_socket != InvalidSock && 
				API::getsockname(m_socket, reinterpret_cast< sockaddr* >( &m_addr ), &namelen) != SVSocketError::Success)
			{
				m_socket = InvalidSock;
			}
			memset(&m_peer, 0, sizeof(sockaddr_in));
		}

		virtual ~SVSocket()
		{
			if ( m_hasOwner )
			{
				Destroy();
			}
		}

		virtual Err Create();
		virtual void Destroy();

		ULONG GetAddr() const;
		void  Log(const std::basic_string<TCHAR> & msg, bool full = false) const;

		Err SetNonBlocking();
		Err SetBlocking();
		Err SetKeepAliveOption(bool bEnable);
		Err SetKeepAliveValues(int interval, int timeVal);
		Err SetLingerOption(bool bEnable, unsigned short nTime);
		Err GetReadTimeout();
		Err SetReadTimeout();
		Err SetOptionReuseAddr();
		Err DisableDelay();
		Err EnableDelay();
		Err GetMaxDgramSize(size_t & sz);
		Err SetBufferSize(int sz);
		Err SetReadTimeout(u_int tout);

		Err Write(const std::basic_string<TCHAR>& data, bool hasHeader = false); // Use for non-JSON data.
		Err Write(const unsigned char* buffer, size_t len, bool hasHeader = false); // Use for non-JSON data.
		Err Read(unsigned char* buffer, size_t len, size_t& amtRead, bool hasHeader = false);
		Err Select(int & nfds, FdSet_t * readfds, FdSet_t * writefds, FdSet_t * exceptfds, const timeval *timeout);
		Err ReadFrom(unsigned char * buffer, size_t len, size_t & amtRead, sockaddr_in & from, size_t & fromLen, bool hasHeader = false);
		Err PeekFrom(unsigned char * buffer, size_t len, size_t & amtRead, sockaddr_in & from, size_t & fromLen, bool hasHeader = false);
		Err WriteTo(const unsigned char * buffer, size_t len, const sockaddr_in & to, size_t toLen, bool hasHeader = false);
		Err ReadAll(bytes & dest, u_long & sz, bool hasHeader = false);

		operator Socket_t() const;
		void Attach(Socket_t socket, const sockaddr_in& addr);
		Socket_t Release();
		bool IsValidSocket() const
		{
			return (m_socket != InvalidSock);
		}
		bool IsConnected() const { return m_isConnected; }

		std::basic_string<TCHAR> state() const;

		bool IsValidPayload(size_t) { return true; } // Currently, we say every payload is valid.  May be specialized later.
		Err Bind(const TCHAR* hostAddr, unsigned short portNo);
		SVSocketError::ErrorEnum Send( const std::basic_string<TCHAR>& data ); // Use Send for JSON (no header).

		bool DataAvailable() const
		{
			bool bDataAvail = false;
			timeval timeout = {0, 20};
			fd_set readset;
			FD_ZERO(&readset);
			FD_SET(m_socket, &readset);
			int max_fd = 1;
			int rc = select(max_fd + 1, &readset, nullptr, nullptr, &timeout);
			if (rc > 0)
			{
				if (FD_ISSET(m_socket, &readset))
				{
					bDataAvail = true;
				}
			}
			return bDataAvail;
		}
	};
} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/

namespace SvSol = Seidenader::SVSocketLibrary;

#include "SVSocket.inl"


