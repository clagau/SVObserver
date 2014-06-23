//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SocketTraits
//* .File Name       : $Workfile:   SocketTraits.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Jun 2014 15:09:28  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace Socket
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
			static const std::string ApiName() { return "tcp"; }
			static void CreateOptions(Socket_t s) {}
			static const char tick = 't';
		};

		template<>
		struct Traits<UdpApi>
		{
			typedef SOCKET Socket_t;
			typedef fd_set FdSet_t;
			static const Socket_t InvalidSock = INVALID_SOCKET;
			static const int sokType = SOCK_DGRAM;
			static const int proto = IPPROTO_UDP;
			static const std::string ApiName() { return "udp"; }
			static void CreateOptions(Socket_t s) {}
			static const char tick = 'u';
		};
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SocketTraits.h_v  $
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
