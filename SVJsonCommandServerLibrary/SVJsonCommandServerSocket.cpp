//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServerSocket
//* .File Name       : $Workfile:   SVJsonCommandServerSocket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 09:26:18  $
//******************************************************************************

#include "stdafx.h"
#include <boost/bind.hpp>
#include "SVJsonCommandServerSocket.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"

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
	if (error == SVSocketError::Success)
	{
		error = m_server.Listen(portNo);
		if (error == SVSocketError::Success)
		{
			bRetVal = true;
			if (!m_thread.IsActive())
			{
				// Start thread for accepts and reads
				m_thread.Create(boost::bind(&SVJsonCommandServerSocket::ThreadProcessHandler, this, _1), "SVJsonCommandServerSocketThread");
				QueueUserAPC(SVJsonCommandServerSocket::OnAPCEvent, m_thread.GetThreadHandle(), NULL);
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

	int rc = select(max_fd + 1, &read_set, NULL, NULL, &tv);
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

			int rc = select(max_fd + 1, &read_set, &write_set, NULL, &tv);
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
					std::string l_Data;

					if( !( m_WriteQueue.empty() ) )
					{
						SVAutoLockAndReleaseTemplate< SVCriticalSection > l_Auto;

						if( l_Auto.Assign( &m_WriteQueueLock ) )
						{
							l_Data = m_WriteQueue.front();

							m_WriteQueue.pop_front();
						}
					}

					if( !( l_Data.empty() ) )
					{
						SVSocketError::ErrorEnum error = m_client.Write( l_Data );

						l_Sleep = ( error != SVSocketError::Success );
						if( l_Sleep )
						{
							CloseClient();
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonCommandServerSocket.cpp_v  $
 * 
 *    Rev 1.3   01 Oct 2013 09:26:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Aug 2013 11:09:26   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed disconnect problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2013 15:11:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 May 2013 08:48:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to create a write queue for the socket and update client socket options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:14:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2013 10:54:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the thread process method to not sleep after a successful read.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
