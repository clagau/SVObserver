//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallback
//* .File Name       : $Workfile:   SVTriggerCallback.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:57:20  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVTriggerCallbackFuncInterface.h"

namespace SvTh
{
	class SVTriggerCallback
	{
	private:
		SVSharedPtr<SVTriggerCallbackFuncInterface> m_pFunc;

	public:
		SVTriggerCallback(SVTriggerCallbackFuncInterface* pFunc);
		SVTriggerCallback(const SVTriggerCallback& rCallback);
		SVTriggerCallback& operator=(const SVTriggerCallback& rCallback);

		HRESULT Notify(unsigned long handle) const;
	};
} //namespace SvTh
