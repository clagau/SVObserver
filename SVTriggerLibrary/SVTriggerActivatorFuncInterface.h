//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerActivatorFuncInterface
//* .File Name       : $Workfile:   SVTriggerActivatorFuncInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:56:50  $
//******************************************************************************
#ifndef SVTRIGGERACTIVATORFUNCINTERFACE_H
#define SVTRIGGERACTIVATORFUNCINTERFACE_H

class SVTriggerActivatorFuncInterface
{
public:
	virtual ~SVTriggerActivatorFuncInterface() {}
	virtual HRESULT operator()(unsigned long handle) const=0;
};

#endif

