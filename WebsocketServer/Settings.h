//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Settings.h
/// All Rights Reserved
//******************************************************************************
/// All server settings in one place.
//******************************************************************************

#pragma once

#include "WebsocketLibrary/Logging.h"
#include "SVHttpLibrary/WebsocketServerSettings.h"

namespace RRWS
{
struct Settings
{
	bool dummySharedMemory {false};
	LogSettings logSettings;
	SVHTTP::WebsocketServerSettings websocketSettings;
};
} // namespace RRWS