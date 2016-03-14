//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadWait
//* .File Name       : $Workfile:   SVThreadWait.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:03:30  $
//******************************************************************************

#pragma once

#include <boost/config.hpp>
#include <boost/function.hpp>

#include "SVEventThread.h"

class SVThreadWait
{
public:
	SVThreadWait();
	virtual ~SVThreadWait();

	HRESULT Create();
	void Destroy();

	void Sleep();

protected:
	typedef boost::function<void ( bool& )> SVThreadSignalHandler;
	typedef SVEventThread<SVThreadSignalHandler> SVWaitEventThread;

	void Dispatch( bool& p_WaitForEvents );

	SVWaitEventThread m_Thread;
	HANDLE m_WaitEvent;

};

