//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file CallbackStructContainers.h
/// All Rights Reserved
//*****************************************************************************
  
/// Containers for trigger-related information
//******************************************************************************

#pragma once

#include "TriggerBasics.h"

namespace Seidenader { namespace TriggerHandling {

	typedef std::vector<TriggerCallbackInformation> TriggerCallbackList;
	typedef std::map<unsigned long, TriggerCallbackList> TriggerCallbackMap;

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;
