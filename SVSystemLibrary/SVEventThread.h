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
#pragma once

#include "SVThreadManager.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"

template< typename SVEventThreadSignalHandler >
class SVEventThread
{
#pragma region Constructor
public:
	SVEventThread();
	~SVEventThread();
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Create(const SVEventThreadSignalHandler& threadHandler, LPCTSTR tag);
	void Destroy();

	HRESULT Restart();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;

	HRESULT Signal();
#pragma endregion Public Methods

#pragma region Private Methods
private:
	static DWORD WINAPI ThreadProc( LPVOID lpParam );
#pragma endregion Private Methods

#pragma region Member Variables
private:
	HANDLE m_hShutdown;
	HANDLE m_hThreadComplete;
	HANDLE m_hThread;
	HANDLE m_hSignalEvent;
	unsigned long m_ulThreadID;
	std::string m_tag;
	SVEventThreadSignalHandler m_threadHandler;
#pragma endregion Member Variables
};

#include "SVEventThread.inl"

