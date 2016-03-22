//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThread
//* .File Name       : $Workfile:   SVThread.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

template<typename SVThreadSignalHandler>
SVThread<SVThreadSignalHandler>::SVThread()
: m_hShutdown( 0 )
, m_hThreadComplete( 0 )
, m_hThread( 0 )
, m_ulThreadID( 0 )
{
}

template<typename SVThreadSignalHandler>
SVThread<SVThreadSignalHandler>::~SVThread()
{
	Destroy();
}

template<typename SVThreadSignalHandler>
DWORD WINAPI SVThread<SVThreadSignalHandler>::ThreadProc( LPVOID lpParam )
{
	HRESULT Result = S_OK;

	if( lpParam )
	{
		#ifdef DEBUG
			SVString l_Message;
		#endif

		SVThread* pThread = static_cast< SVThread* >( lpParam );

		::ResetEvent( pThread->m_hThreadComplete );

		bool l_Continue = true;
		bool l_WaitForEvents = true;

		while( l_Continue )
		{
			if( l_WaitForEvents )
			{
				l_Continue = ::WaitForSingleObjectEx( pThread->m_hShutdown, INFINITE, TRUE ) == WAIT_IO_COMPLETION;
			}

			if( l_Continue )
			{
				l_Continue = ::WaitForSingleObjectEx( pThread->m_hShutdown, 0, FALSE ) == WAIT_TIMEOUT;

				if( l_Continue )
				{
					l_WaitForEvents = true;

					pThread->m_threadHandler( l_WaitForEvents );
				}
				#ifdef DEBUG
					else
					{
						l_Message = SvUl_SF::Format( _T( "SVThread(%d) - Shutdown Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
						::OutputDebugString( l_Message.c_str() );
					}
				#endif
			}
			#ifdef DEBUG
				else
				{
					l_Message = SvUl_SF::Format( _T( "SVThread(%d) - Exit Loop = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
					::OutputDebugString( l_Message.c_str() );
				}
			#endif
		}

		#ifdef DEBUG
			l_Message = SvUl_SF::Format( _T( "SVThread(%d) - Exit Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
			::OutputDebugString( l_Message.c_str() );
		#endif

		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		DWORD errorCode = GetLastError();

		Result = SVMSG_THREAD_EXIT_ERROR;

		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( static_cast<DWORD> (Result), _T( "Unknown Thread" ), StdMessageParams, SvOi::Err_25030_Thread, errorCode );
	}

	return Result;
}

template<typename SVThreadSignalHandler>
HRESULT SVThread<SVThreadSignalHandler>::Create(const SVThreadSignalHandler& threadHandler, LPCTSTR tag, SVThreadAttribute eAttribute )
{
	HRESULT Result = S_OK;

	m_tag = tag;
	m_threadHandler = threadHandler;

	if (!m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( NULL, TRUE, FALSE, NULL );
		if (m_hShutdown == NULL)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25031_Thread, errorCode );
		}
	}

	if( !m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( NULL, TRUE, TRUE, NULL );
		if( m_hThreadComplete == NULL )
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25032_Thread, errorCode );
		}
	}

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( NULL, 0, SVThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );

		if (m_hThread == NULL)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25033_Thread, errorCode );
		}
		else
		{
			SVThreadManager::Instance().Add(m_hThread, tag, eAttribute); // keep track of thread.

			if( Result == S_OK )
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
					Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25034_Thread, errorCode );
				}
			}
		}
	}

	return Result;
}

template<typename SVThreadSignalHandler>
unsigned long SVThread<SVThreadSignalHandler>::GetThreadID() const
{
	return m_ulThreadID;
}

template<typename SVThreadSignalHandler>
HRESULT SVThread<SVThreadSignalHandler>::Restart()
{
	Destroy();

	return Create( m_threadHandler, m_tag.c_str(), SVNone );
}

template< typename SVThreadSignalHandler >
void SVThread< SVThreadSignalHandler >::Destroy()
{
	if( m_hThread )
	{
		if( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT )
		{
			::SetEvent( m_hShutdown );

			if( ::WaitForSingleObject( m_hThreadComplete, m_timeoutShutdownThread ) != WAIT_OBJECT_0 )
			{
				DWORD errorCode = GetLastError();

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_THREAD_EXIT_ERROR, m_tag.c_str(), StdMessageParams, SvOi::Err_25035_Thread, errorCode );

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
				Exception.setMessage( SVMSG_THREAD_EXIT_ERROR, m_tag.c_str(), StdMessageParams, SvOi::Err_25036_Thread, errorCode );

				::TerminateThread( m_hThread, E_FAIL );
			}
		}

		::CloseHandle( m_hThread );
		SVThreadManager::Instance().Remove( m_hThread );
		m_hThread = NULL;
	}

	if( m_hThreadComplete )
	{
		::CloseHandle( m_hThreadComplete );
		m_hThreadComplete = NULL;
	}

	if( m_hShutdown )
	{
		::CloseHandle( m_hShutdown );
		m_hShutdown = NULL;
	}

	m_ulThreadID = 0;
}

template<typename SVThreadSignalHandler>
HANDLE SVThread<SVThreadSignalHandler>::GetThreadHandle() const
{
	return m_hThread;
}

template<typename SVThreadSignalHandler>
int SVThread<SVThreadSignalHandler>::GetPriority() const
{
	return ::GetThreadPriority(m_hThread);
}

template<typename SVThreadSignalHandler>
void SVThread<SVThreadSignalHandler>::SetPriority(int priority)
{
	::SetThreadPriority(m_hThread, priority);
}

template<typename SVThreadSignalHandler>
bool SVThread<SVThreadSignalHandler>::IsActive() const
{
	bool bRetVal = m_hThread != NULL;

	if( bRetVal )
	{
		DWORD exitCode = STILL_ACTIVE;

		bRetVal = bRetVal && ( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT );
		bRetVal = bRetVal && ( ::GetExitCodeThread(m_hThread, &exitCode) != FALSE );
		bRetVal = bRetVal && ( exitCode == STILL_ACTIVE );
	}

	return bRetVal;
}

