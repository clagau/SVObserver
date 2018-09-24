//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Settings.h
/// All Rights Reserved
//******************************************************************************
/// All server settings in one place.
//******************************************************************************

#pragma once

#include "SVAuthLibrary/AuthManagerSettings.h"
#include "SVHttpLibrary/HttpServerSettings.h"
#include "SVLogLibrary/LogSettings.h"
#include "WebsocketLibrary/Definition.h"
#include "SVHttpLibrary/WebsocketClientSettings.h"

namespace SvOgw
{
struct Settings
{
	Settings()
	{
		observerSetting.Port = SvWsl::Default_SecondPort;
		observerSetting.Host = "127.0.0.1";
	}
	bool dummySharedMemory {false};
	SvHttp::WebsocketClientSettings observerSetting;
	SvLog::LogSettings logSettings;
	SvHttp::HttpServerSettings httpSettings;
	SvAuth::AuthManagerSettings authSettings;
};
}
