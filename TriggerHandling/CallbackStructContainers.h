//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file CallbackStructContainers.h
/// All Rights Reserved
//*****************************************************************************
  
/// Containers for trigger-related information
//******************************************************************************

#pragma once

#include "TriggerBasics.h"

namespace SvTh
{
	typedef std::vector<TriggerDispatcher> DispatcherVector;
	typedef std::map<unsigned long, DispatcherVector> TriggerDispatcherMap;
} //namespace SvTh
