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
	catch (boost::property_tree::ini_parser_error &e)
	{
		SV_LOG_GLOBAL(warning) << "The ini file at " << e.filename() << " could not be read.  Reason : " << e.message() << "Line: " << e.line() << "Using default settings. ";
		return;
	}

	catch (...)
	{
		SV_LOG_GLOBAL(warning) << "The ini file at " << path << " does not exists. Using default settings.";
		return;
	}

	RegGetIntIfExists(pt, settings.observerSetting.Port, "Observer.Port");
	RegGetStringIfExists(pt, settings.observerSetting.Host, "Observer.Host");
	RegGetIntIfExists(pt, settings.observerSetting.MaxMessageSize, "Observer.MaxMessagesSize");
	RegGetIntIfExists(pt, settings.observerSetting.ReadBufferSize, "Observer.ReadBufferSize");
	RegGetIntIfExists(pt, settings.observerSetting.WriteBufferSize, "Observer.WriteBufferSize");
	RegGetIntIfExists(pt, settings.observerSetting.PingIntervalSec, "Observer.PingIntervalSec");
	RegGetIntIfExists(pt, settings.observerSetting.PingTimeoutCount, "Observer.PingTimeoutCount");


	RegGetBoolIfExists(pt, settings.dummySharedMemory, "SharedMemory.Dummy");
	RegGetBoolIfExists(pt, settings.logSettings.StdoutLogEnabled, "Logger.StdoutLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.StdoutLogLevel, "Logger.StdoutLogLevel");
	RegGetBoolIfExists(pt, settings.logSettings.FileLogEnabled, "Logger.FileLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.FileLogLevel, "Logger.FileLogLevel");
	RegGetStringIfExists(pt, settings.logSettings.FileLogLocation, "Logger.FileLogLocation");
	RegGetBoolIfExists(pt, settings.logSettings.WindowsEventLogEnabled, "Logger.WindowsEventLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.WindowsEventLogLevel, "Logger.WindowsEventLogLevel");

	
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
	RegGetBoolIfExists(pt, settings.authSettings.AllowUnauthorizedRpcClients, "Auth.AllowUnauthorizedRpcClients");


	m_iniPath = path;
}

std::string  SettingsLoader::GetIni()
{
	return m_iniPath;
}
}