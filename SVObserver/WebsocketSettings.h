//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketSettings.h
/// All Rights Reserved 
//*****************************************************************************
/// Log and http Settings. The settings are load from SVObserverIni with WebsocketSettingsLoader 
//******************************************************************************
#pragma once

#include "SVAuthLibrary/AuthManagerSettings.h"
#include "SVHttpLibrary/HttpServerSettings.h"
#include "SVLogLibrary/LogSettings.h"
#include "WebsocketLibrary/Definition.h"


struct WebSocketSettings
{
	WebSocketSettings()
	{
		httpSettings.Port = SvWsl::Default_SecondPort;
		httpSettings.Host = "127.0.0.1";
	};
	SvLog::LogSettings logSettings;
	SvHttp::HttpServerSettings httpSettings;
};


