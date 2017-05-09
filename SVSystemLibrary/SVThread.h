//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThread
//* .File Name       : $Workfile:   SVThread.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:06  $
//******************************************************************************
#pragma once

#include "SVThreadManager.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVString.h"
/*
SVThreadSignalHandler must have the following prototype:

     void SVProcessThreadData( bool& p_WaitForEvents );

*/

template <typename SVThreadSignalHandler>
class SVThread
{
#pragma region Constructor
public:
	SVThread();
	~SVThread();
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Create(const SVThreadSignalHandler& threadHandler, LPCTSTR tag, SVThreadAttribute eAttribute );
	void Destroy();

	unsigned long GetThreadID() const;

	HRESULT Restart();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;
	HANDLE GetThreadHandle() const;
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
	unsigned long m_ulThreadID;
	SVString m_tag;
	SVThreadSignalHandler m_threadHandler;
	// This const defines how long the destroy should wait at most to complete the shutdown
	// of the thread, before it will kill it.
	// The old value was 5 s, but this was for some cases to short, so we have increased it to 10 s.
	static const int m_timeoutShutdownThread = 10000; 
#pragma endregion Member Variables
};

#include "SVThread.inl"

