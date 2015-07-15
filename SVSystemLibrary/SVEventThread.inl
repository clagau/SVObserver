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

#include <tchar.h>
#include "SVStatusLibrary/SVException.h"
#include "SVMessage/SVMessage.h"
#include "SVThreadManager.h"

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
						l_Message.Format( _T( "SVEventThread(%d) - Shutdown Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
						::OutputDebugString( l_Message.ToString() );
					}
				#endif
			}
			#ifdef DEBUG
				else
				{
					l_Message.Format( _T( "SVEventThread(%d) - Exit Loop = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
					::OutputDebugString( l_Message.ToString() );
				}
			#endif
		}

		#ifdef DEBUG
			l_Message.Format( _T( "SVEventThread(%d) - Exit Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
			::OutputDebugString( l_Message.ToString() );
		#endif

		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		DWORD errorCode = GetLastError();

		hrOk = SVMSG_THREAD_EXIT_ERROR;

		SVException l_svLog;
		l_svLog.SetException(hrOk, _T(__DATE__), _T(__TIME__), _T( "Unknown Thread" ), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 670, errorCode );
		l_svLog.LogException();
	}

	return hrOk;
}

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Create(const SVEventThreadSignalHandler& threadHandler, LPCTSTR tag)
{
	HRESULT l_Status = S_OK;

	m_tag = tag;
	m_threadHandler = threadHandler;

	if (!m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( NULL, TRUE, FALSE, NULL );
		if (m_hShutdown == NULL)
		{
			l_Status = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SVException l_svLog;
			l_svLog.SetException(l_Status, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 667, errorCode);
			l_svLog.LogException();
		}
	}

	if( !m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( NULL, TRUE, TRUE, NULL );
		if( m_hThreadComplete == NULL )
		{
			l_Status = SVMSG_THREAD_CREATION_ERROR;

			DWORD l_ErrorCode = GetLastError();

			SVException l_svLog;
			l_svLog.SetException( l_Status, _T( __DATE__ ), _T( __TIME__ ), m_tag.c_str(), _T( __FILE__ ), _T( __LINE__ ), _T( __TIMESTAMP__ ), 669, l_ErrorCode );
			l_svLog.LogException();
		}
	}

	if( !m_hSignalEvent )
	{
		m_hSignalEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		if( m_hThreadComplete == NULL )
		{
			l_Status = SVMSG_THREAD_CREATION_ERROR;

			DWORD l_ErrorCode = GetLastError();

			SVException l_svLog;
			l_svLog.SetException( l_Status, _T( __DATE__ ), _T( __TIME__ ), m_tag.c_str(), _T( __FILE__ ), _T( __LINE__ ), _T( __TIMESTAMP__ ), 669, l_ErrorCode );
			l_svLog.LogException();
		}
	}

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( NULL, 0, SVEventThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );
		

		if (m_hThread == NULL)
		{
			l_Status = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SVException l_svLog;
			l_svLog.SetException(l_Status, _T(__DATE__), _T(__TIME__), m_tag.c_str(), _T(__FILE__), _T(__LINE__), _T(__TIMESTAMP__), 668, errorCode);
			l_svLog.LogException();
		}
		else
		{
			SVThreadManager::Instance().Add(m_hThread, tag);

			if( l_Status == S_OK )
			{
				unsigned long l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );

				for( size_t i = 0; l_WaitStatus == WAIT_OBJECT_0 && i < 1000; ++i )
				{
					::Sleep( 1 );

					l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );
				}

				if( l_WaitStatus != WAIT_TIMEOUT )
				{
					l_Status = SVMSG_THREAD_CREATION_ERROR;

					DWORD l_ErrorCode = GetLastError();

					SVException l_svLog;
					l_svLog.SetException( l_Status, _T( __DATE__ ), _T( __TIME__ ), m_tag.c_str(), _T( __FILE__ ), _T( __LINE__ ), _T( __TIMESTAMP__ ), 669, l_ErrorCode );
					l_svLog.LogException();
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
				DWORD l_ErrorCode = GetLastError();

				SVException l_svLog;
				l_svLog.SetException( SVMSG_THREAD_EXIT_ERROR, _T( __DATE__ ), _T( __TIME__ ), m_tag.c_str(), _T( __FILE__ ), _T( __LINE__ ), _T( __TIMESTAMP__ ), 669, l_ErrorCode );
				l_svLog.LogException();

				::TerminateThread( m_hThread, E_FAIL );

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( ::WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT )
			{
				DWORD l_ErrorCode = GetLastError();

				SVException l_svLog;
				l_svLog.SetException( SVMSG_THREAD_EXIT_ERROR, _T( __DATE__ ), _T( __TIME__ ), m_tag.c_str(), _T( __FILE__ ), _T( __LINE__ ), _T( __TIMESTAMP__ ), 669, l_ErrorCode );
				l_svLog.LogException();

				::TerminateThread( m_hThread, E_FAIL );
			}
		}
		SVThreadManager::Instance().Remove(m_hThread);
		::CloseHandle( m_hThread );
		m_hThread = NULL;
	}

	if( m_hSignalEvent )
	{
		::CloseHandle( m_hSignalEvent );
		m_hSignalEvent = NULL;
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

template<typename SVEventThreadSignalHandler>
HRESULT SVEventThread<SVEventThreadSignalHandler>::Signal()
{
	HRESULT l_Status = S_OK;

	if( ! IsActive() )
	{
		l_Status = Restart();
	}

	if( l_Status == S_OK )
	{
		if( ! ::SetEvent( m_hSignalEvent ) )
		{
			l_Status = Restart();

			if( l_Status == S_OK )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSystemLibrary\SVEventThread.inl_v  $
 * 
 *    Rev 1.1   01 Dec 2014 13:59:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:51:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jul 2012 16:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2011 15:14:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to allow access to the thread identifier.  Added a new Event notified thread object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/