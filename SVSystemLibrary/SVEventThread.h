//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventThread
//* .File Name       : $Workfile:   SVEventThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:50:48  $
//******************************************************************************

#ifndef SVEVENTTHREAD_H
#define SVEVENTTHREAD_H

#include <string>

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
	std::string m_tag;
	SVEventThreadSignalHandler m_threadHandler;

	static DWORD WINAPI ThreadProc( LPVOID lpParam );

public:
	SVEventThread();
	~SVEventThread();

	HRESULT Create(const SVEventThreadSignalHandler& threadHandler, LPCTSTR tag);
	void Destroy();

	HRESULT Restart();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;

	HRESULT Signal();

};

#include "SVEventThread.inl"

#endif

