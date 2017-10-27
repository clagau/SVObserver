//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventThread
//* .File Name       : $Workfile:   SVEventThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:38:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <string>
#pragma endregion Includes

/*
SVEventThreadSignalHandler must have the following prototype:
     void SVProcessThreadData( bool& p_WaitForEvents );
*/

template< typename SVEventThreadSignalHandler >
class SVEventThread
{
private:
	HANDLE m_hShutdown;
	HANDLE m_hThreadComplete;
	HANDLE m_hThread;
	HANDLE m_hSignalEvent;
	unsigned long m_ulThreadID;
	_bstr_t m_tag;
	SVEventThreadSignalHandler m_threadHandler;

	static DWORD WINAPI ThreadProc( LPVOID lpParam );

public:
	SVEventThread();
	~SVEventThread();

	HRESULT Create(const SVEventThreadSignalHandler& threadHandler, const _bstr_t& tag);
	void Destroy();

	HRESULT Restart();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;

	HRESULT Signal();
};

#include "SVEventThread.inl"
