//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallbackFuncInterface
//* .File Name       : $Workfile:   SVTriggerCallbackFuncInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:57:50  $
//******************************************************************************

#pragma once

class SVTriggerCallbackFuncInterface
{
public:
	virtual ~SVTriggerCallbackFuncInterface() {}
	virtual HRESULT operator()(unsigned long handle) const=0;
};


