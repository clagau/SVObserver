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
HRESULT SVThread::Create(LPCTSTR tag)
{
	HRESULT result {S_OK};

	m_threadName = tag;
	if (true == m_threadName.empty())
	{
		return E_FAIL;
	}

	if (nullptr == m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( nullptr, true, false, nullptr );
		if (nullptr == m_hShutdown)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_THREAD_CREATION_ERROR, m_threadName.c_str(), SvStl::SourceFileParams(StdMessageParams));
			result = SVMSG_THREAD_CREATION_ERROR;
			return result;
		}
	}

	if (nullptr == m_thread.native_handle() && false == m_thread.joinable())
	{
		m_thread = std::thread(&SVThread::ThreadProc, reinterpret_cast<LPVOID> (this));
	}

	return result;
}

void SVThread::Destroy()
{
	if( m_thread.joinable() )
	{
		if (nullptr != m_hShutdown)
		{
			::SetEvent(m_hShutdown);
		}
		m_thread.join();
	}

	if(nullptr != m_hShutdown )
	{
		::CloseHandle( m_hShutdown );
		m_hShutdown = nullptr;
	}
	m_threadName.clear();
}

HANDLE SVThread::GetThreadHandle() const
{
	HANDLE result = m_thread.native_handle();
	return result;
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

bool SVThread::IsActive() const
{
	return m_thread.joinable();
}
#pragma endregion Public Methods

#pragma region Private Methods
DWORD WINAPI SVThread::ThreadProc(LPVOID pParam)
{
	HRESULT result{ S_OK };

	SVThread* pThread = reinterpret_cast<SVThread*> (pParam);
	if (nullptr != pThread)
	{
		bool runThread {true};
		while (runThread)
		{
			runThread = ::WaitForSingleObjectEx(pThread->m_hShutdown, INFINITE, TRUE) == WAIT_IO_COMPLETION;
		}
	}

	return static_cast<DWORD> (result);
}
#pragma endregion Private Methods
} //namespace SvSyl