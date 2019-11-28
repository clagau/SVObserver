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

#pragma region Includes
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageData.h"
#pragma endregion Includes

#pragma region Constructor
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
#pragma endregion Constructor

#pragma region Public Methods
template<typename SVThreadSignalHandler>
HRESULT SVThread<SVThreadSignalHandler>::Create(const SVThreadSignalHandler& threadHandler, LPCTSTR tag, SVThreadAttribute eAttribute )
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
			SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	if( !m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( nullptr, true, true, nullptr );
		if( nullptr == m_hThreadComplete )
		{
			Result = SVMSG_THREAD_CREATION_ERROR;
			SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( nullptr, 0, SVThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );

		if (nullptr == m_hThread)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;
			SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		}
		else
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_THREAD)
			std::string l_Message = SvUl::Format(_T("SVThread(%d) - Create thread = %s\n"), m_ulThreadID, m_tag.c_str());
			::OutputDebugString(l_Message.c_str());
#endif
	
			SVThreadManager::Instance().Add(m_hThread, tag, eAttribute); // keep track of thread.

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
					SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
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
				SVThreadManager::setThreadError( static_cast<DWORD> (SVMSG_THREAD_EXIT_ERROR), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );

				::TerminateThread( m_hThread, E_FAIL );

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( ::WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT )
			{
				SVThreadManager::setThreadError( static_cast<DWORD> (SVMSG_THREAD_EXIT_ERROR), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );

				::TerminateThread( m_hThread, E_FAIL );
			}
		}

		::CloseHandle( m_hThread );
		SVThreadManager::Instance().Remove( m_hThread );
		m_hThread = nullptr;
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

template<typename SVThreadSignalHandler>
HANDLE SVThread<SVThreadSignalHandler>::GetThreadHandle() const
{
	return m_hThread;
}

template<typename SVThreadSignalHandler>
int SVThread<SVThreadSignalHandler>::GetPriority() const
{
	int Result( -1 );
	if( nullptr != m_hThread )
	{
		Result = ::GetThreadPriority(m_hThread);
	}
	return Result;
}

template<typename SVThreadSignalHandler>
void SVThread<SVThreadSignalHandler>::SetPriority(int priority)
{
	if( nullptr != m_hThread )
	{
		::SetThreadPriority(m_hThread, priority);
	}
}

template<typename SVThreadSignalHandler>
bool SVThread<SVThreadSignalHandler>::IsActive() const
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

#pragma endregion Public Methods

#pragma region Private Methods
template<typename SVThreadSignalHandler>
DWORD WINAPI SVThread<SVThreadSignalHandler>::ThreadProc( LPVOID lpParam )
{
	HRESULT Result = S_OK;

	if( lpParam )
	{

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
#if defined (TRACE_THEM_ALL) || defined (TRACE_THREAD)
				else
				{
					std::string l_Message = SvUl::Format( _T( "SVThread(%d) - Shutdown Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
					::OutputDebugString( l_Message.c_str() );
				}
#endif
			}
#if defined (TRACE_THEM_ALL) || defined (TRACE_THREAD)
			else
			{
				std::string l_Message = SvUl::Format( _T( "SVThread(%d) - Exit Loop = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
				::OutputDebugString( l_Message.c_str() );
			}
#endif
		}

#ifdef DEBUG
#if defined (TRACE_THEM_ALL) || defined (TRACE_THREAD)
		l_Message = SvUl::Format( _T( "SVThread(%d) - Exit Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
		::OutputDebugString( l_Message.c_str() );
#endif
#endif

		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		std::string UnknownThread;
		UnknownThread = SvStl::MessageData::convertId2AddtionalText( SvStl::Tid_UnknowThread );
		SVThreadManager::setThreadError( static_cast<DWORD> (SVMSG_THREAD_EXIT_ERROR), UnknownThread.c_str(), SvStl::SourceFileParams(StdMessageParams) );
	}

	return Result;
}
#pragma endregion Private Methods
