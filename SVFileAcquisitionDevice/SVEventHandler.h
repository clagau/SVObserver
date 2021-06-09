//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventHandler
//* .File Name       : $Workfile:   SVEventHandler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:25:54  $
//******************************************************************************

#pragma once

template <typename T>
class SVEventHandler
{
private:
	typedef T SVHandlerFunc;
	 SVHandlerFunc m_handler;

public:
	SVEventHandler();
	SVEventHandler(const SVEventHandler& rHandler);
	SVEventHandler& operator=(const SVEventHandler& rHandler); 
	SVEventHandler& operator=(const SVHandlerFunc& func); 

	HRESULT Fire(unsigned long handle);
};

#include "SVEventhandler.inl"

