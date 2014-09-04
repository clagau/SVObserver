//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocket
//* .File Name       : $Workfile:   SVSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   04 Sep 2014 13:21:34  $
//******************************************************************************

#pragma once

#include <winsock2.h>
#include <string>
#include "TcpApi.h"
#include "UdpApi.h"
#include "SVSocketError.h"
#include "SocketTraits.h"
#include <boost/shared_array.hpp>

typedef boost::shared_array<BYTE> bytes;

extern volatile long g_sn;

namespace Seidenader
{
	namespace Socket
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
			std::string Log(const std::string & msg, bool full = false) const;

			Err SetNonBlocking();
			Err SetBlocking();
			Err SetKeepAliveOption(bool bEnable);
			Err SetLingerOption(bool bEnable, unsigned short nTime);
			Err GetReadTimeout();
			Err SetReadTimeout();
			Err SetOptionReuseAddr();
			Err DisableDelay();
			Err EnableDelay();
			Err GetMaxDgramSize(size_t & sz);
			Err SetBufferSize(int sz);
			Err SetReadTimeout(u_int tout);

			Err Write(const std::string& data, bool hasHeader = false); // Use for non-JSON data.
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

			std::string state() const;

			bool IsValidPayload(size_t) { return true; } // Currently, we say every payload is valid.  May be specialized later.
			Err Bind(const char* hostAddr, unsigned short portNo);
			SVSocketError::ErrorEnum Send( const std::string& data ); // Use Send for JSON (no header).
		};
	}
}

#include "SVSocket.inl"

typedef Seidenader::Socket::SVSocket<TcpApi> SVSocket;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVSocket.h_v  $
 * 
 *    Rev 1.5   04 Sep 2014 13:21:34   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Log method to return string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Aug 2014 14:12:34   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Syncing with svrc code base
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jul 2014 15:45:00   jHanebach
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
 *    Rev 1.1   24 Mar 2014 10:58:16   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Changes for run/reject server.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   06 Aug 2013 11:11:40   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed disconnect problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:15:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
 *    Rev 1.0   19 Dec 2011 13:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
