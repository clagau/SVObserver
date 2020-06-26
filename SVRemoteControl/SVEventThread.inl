//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventThread
//* .File Name       : $Workfile:   SVEventThread.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Sep 2011 15:14:50  $
//******************************************************************************

#pragma region Includes
#include <tchar.h>
#pragma endregion Includes

static const int FIVE_SECONDS = 5000;
static const int RETRY_COUNT = 1000;

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
	HRESULT hrOk = S_OK;

	::CoInitialize( nullptr );

	if( lpParam )
	{
		SVEventThread* pThread = static_cast< SVEventThread* >( lpParam );

		::ResetEvent( pThread->m_hThreadComplete );

		bool l_Continue = true;
		bool l_WaitForEvents = true;

		HANDLE l_pHandles[ 2 ] = { pThread->m_hSignalEvent, pThread->m_hShutdown };

		while( l_Continue )
		{
			if( l_WaitForEvents )
			{
				l_Continue = ( WAIT_OBJECT_0 == ::WaitForMultipleObjects( 2, l_pHandles, false, INFINITE ) );
			}

			if( l_Continue )
			{
				l_Continue = WAIT_TIMEOUT == ::WaitForSingleObject( pThread->m_hShutdown, 0 );

				if( l_Continue )
				{
					l_WaitForEvents = true;

					pThread->m_threadHandler( l_WaitForEvents );
				}
			}
		}

		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		hrOk = E_FAIL;
	}

	::CoUninitialize();

	return hrOk;
}

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Create(const SVEventThreadSignalHandler& threadHandler, const _bstr_t& tag)
{
	HRESULT l_Status = S_OK;

	m_tag = tag;
	m_threadHandler = threadHandler;

	if (!m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( nullptr, true, false, nullptr );
		if (nullptr == m_hShutdown)
		{
			l_Status = E_FAIL;
		}
	}

	if( !m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( nullptr, true, true, nullptr );
		if( nullptr == m_hThreadComplete )
		{
			l_Status = E_FAIL;
		}
	}

	if( !m_hSignalEvent )
	{
		m_hSignalEvent = ::CreateEvent( nullptr, false, false, nullptr );
		if( nullptr == m_hThreadComplete )
		{
			l_Status = E_FAIL;
		}
	}

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( nullptr, 0, SVEventThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );

		if (nullptr == m_hThread )
		{
			l_Status = E_FAIL;
		}
		else
		{
			if( S_OK == l_Status )
			{
				unsigned long l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );

				for( size_t i = 0; WAIT_OBJECT_0 == l_WaitStatus && i < RETRY_COUNT; ++i )
				{
					::Sleep( 1 );

					l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );
				}

				if( WAIT_TIMEOUT != l_WaitStatus )
				{
					l_Status = E_FAIL;
				}
			}
		}
	}
	return l_Status;
}

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Restart()
{
	Destroy();

	return Create( m_threadHandler, m_tag );
}

template< typename SVEventThreadSignalHandler >
void SVEventThread< SVEventThreadSignalHandler >::Destroy()
{
	if( m_hThread )
	{
		if( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThreadComplete, 0 ) )
		{
			::SetEvent( m_hShutdown );

			if( WAIT_OBJECT_0 != ::WaitForSingleObject( m_hThreadComplete, FIVE_SECONDS ) )
			{
				::TerminateThread(m_hThread, static_cast<DWORD> (E_FAIL));

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThread, 0 ) )
			{
				::TerminateThread(m_hThread, static_cast<DWORD> (E_FAIL));
			}
		}

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

		bRetVal = bRetVal && ( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThreadComplete, 0 ) );
		bRetVal = bRetVal && ( FALSE != ::GetExitCodeThread(m_hThread, &exitCode) );
		bRetVal = bRetVal && ( STILL_ACTIVE == exitCode );
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
