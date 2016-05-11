//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventThread
//* .File Name       : $Workfile:   SVEventThread.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

template<typename SVEventThreadSignalHandler>
SVEventThread<SVEventThreadSignalHandler>::SVEventThread()
: m_hShutdown( 0 )
, m_hThreadComplete( 0 )
, m_hThread( 0 )
, m_hSignalEvent( 0 )
, m_ulThreadID( 0 )
{
}

template<typename SVEventThreadSignalHandler>
SVEventThread<SVEventThreadSignalHandler>::~SVEventThread()
{
	Destroy();
}

template<typename SVEventThreadSignalHandler>
DWORD WINAPI SVEventThread<SVEventThreadSignalHandler>::ThreadProc( LPVOID lpParam )
{
	HRESULT Result = S_OK;

	if( lpParam )
	{
		#ifdef DEBUG
			SVString l_Message;
		#endif

		SVEventThread* pThread = static_cast< SVEventThread* >( lpParam );

		::ResetEvent( pThread->m_hThreadComplete );

		bool l_Continue = true;
		bool l_WaitForEvents = true;

		HANDLE l_pHandles[ 2 ] = { pThread->m_hSignalEvent, pThread->m_hShutdown };

		while( l_Continue )
		{
			if( l_WaitForEvents )
			{
				l_Continue = ( ::WaitForMultipleObjects( 2, l_pHandles, false, INFINITE ) == WAIT_OBJECT_0 );
			}

			if( l_Continue )
			{
				l_Continue = ::WaitForSingleObject( pThread->m_hShutdown, 0 ) == WAIT_TIMEOUT;

				if( l_Continue )
				{
					l_WaitForEvents = true;

					pThread->m_threadHandler( l_WaitForEvents );
				}
				#ifdef DEBUG
					else
					{
						l_Message = SvUl_SF::Format( _T( "SVEventThread(%d) - Shutdown Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
						::OutputDebugString( l_Message.c_str() );
					}
				#endif
			}
			#ifdef DEBUG
				else
				{
					l_Message = SvUl_SF::Format( _T( "SVEventThread(%d) - Exit Loop = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
					::OutputDebugString( l_Message.c_str() );
				}
			#endif
		}

		#ifdef DEBUG
			l_Message = SvUl_SF::Format( _T( "SVEventThread(%d) - Exit Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
			::OutputDebugString( l_Message.c_str() );
		#endif

		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		DWORD errorCode = GetLastError();

		Result = SVMSG_THREAD_EXIT_ERROR;

		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( static_cast<DWORD> (Result), SvOi::Tid_UnknowThread, StdMessageParams, SvOi::Err_25022_EventThread, errorCode );
	}

	return Result;
}

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Create(const SVEventThreadSignalHandler& threadHandler, LPCTSTR tag)
{
	HRESULT Result = S_OK;

	m_tag = tag;
	m_threadHandler = threadHandler;

	if (!m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( nullptr, true, false, nullptr );
		if (nullptr == m_hShutdown)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25023_EventThread, errorCode );
		}
	}

	if( !m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( nullptr, true, true, nullptr );
		if( nullptr == m_hThreadComplete )
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25024_EventThread, errorCode );
		}
	}

	if( !m_hSignalEvent )
	{
		m_hSignalEvent = ::CreateEvent( nullptr, false, false, nullptr );
		if( nullptr == m_hThreadComplete )
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25025_EventThread, errorCode );
		}
	}

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( nullptr, 0, SVEventThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );
		
		if (nullptr == m_hThread)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25026_EventThread, errorCode );
		}
		else
		{
			SVThreadManager::Instance().Add(m_hThread, tag);

			if( S_OK == Result )
			{
				unsigned long l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );

				for( size_t i = 0; l_WaitStatus == WAIT_OBJECT_0 && i < 1000; ++i )
				{
					::Sleep( 1 );

					l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );
				}

				if( l_WaitStatus != WAIT_TIMEOUT )
				{
					Result = SVMSG_THREAD_CREATION_ERROR;

					DWORD errorCode = GetLastError();

					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25027_EventThread, errorCode );
				}
			}
		}
	}

	return Result;
}

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Restart()
{
	Destroy();

	return Create( m_threadHandler, m_tag.c_str() );
}

template< typename SVEventThreadSignalHandler >
void SVEventThread< SVEventThreadSignalHandler >::Destroy()
{
	if( m_hThread )
	{
		if( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT )
		{
			::SetEvent( m_hShutdown );

			if( ::WaitForSingleObject( m_hThreadComplete, 5000 ) != WAIT_OBJECT_0 )
			{
				DWORD errorCode = GetLastError();

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_THREAD_EXIT_ERROR, m_tag.c_str(), StdMessageParams, SvOi::Err_25028_EventThread, errorCode );

				::TerminateThread( m_hThread, E_FAIL );

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( ::WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT )
			{
				DWORD errorCode = GetLastError();

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_THREAD_EXIT_ERROR, m_tag.c_str(), StdMessageParams, SvOi::Err_25029_EventThread, errorCode );

				::TerminateThread( m_hThread, E_FAIL );
			}
		}
		SVThreadManager::Instance().Remove(m_hThread);
		::CloseHandle( m_hThread );
		m_hThread = nullptr;
	}

	if( m_hSignalEvent )
	{
		::CloseHandle( m_hSignalEvent );
		m_hSignalEvent = nullptr;
	}

	if( m_hThreadComplete )
	{
		::CloseHandle( m_hThreadComplete );
		m_hThreadComplete = nullptr;
	}

	if( m_hShutdown )
	{
		::CloseHandle( m_hShutdown );
		m_hShutdown = nullptr;
	}

	m_ulThreadID = 0;
}

template<typename SVEventThreadSignalHandler>
HANDLE SVEventThread<SVEventThreadSignalHandler>::GetThreadHandle() const
{
	return m_hThread;
}

template<typename SVEventThreadSignalHandler>
int SVEventThread<SVEventThreadSignalHandler>::GetPriority() const
{
	return ::GetThreadPriority(m_hThread);
}

template<typename SVEventThreadSignalHandler>
void SVEventThread<SVEventThreadSignalHandler>::SetPriority(int priority)
{
	::SetThreadPriority(m_hThread, priority);
}

template<typename SVEventThreadSignalHandler>
bool SVEventThread<SVEventThreadSignalHandler>::IsActive() const
{
	bool bRetVal = nullptr != m_hThread;

	if( bRetVal )
	{
		DWORD exitCode = STILL_ACTIVE;

		bRetVal = bRetVal && ( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT );
		bRetVal = bRetVal && ( ::GetExitCodeThread(m_hThread, &exitCode) != FALSE );
		bRetVal = bRetVal && ( exitCode == STILL_ACTIVE );
	}

	return bRetVal;
}

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Signal()
{
	HRESULT l_Status = S_OK;

	if( ! IsActive() )
	{
		l_Status = Restart();
	}

	if( S_OK == l_Status )
	{
		if( ! ::SetEvent( m_hSignalEvent ) )
		{
			l_Status = Restart();

			if( S_OK == l_Status )
			{
				if( ! ::SetEvent( m_hSignalEvent ) )
				{
					l_Status = E_FAIL;
				}
			}
		}
	}

	return l_Status;
}

