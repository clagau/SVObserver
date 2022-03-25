//******************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Settings.h
/// All Rights Reserved
//******************************************************************************
/// All server settings in one place.
//******************************************************************************

#pragma once

#include "SVAuthLibrary/AuthManagerSettings.h"
#include "SVLogLibrary/LogSettings.h"

namespace SvAuth
{
struct WebAppSettings
{
	std::string chrome = "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
	std::string user;
	std::string host;
	std::string protocol = "http";
	std::string locale = "de";
	bool hideTopNavigation = false;
	bool hideBottomNavigation = false;
};

struct Settings
{
	Settings()
	{
		logSettings.FileLogEnabled = false;
		logSettings.StdoutLogEnabled = true;
		logSettings.StdoutLogLevel = "info";
		logSettings.WindowsEventLogEnabled = false;
		authSettings.AllowUnauthorizedRpcClients = true;
	}
	SvLog::LogSettings logSettings;
	SvAuth::AuthManagerSettings authSettings;
	WebAppSettings webAppSettings;
};
}
