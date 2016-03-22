//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallbackStruct
//* .File Name       : $Workfile:   SVTriggerCallbackStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:50  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <deque>
#include "SVTriggerCallbackPtr.h"

struct SVTriggerCallbackStruct
{
	SVTriggerCallbackPtr pCallback;
	void* pOwner;
	void* pData;

	SVTriggerCallbackStruct() : pCallback(0), pOwner(0), pData(0) {}
};

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
typedef std::deque<SVTriggerCallbackStruct> SVTriggerCallbackList;

