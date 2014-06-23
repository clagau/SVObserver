//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TcpApi
//* .File Name       : $Workfile:   TcpApi.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Jun 2014 15:09:28  $
//******************************************************************************

#pragma once

#include <winsock2.h>
#include <vector>
#include <set>
#include <iostream>
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
		   //static int startup();
		   //static int cleanup();
		   static Socket_t socket(int af, int type, int protocol) { return ::socket(af, type, protocol); }
		   static int bind(Socket_t u, const sockaddr* name, int namelen) { return ::bind(u, name, namelen); }
		   //static int bind(Socket_t u, Socket_t udpsock);
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
		   //static int sendmsg(Socket_t u, const char* buf, int len, int ttl = -1, bool inorder = false)
		   //static int recvmsg(Socket_t u, char* buf, int len);
		   //static int64_t sendfile(Socket_t u, std::fstream& ifs, int64_t& offset, int64_t size, int block = 364000);
		   //static int64_t recvfile(Socket_t u, std::fstream& ofs, int64_t& offset, int64_t size, int block = 7280000);
		   static int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout)
			{ return ::select(nfds, readfds, writefds, exceptfds, timeout); }
		   //static int selectEx(const std::vector<Socket_t>& fds, std::vector<Socket_t>* readfds, std::vector<Socket_t>* writefds, std::vector<Socket_t>* exceptfds, long long msTimeOut);
		   //static int epoll_create() { return ::epoll_create(); }
		   //static int epoll_add_usock(const int eid, const Socket_t u, const int* events = NULL);
		   //static int epoll_add_ssock(const int eid, const Socket_t s, const int* events = NULL);
		   //static int epoll_remove_usock(const int eid, const Socket_t u);
		   //static int epoll_remove_ssock(const int eid, const Socket_t s);
		   //static int epoll_wait(const int eid, std::set<Socket_t>* readfds, std::set<Socket_t>* writefds, long long msTimeOut, std::set<Socket_t>* lrfds = NULL, std::set<Socket_t>* wrfds = NULL);
		   //static int epoll_release(const int eid) { return ::epoll_release(eid); }
		   //static CUDTException& getlasterror();
		   //static int perfmon(Socket_t u, CPerfMon* perf, bool clear = true);
		   //static UDTSTATUS getsockstate(Socket_t u);
		};
	}
}

typedef Seidenader::Socket::TcpApi TcpApi;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\TcpApi.h_v  $
 * 
 *    Rev 1.0   19 Jun 2014 15:09:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial check in.  Merged changes from SVRemoteControl project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Jun 2014 11:33:14   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Added pvcs headers/footers.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
