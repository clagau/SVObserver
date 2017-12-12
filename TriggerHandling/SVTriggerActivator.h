//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerActivator
//* .File Name       : $Workfile:   SVTriggerActivator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:56:20  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVTriggerActivatorFuncInterface.h"
#pragma endregion Includes

namespace SvTh
{
	class SVTriggerActivator
	{
	private:
		std::shared_ptr<SVTriggerActivatorFuncInterface> m_pFunc;

	public:
		SVTriggerActivator(SVTriggerActivatorFuncInterface* pFunc);
		SVTriggerActivator(const SVTriggerActivator& rActivator);
		SVTriggerActivator& operator=(const SVTriggerActivator& rActivator);

		HRESULT Exec(unsigned long handle) const;
	};

}//namespace SvTh