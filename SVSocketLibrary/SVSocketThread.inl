//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketThread
//* .File Name       : $Workfile:   SVSocketThread.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:18:10  $
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
		m_hThread = ::CreateThread( NULL, 0, SVSocketThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );

		if (m_hThread == NULL)
		{
			l_Status = S_FALSE;
		}
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
		m_hThread = NULL;
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
	bool bRetVal = m_hThread != NULL;

	if( bRetVal )
	{
		DWORD exitCode = STILL_ACTIVE;

		BOOL rc = ::GetExitCodeThread(m_hThread, &exitCode);
		bRetVal = bRetVal && ( rc != FALSE );
		bRetVal = bRetVal && ( exitCode == STILL_ACTIVE );
	}
	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSocketLibrary\SVSocketThread.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:18:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2013 11:02:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to include error messages and changes thread name from standard string to basic string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/