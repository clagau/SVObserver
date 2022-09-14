//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThread
//* .File Name       : $Workfile:   SVThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvStl
{
struct SourceFileParams;
}

namespace SvSyl
{

class SVThread
{
	using SignalFunctor = std::function<void(const ULONG_PTR)>;
	enum ThreadHandles
	{
		DoSignal,
		ShutDown,
		HandleCount
	};

#pragma region Constructor
public:
	SVThread() = default;
	~SVThread();
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Create(SignalFunctor callback, LPCTSTR tag);
	void Destroy();

	void Signal(const ULONG_PTR pCaller);

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const { return m_thread.native_handle(); }

	void ProcessQueue();
#pragma endregion Public Methods

#pragma region Private Methods
private:
	static void ThreadProc(SVThread* pThread);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	mutable std::thread m_thread {};
	SignalFunctor m_callback {nullptr};
	HANDLE m_eventHandles[ThreadHandles::HandleCount] {nullptr, nullptr};
	std::mutex m_threadMutex {};
	std::queue<ULONG_PTR> m_signalQueue {};
	std::string m_threadName {};
#pragma endregion Member Variables
};
} //namespace SvSyl