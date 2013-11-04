//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocket
//* .File Name       : $Workfile:   SVSocket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 Aug 2013 11:11:40  $
//******************************************************************************
#include "Stdafx.h"
#include "SVSocket.h"
//#include "SVSocketBuffer.h"

SVSocket::SVSocket()
: m_socket(INVALID_SOCKET)
{
	memset(&m_addr, 0, sizeof(m_addr));
}

SVSocket::~SVSocket()
{
	Destroy();
}

bool SVSocket::IsValidSocket() const
{
	return (m_socket == INVALID_SOCKET) ? false : true;
}

SVSocketError::ErrorEnum SVSocket::Create()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;

	if (m_socket == INVALID_SOCKET)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET)
		{
			error = SVSocketError::GetLastSocketError();
		}
		else
		{
			error = SetNonBlocking();
			if (error == SVSocketError::Success)
			{
				error = DisableDelay();
			}
		}
	}
	else
	{
		// already created
		error = SVSocketError::AlreadyCreated;
	}
	return error;
}

void SVSocket::Destroy()
{
	if (m_socket != INVALID_SOCKET)
	{
		int rc = closesocket(m_socket);
		if (rc == SOCKET_ERROR)
		{
			SVSocketError::ErrorEnum error = SVSocketError::GetLastSocketError();
		}
		m_socket = INVALID_SOCKET;
	}
}

SVSocket::operator SOCKET() const
{
	return m_socket;
}

void SVSocket::Attach(SOCKET socket, const sockaddr_in& addr)
{
	if (socket != m_socket)
	{
		Destroy();
		m_socket = socket;
		m_addr = addr;
	}
}

SOCKET SVSocket::Release()
{
	SOCKET socket = m_socket;
	m_socket = INVALID_SOCKET;
	return socket;
}

SVSocketError::ErrorEnum SVSocket::Bind(const char* hostAddr, unsigned short portNo)
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	if (IsValidSocket())
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(hostAddr);
		addr.sin_port = htons(portNo);
		if (bind(m_socket, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			error = SVSocketError::GetLastSocketError();
		}
	}
	else
	{
		error = SVSocketError::NotASocket;
	}
	return error;
}

ULONG SVSocket::GetAddr() const
{
	return m_addr.sin_addr.S_un.S_addr;
}

SVSocketError::ErrorEnum SVSocket::SetNonBlocking()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	int nCurFlags = 1;
    if (ioctlsocket(m_socket, FIONBIO, (ULONG *)&nCurFlags) == SOCKET_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

SVSocketError::ErrorEnum SVSocket::SetBlocking()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	int nCurFlags = 0;
    if (ioctlsocket(m_socket, FIONBIO, (ULONG *)&nCurFlags) == SOCKET_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

SVSocketError::ErrorEnum SVSocket::SetKeepAliveOption(bool bEnable)
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	int iOptVal;
	int iOptLen = sizeof(int);

	if (getsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&iOptVal, &iOptLen) != SOCKET_ERROR) 
	{
		if (setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&bOptVal, bOptLen) == SOCKET_ERROR) 
		{
			error = SVSocketError::GetLastSocketError();
		}
	}
	else
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

SVSocketError::ErrorEnum SVSocket::SetLingerOption(bool bEnable, unsigned short nTime)
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;

	LINGER linger;
    linger.l_onoff = (bEnable == true) ? 1 : 0;
    linger.l_linger = nTime;

    if (setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char *)&linger, sizeof(linger)) == SOCKET_ERROR)
    {
		error = SVSocketError::GetLastSocketError();
	}
    return error;
}

SVSocketError::ErrorEnum SVSocket::SetOptionReuseAddr()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
    int nReuse  = TRUE;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&nReuse, sizeof(int)) == SOCKET_ERROR)
    {
		error = SVSocketError::GetLastSocketError();
    }
    return error;
}

SVSocketError::ErrorEnum SVSocket::DisableDelay()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
    DWORD enabled = FALSE;
    if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&enabled, sizeof(enabled)) == SOCKET_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
    }
    return error;
}

SVSocketError::ErrorEnum SVSocket::EnableDelay()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
    DWORD enabled = TRUE;
    if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&enabled, sizeof(enabled)) == SOCKET_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
    }
    return error;
}

SVSocketError::ErrorEnum SVSocket::Write(const std::string& data)
{
	return Write((const unsigned char *)data.c_str(), data.length());
}

SVSocketError::ErrorEnum SVSocket::Write(const unsigned char* buffer, size_t len)
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;

	if (len > 0)
	{
		int l_Error = send(m_socket, (const char *)buffer, (int)len, 0);
		if ( l_Error == SOCKET_ERROR)
		{
			error = SVSocketError::GetLastSocketError();
		}
	}
	return error;
}

SVSocketError::ErrorEnum SVSocket::Read(unsigned char* buffer, size_t len, size_t& amtRead)
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	amtRead = 0;
	// get available data
	int amt = recv(m_socket, (char *)buffer, (int)len, 0);
	if (amt< 0)
	{
		error = SVSocketError::GetLastSocketError();
	}
	else
	{
		amtRead = amt;
	}
	return error;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVSocket.cpp_v  $
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
 *    Rev 1.0   25 Apr 2013 17:15:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 14:20:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with partial packets and transfer issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:27:16   jspila
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
