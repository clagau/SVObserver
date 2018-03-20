//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Settings.h
/// All Rights Reserved
//******************************************************************************
/// All server settings in one place.
//******************************************************************************

#pragma once

#include "WebsocketLibrary/Logging.h"
#include "SvHttpLibrary/WebsocketServerSettings.h"

namespace SvOws
{
struct Settings
{
	bool dummySharedMemory {false};
	SvWsl::LogSettings logSettings;
	SvHttp::WebsocketServerSettings websocketSettings;
};
} // namespace RRWS