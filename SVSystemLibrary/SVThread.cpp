//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

// This const defines how long the destroy should wait at most to complete the shutdown
// of the thread, before it will kill it.
// The old value was 5 s, but this was for some cases to short, so we have increased it to 10 s.
constexpr int cTimeoutShutdownThread = 10000;

namespace SvSyl
{
#pragma region Constructor
SVThread::~SVThread()
{
	Destroy();
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT SVThread::Create(SignalFunctor callback, LPCTSTR tag)
{
	HRESULT result {S_OK};

	Destroy();
	m_callback = callback;
	m_threadName = tag;
	if (true == m_threadName.empty())
	{
		return E_FAIL;
	}

	for (int i = 0; i < ThreadHandles::HandleCount; ++i)
	{
		m_eventHandles[i] = ::CreateEvent(nullptr, i == ThreadHandles::ShutDown, false, nullptr);
		if (nullptr == m_eventHandles[i])
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_THREAD_CREATION_ERROR, m_threadName.c_str(), SvStl::SourceFileParams(StdMessageParams));
			result = SVMSG_THREAD_CREATION_ERROR;
			return result;
		}
	}

	if (nullptr == m_thread.native_handle() && false == m_thread.joinable())
	{
		m_thread = std::thread(&SVThread::ThreadProc, this);
	}

	return result;
}

void SVThread::Destroy()
{
	if( m_thread.joinable() )
	{
		if (nullptr != m_eventHandles[ThreadHandles::ShutDown])
		{
			::SetEvent(m_eventHandles[ThreadHandles::ShutDown]);
		}
		m_thread.join();
	}

	for (int i = 0; i < ThreadHandles::HandleCount; ++i)
	{
		if (nullptr != m_eventHandles[i])
		{
			::CloseHandle(m_eventHandles[i]);
			m_eventHandles[i] = nullptr;
		}
	}
	m_threadName.clear();
}

void SVThread::Signal(const ULONG_PTR pCaller)
{
	if (nullptr != m_eventHandles[ThreadHandles::DoSignal])
	{
		{
			std::lock_guard<std::mutex> guard {m_threadMutex};
			m_signalQueue.push(pCaller);
		}
		::SetEvent(m_eventHandles[ThreadHandles::DoSignal]);
	}
}

bool SVThread::IsActive() const
{
	return m_thread.joinable();
}

int SVThread::GetPriority() const
{
	int Result{ -1 };
	if( nullptr != m_thread.native_handle())
	{
		Result = ::GetThreadPriority(m_thread.native_handle());
	}
	return Result;
}

void SVThread::SetPriority(int priority)
{
	if( nullptr != m_thread.native_handle() )
	{
		::SetThreadPriority(m_thread.native_handle(), priority);
	}
}

void SVThread::ProcessQueue()
{
	bool process {true};
	while(process)
	{
		ULONG_PTR pCaller {0ULL};
		{
			std::lock_guard<std::mutex> guard {m_threadMutex};
			process = false == m_signalQueue.empty();
			if (process)
			{
				pCaller = m_signalQueue.front();
				m_signalQueue.pop();
			}
		}
		if (process)
		{
			m_callback(pCaller);
		}
	}
}
#pragma endregion Public Methods

#pragma region Private Methods
void SVThread::ThreadProc(SVThread* pThread)
{
	if (nullptr != pThread)
	{
		bool runThread {true};
		while (runThread)
		{
			if (::WaitForMultipleObjects(ThreadHandles::HandleCount, pThread->m_eventHandles, false, INFINITE) == WAIT_OBJECT_0)
			{
				pThread->ProcessQueue();
			}
			else
			{
				runThread = false;
			}
		}
	}
}
#pragma endregion Private Methods
} //namespace SvSyl