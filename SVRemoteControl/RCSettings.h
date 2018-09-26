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
		logSettings.FileLogEnabled = false;
		logSettings.StdoutLogEnabled = true;
		logSettings.StdoutLogLevel = "debug";
		logSettings.WindowsEventLogEnabled = true;
		logSettings.WindowsEventLogLevel = "info";
	}
	SvLog::LogSettings logSettings;
	SvHttp::WebsocketClientSettings httpClientSettings;
};

