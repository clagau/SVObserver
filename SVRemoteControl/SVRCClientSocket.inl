//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRCClientSocket
//* .File Name       : $Workfile:     $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:     $
//* .Check In Date   : $Date:     $
//******************************************************************************

#pragma warning( push )
#pragma warning( disable: 4996 )
namespace SvSol
{
	template<typename API>
	inline SVRCClientSocket<API>::SVRCClientSocket()
	:	m_state(Disconnected),
		m_serverIPAddr( "" ),
		m_serverPortNo( 0 ),
		m_WaitHandle( NULL ),
		m_newServerIPAddr( "" ),
		m_newServerPortNo( 0 )
	{
		m_WaitHandle = ::CreateEvent( NULL, TRUE, TRUE, NULL );
	}

	template<typename API>
	inline SVRCClientSocket<API>::~SVRCClientSocket()
	{
		Destroy();

		if( m_WaitHandle != NULL )
		{
			::CloseHandle( m_WaitHandle );

			m_WaitHandle = NULL;
		}
	}

	template<typename API>
	inline void SVRCClientSocket<API>::SetConnectionStatusCallback( ConnectionStatusCallback func )
	{
		m_connectionStatusFunc = func;
	}

	template<typename API>
	inline void SVRCClientSocket<API>::SetDataReceivedCallback( DataReceivedCallback func )
	{
		m_dataReceivedFunc = func;
	}

