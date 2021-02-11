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
#include "SVHttpLibrary/WebsocketClientSettings.h"
#include "SVSharedMemoryLibrary/ShareControlSetting.h"

namespace SvOgw
{
struct Settings
{
	Settings()
	{
		observerSetting.Port = SvHttp::Default_SecondPort;
		observerSetting.Host = "127.0.0.1";
		logSettings.FileLogEnabled = false;
		logSettings.StdoutLogEnabled = false;
		logSettings.WindowsEventLogEnabled = true;
		logSettings.WindowsEventLogLevel = "info";
		logSettings.eventLogFacility = SvLog::EventLogFacility::Gateway;
		authSettings.AllowUnauthorizedRpcClients = true;
	}
	bool skipPermissionChecks{ false };
	SvHttp::WebsocketClientSettings observerSetting;
	SvLog::LogSettings logSettings;
	SvHttp::HttpServerSettings httpSettings;
	SvAuth::AuthManagerSettings authSettings;
	SvSml::ShareControlSettings shareControlSettings;
};
}
