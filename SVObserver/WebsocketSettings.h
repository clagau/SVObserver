//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file WebsocketSettings.h
/// All Rights Reserved 
//*****************************************************************************
/// Log and http Settings. The settings are load from SVObserverIni with WebsocketSettingsLoader 
//******************************************************************************
#pragma once

#include "SVHttpLibrary/HttpServerSettings.h"
#include "SVLogLibrary/LogSettings.h"
#include "SvHttpLibrary/DefaultSettings.h"
#include "SVSharedMemoryLibrary/ShareControlSetting.h"


struct WebSocketSettings
{
	WebSocketSettings()
	{
		httpSettings.Port = SvHttp::Default_SecondPort;
		httpSettings.Host = "127.0.0.1";
		logSettings.FileLogEnabled = false;
		logSettings.StdoutLogEnabled = false;
		logSettings.WindowsEventLogEnabled = true;
		logSettings.WindowsEventLogLevel = "info";
		logSettings.eventLogFacility  = SvLog::EventLogFacility::Websocket;
	};
	SvLog::LogSettings logSettings;
	SvHttp::HttpServerSettings httpSettings;
	SvSml::ShareControlSettings shareControlSettings;
};