	template<typename API>
	inline std::string SVRCClientSocket<API>::GetLocalIPAddr() const
	{
		std::string localIPAddr;
		// Get Host name - we will always be on the same machine as the server
		hostent* localHost = gethostbyname("");
		if (localHost != NULL)
		{
			localIPAddr = (char *)inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);
		}
		return localIPAddr;
	}

	template<typename API>
	inline SVSocketError::ErrorEnum SVRCClientSocket<API>::BuildConnection( const _bstr_t& p_rAddr, unsigned short portNo, DWORD p_TimeoutInMilliseconds )
	{
		SVSocketError::ErrorEnum bRetVal = SVSocketError::Success;

		if( m_serverIPAddr != p_rAddr || m_serverPortNo != portNo )
		{
			Disconnect();
		}

		if( !( IsConnected() ) && ( 0 < p_rAddr.length() ) )
		{
			if (!m_socket.IsValidSocket())
			{
				SVSocketError::ErrorEnum error = m_socket.Create();

				if (error != SVSocketError::Success)
				{
					bRetVal = error;
				}
				else
				{
					m_socket.SetNonBlocking();
					m_socket.SetKeepAliveOption(true);
					m_socket.SetKeepAliveValues(1000, 2000);
				}
			}

			if( bRetVal == SVSocketError::Success )
			{
				if (!m_thread.IsActive())
				{
					wchar_t buf[32];
					m_socket.SetNonBlocking();
					m_socket.DisableDelay();
					_bstr_t thName = L"SVRCClientSocketThread";
					thName += _ltow(portNo, buf, 16);
					// Start thread for connect, read, and close handling
					HRESULT hr = m_thread.Create(boost::bind(&SVRCClientSocket<API>::ThreadFunc, this), thName);
					if (hr != S_OK)
					{
						bRetVal = SVSocketError::NoRecovery;
					}
				}
			}

			if( bRetVal == SVSocketError::Success )
			{
				SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

				if( l_AutoLock.Assign( &m_CriticalSection ) )
				{
					m_newServerIPAddr = p_rAddr;
					m_newServerPortNo = portNo;
				}
			}

			if( bRetVal == SVSocketError::Success )
			{
				HRESULT hr = WaitForRequest( p_TimeoutInMilliseconds );
				if (!SUCCEEDED(hr))
				{
					bRetVal = SVSocketError::ConnectionTimeout;
					::OutputDebugStringA("SVRCClientSocket<API>::Connect: Wait timeout.");
				}
			}

		}
		return bRetVal;
	}

	template<typename API>
	inline void SVRCClientSocket<API>::Disconnect()
	{
		Destroy();
	}

	template<typename API>
	inline bool SVRCClientSocket<API>::IsConnected() const
	{
		return (m_state == Connected);
	}

	template<typename API>
	inline void SVRCClientSocket<API>::Destroy()
	{
		m_thread.Destroy();
		while (m_thread.IsActive())
		{
			Sleep(1);
		}
		Close();
		m_serverIPAddr = "";
		m_serverPortNo = 0;
		m_state = Disconnected;

		WaitReset();
	}

	template<typename API>
	inline SVSocketError::ErrorEnum SVRCClientSocket<API>::Write( const std::string& data )
	{

		if( !( data.empty() ) )
		{
			return m_socket.Send(data); // Use Send instead of Write.
		}

		return SVSocketError::NoData;
	}

	template<typename API>
	inline void SVRCClientSocket<API>::OnConnectionStatus()
	{
		if (!m_connectionStatusFunc.empty())
		{
			m_connectionStatusFunc(m_state);
		}
	}

	template<typename API>
	inline void SVRCClientSocket<API>::OnDataReceived( std::istream& is )
	{
		if (!m_dataReceivedFunc.empty())
		{
			m_dataReceivedFunc(is);
		}
	}

	template<typename API>
	inline void SVRCClientSocket<API>::ThreadFunc()
	{
		bool l_Sleep = true;

		if (m_socket.IsValidSocket())
		{
			if( WaitTimeout() == S_OK )
			{
				SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

				if( l_AutoLock.Assign( &m_CriticalSection ) )
				{
					if ( IsConnected() ) 
					{
						NotifyRequestComplete();
					}
					else
					{
						m_serverIPAddr = m_newServerIPAddr;
						m_serverPortNo = m_newServerPortNo;
					}
				}
			}

			// Do connect
			if (!IsConnected()) 
			{
				// How to determine if we need to do this...
				if (m_state == Disconnected)
				{
#ifdef TRACE_SOCKET					
					char msg[200];

					sprintf(msg, "Try to connect : %s port: %i \n", static_cast< const char* >(m_serverIPAddr), m_serverPortNo);
					::OutputDebugStringA(msg);
#endif 
					
					
					
					SVSocketError::ErrorEnum error = m_socket.Connect( static_cast< const char* >( m_serverIPAddr ), m_serverPortNo);
					if (error == SVSocketError::Success || error == SVSocketError::OperationInProgress || error == SVSocketError::WouldBlock)
					{
						m_state = Connecting;
						OnConnectionStatus();
					}
				}
				else if (m_state == Connecting)
				{
					fd_set write_set;
					fd_set error_set;
					struct timeval tv = { 0, 1000 };
			
					FD_ZERO(&write_set);
					FD_ZERO(&error_set);
					FD_SET(m_socket, &write_set);
					FD_SET(m_socket, &error_set);
					int max_fd = 1;

					int rc = select(max_fd + 1, NULL, &write_set, &error_set, &tv);
					if (rc > 0)
					{
						if (FD_ISSET(m_socket, &write_set)) 
						{
							m_state = Connected;
							OnConnectionStatus();
							NotifyRequestComplete();
						}
						else if (FD_ISSET(m_socket, &error_set))
						{
							// should probably get the error code here and see what is wrong with the connect
							int error;
							int errorLen = sizeof(error);
							getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char *)&error, &errorLen);
							SVSocketError::ErrorEnum x = SVSocketError::TranslateError(error);
						
							m_socket.Destroy();
							m_socket.Create();
						
							m_state = Disconnected;
							OnConnectionStatus();
						}
					}
	/*
					else
					{
						m_state = Disconnected;
						OnConnectionStatus();
					}
	*/
				}
			}
			// Do read / close handling
			else 
			{
				struct timeval tv = { 0, 1000 };
				fd_set read_set;
				FD_ZERO(&read_set);
				FD_SET(m_socket, &read_set);
				int max_fd = 1;

				int rc = select(max_fd + 1, &read_set, NULL, NULL, &tv);
				if (rc > 0) 
				{
					// do read / close handling
					if (FD_ISSET(m_socket, &read_set))    
					{        
						//data is available on the socket, read it        
						//a return value of 0 on the socket indicates the socket was closed        
						//for that case close the socket and remove socket from fd_set
						unsigned char buf[ 16 * 1024 ];
						int l_Size = sizeof(buf) - 1;
						int amtRead = recv(m_socket, (char *)buf, l_Size, 0);
						l_Sleep = ( amtRead <= 0 );
						if (l_Sleep)
						{
							// client socket closed...
							m_socket.Destroy();
							m_socket.Create();
							m_state = Disconnected;
							OnConnectionStatus();
						}
						else
						{
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

	template<typename API>
	inline void SVRCClientSocket<API>::ProcessDataRead(const char* p_Buf)
	{
		m_dataBuffer.Add(p_Buf);
	
		// check if complete json
		while (m_dataBuffer.IsComplete())
		{
			if (m_dataBuffer.IsValid())
			{
				OnDataReceived(m_dataBuffer.GetJsonDataStream());
			}
			m_dataBuffer.Clear();
		}
	}

	template<typename API>
	inline void SVRCClientSocket<API>::Close()
	{
		m_socket.Destroy();
	}

	template<typename API>
	inline HRESULT SVRCClientSocket<API>::NotifyRequestComplete() const
	{
		HRESULT l_Status = S_OK;

		if( m_WaitHandle != NULL )
		{
			::SetEvent( m_WaitHandle );
		}
		else
		{
			l_Status = E_FAIL;
		}

		return l_Status;
	}

	template<typename API>
	inline HRESULT SVRCClientSocket<API>::WaitReset() const
	{
		HRESULT l_Status = S_OK;

		if( m_WaitHandle != NULL )
		{
			if( !( ::ResetEvent( m_WaitHandle ) ) )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		return l_Status;
	}

	template<typename API>
	inline HRESULT SVRCClientSocket<API>::WaitTimeout() const
	{
		HRESULT l_Status = S_OK;

		if( m_WaitHandle != NULL )
		{
			if( ::WaitForSingleObject( m_WaitHandle, 0 ) != WAIT_TIMEOUT )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		return l_Status;
	}

	template<typename API>
	inline HRESULT SVRCClientSocket<API>::WaitForRequest( DWORD p_TimeoutInMilliseconds ) const
	{
		HRESULT l_Status = S_OK;

		if( m_WaitHandle != NULL )
		{
			unsigned long l_WaitStatus = ::WaitForSingleObject( m_WaitHandle, p_TimeoutInMilliseconds );

			if( l_WaitStatus == WAIT_TIMEOUT )
			{
				l_Status = RPC_E_TIMEOUT;
			}
			else if( l_WaitStatus != WAIT_OBJECT_0 )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		return l_Status;
	}

	template<typename API>
	SVSocketError::ErrorEnum SVRCClientSocket<API>::SetNonBlocking()
	{
		return m_socket.SetNonBlocking();
	}

	template<typename API>
	SVSocketError::ErrorEnum SVRCClientSocket<API>::SetBlocking()
	{
		return m_socket.SetBlocking();
	}
}

#pragma warning( pop )
