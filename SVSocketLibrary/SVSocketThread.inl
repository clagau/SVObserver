//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketThread
//* .File Name       : $Workfile:   SVSocketThread.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:47:40  $
//******************************************************************************

template<typename ThreadFunc>
SVSocketThread<ThreadFunc>::SVSocketThread()
: m_hThread( 0 )
, m_ulThreadID( 0 )
, m_bQuit(false)
{
}

template<typename ThreadFunc>
SVSocketThread<ThreadFunc>::~SVSocketThread()
{
	Destroy();
}

template<typename ThreadFunc>
DWORD WINAPI SVSocketThread<ThreadFunc>::ThreadProc( LPVOID lpParam )
{
	if( lpParam )
	{
		SVSocketThread* pThread = static_cast< SVSocketThread* >( lpParam );

		while( !pThread->m_bQuit )
		{
			pThread->m_threadFunc();
		}
	}
	return 0;
}

template<typename ThreadFunc>
HRESULT SVSocketThread<ThreadFunc>::Create(const ThreadFunc& threadFunc, const _bstr_t& p_rTag)
{
	HRESULT l_Status = S_OK;
	m_bQuit = false;
	m_tag = p_rTag;
	m_threadFunc = threadFunc;

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( nullptr, 0, SVSocketThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );

		if (nullptr == m_hThread)
		{
			l_Status = S_FALSE;
		}
		SVThreadManager::Instance().Add( m_hThread, p_rTag );
	}
	return l_Status;
}

template< typename ThreadFunc >
void SVSocketThread< ThreadFunc >::Destroy()
{
	if( m_hThread )
	{
		m_bQuit = true;
		
		while (IsActive())
		{
			Sleep(1);
		}
		::CloseHandle( m_hThread );
		SVThreadManager::Instance().Remove(m_hThread );
		m_hThread = nullptr;
	}

	m_ulThreadID = 0;
}

template<typename ThreadFunc>
HANDLE SVSocketThread<ThreadFunc>::GetThreadHandle() const
{
	return m_hThread;
}

template<typename ThreadFunc>
int SVSocketThread<ThreadFunc>::GetPriority() const
{
	return ::GetThreadPriority(m_hThread);
}

template<typename ThreadFunc>
void SVSocketThread<ThreadFunc>::SetPriority(int priority)
{
	::SetThreadPriority(m_hThread, priority);
}

template<typename ThreadFunc>
bool SVSocketThread<ThreadFunc>::IsActive() const
{
	bool bRetVal = nullptr != m_hThread ;

	if( bRetVal )
	{
		DWORD exitCode = STILL_ACTIVE;

		BOOL rc = ::GetExitCodeThread(m_hThread, &exitCode);
		bRetVal = bRetVal && ( rc != FALSE );
		bRetVal = bRetVal && ( exitCode == STILL_ACTIVE );
	}
	return bRetVal;
}

