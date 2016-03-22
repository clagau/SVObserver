//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSubscriberProviderInterfaces
//* .File Name       : $Workfile:   SVSubscriberProviderInterfaces.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:15:36  $
//******************************************************************************

#pragma once

#include "SVOLibrary/SVOCallbackClass.h"

// temp hack
//typedef HRESULT (*SVSUBSCRIBERCALLBACKFNPTR) (ULONG);

class SVSubscriberInterface
{
public:
	// temp hack
	virtual SVOCallbackPtr GetCallbackFunction(const CString& sType) const = 0;

};

class SVProviderInterface
{
};


