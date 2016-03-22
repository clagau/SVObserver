//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncProcedure
//* .File Name       : $Workfile:   SVAsyncProcedure.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

#pragma once

#include "SVThread.h"

template <typename SVAPCSignalHandler, typename SVThreadSignalHandler>
class SVAsyncProcedure
{
private:
	SVThread<SVThreadSignalHandler> m_thread;
	SVAPCSignalHandler m_apcHandler;
	std::string m_tag;

	// The APCSignal handler must implement this
	//static void CALLBACK APCProc( DWORD dwParam );
	
public:
	SVAsyncProcedure();
	~SVAsyncProcedure();

	HRESULT Create(const SVAPCSignalHandler& apcHandler, const SVThreadSignalHandler& threadHandler, LPCTSTR tag, SVThreadAttribute eAttribute );
	void Destroy();

	unsigned long GetThreadID() const;

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;

	HRESULT Signal(void* pData);
};

#include "SVAsyncProcedure.inl"


