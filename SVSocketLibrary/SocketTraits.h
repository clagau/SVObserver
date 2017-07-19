//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SocketTraits
//* .File Name       : $Workfile:   SocketTraits.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Aug 2014 14:12:32  $
//******************************************************************************

#pragma once

namespace SvSol
{
	template<typename SOCK>
	struct Traits;

	template<>
	struct Traits<TcpApi>
	{
		typedef SOCKET Socket_t;
		typedef fd_set FdSet_t;
		static const Socket_t InvalidSock = INVALID_SOCKET;
		static const int sokType = SOCK_STREAM;
		static const int proto = IPPROTO_TCP;
		static const std::basic_string<TCHAR> ApiName() { return _T("tcp"); }
		static void CreateOptions(Socket_t s) {}
		static const char tick = 't';
		static const size_t buffer_size = 64*1024;
		static const bool needsHeader = false;
	};

	template<>
	struct Traits<UdpApi>
	{
		typedef SOCKET Socket_t;
		typedef fd_set FdSet_t;
		static const Socket_t InvalidSock = INVALID_SOCKET;
		static const int sokType = SOCK_DGRAM;
		static const int proto = IPPROTO_UDP;
		static const std::basic_string<TCHAR> ApiName() { return _T("udp"); }
		static void CreateOptions(Socket_t s) {}
		static const char tick = 'u';
		static const size_t buffer_size = 6000;//has to be the same in SVRC client
		static const bool needsHeader = true;
	};
} //namespace SvSol

