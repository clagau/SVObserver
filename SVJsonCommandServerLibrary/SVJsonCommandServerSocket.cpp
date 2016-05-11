//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServerSocket
//* .File Name       : $Workfile:   SVJsonCommandServerSocket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   01 Dec 2014 12:22:52  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVJsonCommandServerSocket.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVSystemLibrary/SVThreadManager.h"

SVJsonCommandServerSocket::SVJsonCommandServerSocket()
{
}

SVJsonCommandServerSocket::~SVJsonCommandServerSocket()
{
}

void SVJsonCommandServerSocket::SetConnectionAcceptedCallback(ConnectionAcceptedCallback func)
{
	m_connectionAcceptedFunc = func;
}

void SVJsonCommandServerSocket::SetDataReceivedCallback(DataReceivedCallback func)
{
	m_dataReceivedFunc = func;
}

bool SVJsonCommandServerSocket::Start(unsigned short portNo)
{
	bool bRetVal = false;
	SVSocketError::ErrorEnum error = m_server.Create();
	if (SVSocketError::Success == error)
	{
		error = m_server.SetNonBlocking();
		if (SVSocketError::Success == error)
		{
			error = m_server.Listen(portNo);
			if (SVSocketError::Success == error)
			{
				bRetVal = true;
				if (!m_thread.IsActive())
				{
					// Start thread for accepts and reads
					m_thread.Create(boost::bind(&SVJsonCommandServerSocket::ThreadProcessHandler, this, _1), "SVJsonCommandServerSocketThread", SVNone);
					QueueUserAPC(SVJsonCommandServerSocket::OnAPCEvent, m_thread.GetThreadHandle(), reinterpret_cast<ULONG_PTR>(nullptr));
				}
			}
		}
	}
	return bRetVal;
}

void SVJsonCommandServerSocket::Stop()
{
	m_thread.Destroy();
	while (m_thread.IsActive())
	{
		Sleep(1);
	}
	m_server.Destroy();

	CloseClient();
}

bool SVJsonCommandServerSocket::CanAccept() const
{
	return (m_client.IsValidSocket()) ? false : true;
}

bool SVJsonCommandServerSocket::HasClient() const
{
	return m_client.IsValidSocket();
}

bool SVJsonCommandServerSocket::Write(const std::string& data)
{
	bool bRetVal = false;

	if (HasClient())
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_Auto;

		bRetVal = ( l_Auto.Assign( &m_WriteQueueLock ) != FALSE );

		if( bRetVal )
		{
			m_WriteQueue.push_back( data );
		}
		else
		{
			// @TODO - need to log something
		}
	}

	return bRetVal;
}

void SVJsonCommandServerSocket::OnAccept()
{
	if (!m_connectionAcceptedFunc.empty())
	{
		m_connectionAcceptedFunc();
	}
}

void SVJsonCommandServerSocket::OnDataReceived(const std::string& data)
{
	if (!m_dataReceivedFunc.empty())
	{
		m_dataReceivedFunc(data);
	}
}

void CALLBACK SVJsonCommandServerSocket::OnAPCEvent( ULONG_PTR data )
{
}

bool SVJsonCommandServerSocket::SameClient(const sockaddr_in& addr) const
{
	return (m_client.GetAddr() == addr.sin_addr.S_un.S_addr);
}

void SVJsonCommandServerSocket::CheckConnection()
{
	fd_set read_set;
	struct timeval tv = { 0, 1 };
		
	FD_ZERO(&read_set);
	FD_SET(m_server, &read_set);

	int max_fd = 1;

	int rc = select(max_fd + 1, &read_set, nullptr, nullptr, &tv);
	if (rc > 0) 
	{
		if (FD_ISSET(m_server, &read_set)) 
		{
			sockaddr_in addr;
			int len = sizeof(addr);
			SOCKET socket = accept(m_server, (SOCKADDR *)&addr, &len);
			if (socket != INVALID_SOCKET)
			{
				if (CanAccept() || SameClient(addr))
				{
					// set active client
					m_client.Attach(socket, addr);
					m_client.SetLingerOption( false, 0 );
					m_client.SetKeepAliveOption( true );
					m_client.DisableDelay();
/*
					// debug for now...
					printf("Client Accepted: %s\n", inet_ntoa(addr.sin_addr));
*/
					OnAccept();
				}
				else
				{
					closesocket(socket);
				}
			}
		}
	}
}

