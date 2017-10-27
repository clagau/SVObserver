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
namespace SvSol
{
	template<typename ThreadFunc>
	SVSocketThread<ThreadFunc>::SVSocketThread()
		: m_hThread(0)
		, m_ulThreadID(0)
		, m_hStopEvent(nullptr)
	{

	}

	template<typename ThreadFunc>
	SVSocketThread<ThreadFunc>::~SVSocketThread()
	{
		Destroy();
		if (m_hStopEvent)
		{
			::CloseHandle(m_hStopEvent);
			m_hStopEvent = nullptr;
		}

	}

	template<typename ThreadFunc>
	DWORD WINAPI SVSocketThread<ThreadFunc>::ThreadProc(LPVOID lpParam)
	{
		if (lpParam)
		{
			CoInitialize(NULL); // thread might call COM
			SVSocketThread* pThread = static_cast<SVSocketThread*>(lpParam);

			while (::WaitForSingleObject(pThread->m_hStopEvent, 0) == WAIT_TIMEOUT)
			{
				pThread->m_threadFunc();
			}
			CoUninitialize();
		}
		return 0;
	}

	template<typename ThreadFunc>
	HRESULT SVSocketThread<ThreadFunc>::Create(const ThreadFunc& threadFunc, const _bstr_t& p_rTag)
	{
		HRESULT l_Status = S_OK;
		m_hStopEvent = ::CreateEvent(nullptr, true, false, nullptr);
		if (!m_hStopEvent)
		{
			ASSERT(FALSE);
			return E_FAIL;
		}

		m_tag = p_rTag;
		m_threadFunc = threadFunc;

		if (!m_hThread)
		{
			m_hThread = ::CreateThread(NULL, 0, SVSocketThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID);

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
		if (m_hThread)
		{
			::SetEvent(m_hStopEvent);
			if (::WaitForSingleObject(m_hThread, 2000) != WAIT_OBJECT_0)
			{
				::TerminateThread(m_hThread, E_FAIL);
			}
			::CloseHandle(m_hThread);
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

		if (bRetVal)
		{
			DWORD exitCode = STILL_ACTIVE;

			BOOL rc = ::GetExitCodeThread(m_hThread, &exitCode);
			bRetVal = bRetVal && (rc != FALSE);
			bRetVal = bRetVal && (exitCode == STILL_ACTIVE);
		}
		return bRetVal;
	}

}