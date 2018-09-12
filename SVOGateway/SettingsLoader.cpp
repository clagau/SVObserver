//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SettingsLoader.cpp
/// All Rights Reserved
//******************************************************************************
/// Read settings from the Windows Registry. It does not use the raw Windows
/// API functions for access the registry, but uses some C++ wrapper you can
/// find the Registry.h file.
//******************************************************************************

#include "stdafx.h"

#include <algorithm>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "SVStatusLibrary/GlobalPath.h"
#include "SVLogLibrary/Logging.h"

#include "SettingsLoader.h"

namespace SvOgw
{
static void RegGetStringIfExists(const boost::property_tree::ptree& pt, std::string& dst, const std::string& path)
{
	const auto v = pt.get_optional<std::string>(path);
	if (v)
	{
		dst = *v;
	}
}

static void RegGetPathIfExists(const boost::property_tree::ptree& pt, std::experimental::filesystem::path& dst, const std::string& path)
{
	const auto v = pt.get_optional<std::string>(path);
	if (v)
	{
		dst = std::experimental::filesystem::path(*v);
	}
}

template <typename T> static void RegGetIntIfExists(const boost::property_tree::ptree& pt, T& dst, const std::string& path)
{
	const auto v = pt.get_optional<T>(path);
	if (v)
	{
		dst = *v;
	}
}

static void RegGetBoolIfExists(const boost::property_tree::ptree& pt, bool& dst, const std::string& path)
{

	const auto v = pt.get_optional<std::string>(path);
	if (v)
	{
		std::string str = *v;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		dst = (str == "1" || str == "true");
	}
}

void SettingsLoader::loadFromIni(Settings& settings)
{
	boost::property_tree::ptree pt;

	const auto path = SvStl::GlobalPath::Inst().GetSVOGatewayIniPath();
	try
	{
		boost::property_tree::ini_parser::read_ini(path, pt);
	}
	catch (...)
	{
		SV_LOG_GLOBAL(warning) << "The ini file at " << path << " does not exists. Using default settings.";
		return;
	}

	RegGetIntIfExists(pt, settings.dummySharedMemory, "SharedMemory.Dummy");

	RegGetStringIfExists(pt, settings.logSettings.log_level, "Logger.LogLevel");
	RegGetIntIfExists(pt, settings.logSettings.log_to_stdout_enabled, "Logger.LogToStdoutEnabled");
	RegGetIntIfExists(pt, settings.logSettings.windows_event_log_enabled, "Logger.WindowsEventLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.windows_event_log_source, "Logger.WindowsEventLogSource");
	RegGetStringIfExists(pt, settings.logSettings.windows_event_log_level, "Logger.WindowsEventLogLevel");

	RegGetStringIfExists(pt, settings.httpSettings.Host, "Http.Host");
	RegGetIntIfExists(pt, settings.httpSettings.Port, "Http.Port");
	RegGetIntIfExists(pt, settings.httpSettings.ReadBufferSize, "Http.ReadBufferSize");
	RegGetIntIfExists(pt, settings.httpSettings.WriteBufferSize, "Http.WriteBufferSize");
	RegGetIntIfExists(pt, settings.httpSettings.ConnectionCleanupIntervalSec, "Http.ConnectionCleanupIntervalSec");
	RegGetIntIfExists(pt, settings.httpSettings.bEnableFileServing, "Http.EnableFileServing");
	RegGetPathIfExists(pt, settings.httpSettings.DataDir, "Http.DataDir");
	RegGetStringIfExists(pt, settings.httpSettings.DefaultIndexHtmlFile, "Http.DefaultIndexHtmlFile");
	RegGetStringIfExists(pt, settings.httpSettings.DefaultErrorHtmlFile, "Http.DefaultErrorHtmlFile");

	RegGetStringIfExists(pt, settings.authSettings.UserSettingsFile, "Auth.UserSettingsFile");
	RegGetStringIfExists(pt, settings.authSettings.JwtHmacSecret, "Auth.JwtHmacSecret");
	RegGetStringIfExists(pt, settings.authSettings.JwtRsaPublicKeyFile, "Auth.JwtRsaPublicKeyFile");
	RegGetStringIfExists(pt, settings.authSettings.JwtRsaPrivateKeyFile, "Auth.JwtRsaPrivateKeyFile");
}
}// namespace SvOgw
