//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TcpApi
//* .File Name       : $Workfile:   TcpApi.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   27 Oct 2014 09:39:04  $
//******************************************************************************

#pragma once

#include <winsock2.h>
#include <Mstcpip.h>

namespace Seidenader
{
	namespace Socket
	{
		class TcpApi
		{
		public: //API
		   typedef SOCKET Socket_t;
		   typedef SOCKET SysSock_t;
		   typedef int Opt_t;

		   static Socket_t socket(int af, int type, int protocol) { return ::socket(af, type, protocol); }
		   static int bind(Socket_t u, const sockaddr* name, int namelen) { return ::bind(u, name, namelen); }
		   static int listen(Socket_t u, int backlog) { return ::listen(u, backlog); }
		   static Socket_t accept(Socket_t u, sockaddr* addr, int* addrlen) { return ::accept(u, addr, addrlen); }
		   static int connect(Socket_t u, const sockaddr* name, int namelen) { return ::connect(u, name, namelen); }
		   static int close(Socket_t u) { return ::closesocket(u); }
		   static int getpeername(Socket_t u, sockaddr* name, int* namelen) { return ::getpeername(u, name, namelen); }
		   static int getsockname(Socket_t u, sockaddr* name, int* namelen) { return ::getsockname(u, name, namelen); }
		   static int getsockopt(Socket_t u, int level, Opt_t optname, void* optval, int* optlen) 
			{ return ::getsockopt(u, level, optname, reinterpret_cast<char *>(optval), optlen); }
		   static int setsockopt(Socket_t u, int level, Opt_t optname, const void* optval, size_t optlen) 
			{ return ::setsockopt(u, level, optname, reinterpret_cast<const char *>(optval), static_cast< int >( optlen )); }
		   static int send(Socket_t u, const char* buf, int len, int flags) { return ::send(u, buf, len, flags); }
		   static int recv(Socket_t u, char* buf, int len, int flags) { return ::recv(u, buf, len, flags); }
		   static int recvfrom(Socket_t u, char * buf, int len, int flags, sockaddr * from, int * fromlen)
			{ return ::recvfrom(u, buf, len, flags, from, fromlen); }
		   static int sendto(Socket_t u, const char * buf, int len, int flags, const sockaddr * to, int tolen)
			{ return ::sendto(u, buf, len, flags, to, tolen); }
		   static int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout)
			{ return ::select(nfds, readfds, writefds, exceptfds, timeout); }
   		   static int shutdown(Socket_t u, int flags) { return ::shutdown(u, flags); }
		   static int setKeepAliveValues(Socket_t u, int interval, int timeVal)
		   {
				struct tcp_keepalive alive;
				DWORD dwBytesRet = 0;
				alive.onoff = true;					 // keepalive enabled
				alive.keepaliveinterval = interval;  // set Interval between keepalive probes
				alive.keepalivetime = timeVal;       // set Keep alive time

				return WSAIoctl(u, SIO_KEEPALIVE_VALS, &alive, sizeof(alive), nullptr, 0, &dwBytesRet, nullptr, nullptr);
		   }
		};
	}
}

typedef Seidenader::Socket::TcpApi TcpApi;