void SVJsonCommandServerSocket::ThreadProcessHandler(bool& bWaitEvents)
{
	bool l_Sleep = true;

	bWaitEvents = false;

	if (m_server.IsValidSocket())
	{
		CheckConnection();
		
		if (m_client.IsValidSocket())
		{
			struct timeval tv = { 0, 1000 };
			fd_set read_set;
			fd_set write_set;
			//fd_set error_set;
			FD_ZERO(&read_set);
			FD_SET(m_client, &read_set);
			FD_ZERO(&write_set);
			FD_SET(m_client, &write_set);
			//FD_ZERO(&error_set);
			//FD_SET(m_client, &error_set);
			int max_fd = 1;

			int rc = select(max_fd + 1, &read_set, &write_set, nullptr, &tv);
			if (rc > 0) 
			{
				/* Not used as of yet
				if (FD_ISSET(m_client, &error_set))
				{
					// should probably get the error code here and see what is wrong with the socket
					int error;
					int errorLen = sizeof(error);
					getsockopt(m_client, SOL_SOCKET, SO_ERROR, (char *)&error, &errorLen);
					SVSocketError::ErrorEnum x = SVSocketError::TranslateError(error);
						
					m_client.Destroy();
					m_dataBuffer.Reset();
				}
				*/

				if (FD_ISSET(m_client, &write_set))
				{
					if( !( m_WriteQueue.empty() ) )
					{
						SVAutoLockAndReleaseTemplate< SVCriticalSection > l_Auto;

						if( l_Auto.Assign( &m_WriteQueueLock ) )
						{
							std::string l_Data = m_WriteQueue.front();

							m_WriteQueue.pop_front();

							if( !( l_Data.empty() ) )
							{
								SVSocketError::ErrorEnum error = m_client.Send( l_Data ); // Use Send instead of Write for JSON.

								l_Sleep = ( error != SVSocketError::Success );
								if( l_Sleep )
								{
									// @TODO - need to log something
									CloseClient();
								}
							}
							else
							{
								// @TODO - need to log something
							}
						}
						else
						{
							// @TODO - need to log something
						}
					}
				}

				// do reads / close handling for client (only one client)
				if (FD_ISSET(m_client, &read_set))
				{
					//data is available on the socket, read it
					//a return value of 0 on the socket indicates the socket was closed
					//for that case close the socket and remove socket from fd_set
					unsigned char buf[ 16 * 1024 ];
					size_t l_Size = sizeof(buf) - 1;
					size_t amtRead;
					SVSocketError::ErrorEnum error = m_client.Read(buf, l_Size, amtRead);
					l_Sleep = ( amtRead <= 0 );
					if (l_Sleep)
					{
						/*
						sockaddr_in addr;
						int len = sizeof(addr);
						getpeername(m_client, (SOCKADDR *)&addr, &len);

						printf("Client Closed: %s\n", inet_ntoa(addr.sin_addr));
						*/
						// client socket closed...
						CloseClient();
					}
					else
					{
						// Handle inbound data
						buf[amtRead] = 0;
						ProcessDataRead(reinterpret_cast<const char *>(buf));
					}
				}
			}
		}
	}

	if( l_Sleep )
	{
		Sleep(20);
	}
}

void SVJsonCommandServerSocket::ProcessDataRead(const char* p_pBuf)
{
	m_dataBuffer.Add(p_pBuf);
	
	// check if complete json
	while (m_dataBuffer.IsComplete())
	{
		if (m_dataBuffer.IsValid())
		{
			OnDataReceived(m_dataBuffer.GetJsonData());
		}
		m_dataBuffer.Clear();
	}
}

void SVJsonCommandServerSocket::CloseClient()
{
	m_client.Destroy();
	m_dataBuffer.Reset();

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_Auto;

	if( l_Auto.Assign( &m_WriteQueueLock ) )
	{
		m_WriteQueue.clear();
	}
}

