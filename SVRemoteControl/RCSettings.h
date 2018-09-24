//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file RCSettings.h
/// All Rights Reserved 
//*****************************************************************************
/// Settings for the SVREmotecontrol. 
//******************************************************************************
#pragma once

#include "SVLogLibrary/Logging.h"
#include "SVHttpLibrary/WebsocketClientSettings.h"

struct RCSettings
{
	RCSettings()
	{
		logSettings.log_to_stdout_enabled = false;
		logSettings.windows_event_log_enabled = true;
		logSettings.windows_event_log_level = "info";
		logSettings.log_level = "debug";
	}
	SvLog::LogSettings logSettings;
	SvHttp::WebsocketClientSettings httpClientSettings;
};

