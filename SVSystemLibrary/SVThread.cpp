//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThread
//* .File Name       : $Workfile:   SVThread.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVThread.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageData.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVLogLibrary/Logging.h"
#pragma endregion Includes

// This const defines how long the destroy should wait at most to complete the shutdown
// of the thread, before it will kill it.
// The old value was 5 s, but this was for some cases to short, so we have increased it to 10 s.
constexpr int cTimeoutShutdownThread = 10000;

bool SVThread::m_diagnostic{ false };

#pragma region Constructor
SVThread::~SVThread()
{
	Destroy();
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT SVThread::Create(const ProcessThread& rProcessThread, LPCTSTR tag, SVThreadAttribute eAttribute )
{
	HRESULT Result = S_OK;

	m_tag = tag;
	m_pProcessThread = rProcessThread;
	if (nullptr == m_pProcessThread || true == m_tag.empty())
	{
		Result = SVMSG_THREAD_CREATION_ERROR;
		SVThreadManager::setThreadError(static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams));
		return E_FAIL;
	}

	if (!m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( nullptr, true, false, nullptr );
		if (nullptr == m_hShutdown)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;
			SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	if(nullptr == m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( nullptr, true, true, nullptr );
		if( nullptr == m_hThreadComplete )
		{
			Result = SVMSG_THREAD_CREATION_ERROR;
			SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		}
	}

	if (nullptr == m_hThread)
	{
		m_hThread = ::CreateThread( nullptr, 0, SVThread::ThreadProc, reinterpret_cast<LPVOID> (this), 0, &m_ulThreadID );

		if (nullptr == m_hThread)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;
			SVThreadManager::setThreadError( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		}
		else
		{
			if (m_diagnostic)
			{
				SvDef::StringVector msgList;
				std::string message = std::to_string(m_ulThreadID) + ' ' + m_tag;
				msgList.push_back(message);
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CreateThread, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
			
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

unsigned long SVThread::GetThreadID() const
{
	return m_ulThreadID;
}

HRESULT SVThread::Restart()
{
	///@TODO[gra][10.10][04.09.2020]: This is to avoid problems when threads are present but are not needed
	///e.g. Trigger that is not used in the configuration but the hardware trigger is present
	/// The SVTriggerProcessingClass::Startup and SVDigitizerProcessingClass::Startup creates all devices even those not required!
	if (nullptr != m_pProcessThread && false == m_tag.empty())
	{
		Destroy();

		return Create(m_pProcessThread, m_tag.c_str(), SVNone);
	}

	return S_OK;
}

void SVThread::Destroy()
{
	if( m_hThread )
	{
		if( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT )
		{
			::SetEvent( m_hShutdown );

			if( ::WaitForSingleObject( m_hThreadComplete, cTimeoutShutdownThread) != WAIT_OBJECT_0 )
			{
				SVThreadManager::setThreadError( static_cast<DWORD> (SVMSG_THREAD_EXIT_ERROR), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );

				::TerminateThread(m_hThread, static_cast<DWORD> (E_FAIL));

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( ::WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT )
			{
				SVThreadManager::setThreadError( static_cast<DWORD> (SVMSG_THREAD_EXIT_ERROR), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams) );

				::TerminateThread(m_hThread, static_cast<DWORD> (E_FAIL));
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
	if (m_diagnostic && 0 != m_ulThreadID)
	{
		SvDef::StringVector msgList;
		std::string message = std::to_string(m_ulThreadID) + ' ' + m_tag;
		msgList.push_back(message);
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_DestroyThread, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	m_tag.clear();
	m_ulThreadID = 0;
}

HANDLE SVThread::GetThreadHandle() const
{
	return m_hThread;
}

int SVThread::GetPriority() const
{
	int Result{ -1 };
	if( nullptr != m_hThread )
	{
		Result = ::GetThreadPriority(m_hThread);
	}
	return Result;
}

void SVThread::SetPriority(int priority)
{
	if( nullptr != m_hThread )
	{
		::SetThreadPriority(m_hThread, priority);
	}
}

bool SVThread::IsActive() const
{
	bool bRetVal = nullptr != m_hThread;
	bRetVal = bRetVal && ( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT );
	bRetVal = bRetVal && (::WaitForSingleObject(m_hThread,0) == WAIT_TIMEOUT) ;
	return bRetVal;
}

#pragma endregion Public Methods

#pragma region Private Methods
DWORD WINAPI SVThread::ThreadProc(LPVOID pParam)
{
	HRESULT result{ S_OK };

	if(nullptr != pParam)
	{

		SVThread* pThread = static_cast<SVThread*> (pParam);

		::ResetEvent( pThread->m_hThreadComplete );

		bool continueThread = true;
		bool waitForEvents = true;

		while(continueThread)
		{
			if( waitForEvents )
			{
				continueThread = ::WaitForSingleObjectEx( pThread->m_hShutdown, INFINITE, TRUE ) == WAIT_IO_COMPLETION;
			}

			if(continueThread)
			{
				continueThread = ::WaitForSingleObjectEx( pThread->m_hShutdown, 0, FALSE ) == WAIT_TIMEOUT;

				if(continueThread)
				{
					waitForEvents = true;

					if (nullptr != pThread->m_pProcessThread)
					{
						pThread->m_pProcessThread(waitForEvents);
					}
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
		UnknownThread = SvStl::MessageData::convertId2AdditionalText( SvStl::Tid_UnknowThread );
		SVThreadManager::setThreadError( static_cast<DWORD> (SVMSG_THREAD_EXIT_ERROR), UnknownThread.c_str(), SvStl::SourceFileParams(StdMessageParams) );
	}

	return static_cast<DWORD> (result);
}
#pragma endregion Private Methods
