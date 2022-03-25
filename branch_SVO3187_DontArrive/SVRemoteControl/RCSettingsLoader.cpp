//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file WebSocketSettingsLoader.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "stdafx.h"

#include <algorithm>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "SVStatusLibrary/GlobalPath.h"
#include "SVLogLibrary/Logging.h"

#include "RCSettingsLoader.h"
#include <filesystem>




static void RegGetStringIfExists(const boost::property_tree::ptree& pt, std::string& dst, const std::string& path)
{
	const auto v = pt.get_optional<std::string>(path);
	if (v)
	{
		dst = *v;
	}
}

static void RegGetPathIfExists(const boost::property_tree::ptree& pt, std::filesystem::path& dst, const std::string& path)
{
	const auto v = pt.get_optional<std::string>(path);
	if (v)
	{
		dst = std::filesystem::path(*v);
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
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return static_cast<char> (std::tolower(c)); });
		dst = (str == "1" || str == "true");
	}
}

void RCSettingsLoader::loadFromIni(LPCSTR IniPath, RCSettings& settings)
{
	
	
	boost::property_tree::ptree pt;

	try
	{
		boost::property_tree::ini_parser::read_ini(IniPath, pt);
	}
	catch (boost::property_tree::ini_parser_error &e)
	{
		SV_LOG_GLOBAL(warning) << "The ini file at " << e.filename() << " could not be read.  Reason : " << e.message() << "Line: " << e.line() << "Using default settings. ";
		return;
	}
	catch (...)
	{
		SV_LOG_GLOBAL(warning) << "The ini file at " << IniPath << " does not exists. Using default settings.";
		return;
	}


	
	
	RegGetBoolIfExists(pt, settings.logSettings.StdoutLogEnabled, "Logger.StdoutLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.StdoutLogLevel, "Logger.StdoutLogLevel");
	RegGetBoolIfExists(pt, settings.logSettings.FileLogEnabled, "Logger.FileLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.FileLogLevel, "Logger.FileLogLevel");
	RegGetStringIfExists(pt, settings.logSettings.FileLogLocation, "Logger.FileLogLocation");
	RegGetBoolIfExists(pt, settings.logSettings.WindowsEventLogEnabled, "Logger.WindowsEventLogEnabled");
	RegGetStringIfExists(pt, settings.logSettings.WindowsEventLogLevel, "Logger.WindowsEventLogLevel");



	RegGetStringIfExists(pt, settings.logSettings.WindowsEventLogLevel, "Logger.WindowsEventLogLevel");

	RegGetStringIfExists(pt, settings.httpClientSettings.Host, "Http.Host");
	RegGetIntIfExists(pt, settings.httpClientSettings.Port, "Http.Port");
	RegGetIntIfExists(pt, settings.httpClientSettings.ReadBufferSize, "Http.ReadBufferSize");
	RegGetIntIfExists(pt, settings.httpClientSettings.WriteBufferSize, "Http.WriteBufferSize");
	RegGetIntIfExists(pt, settings.httpClientSettings.MaxMessageSize, "Http.MaxMessageSize");
	RegGetIntIfExists(pt, settings.httpClientSettings.PingIntervalSec, "Http.PingIntervalSec");
	RegGetIntIfExists(pt, settings.httpClientSettings.PingTimeoutCount, "Http.PingTimeoutCount");

	
	unsigned int shortTimeOut {0}, mediumTimeOut {0}, longTimeout {0};
	RegGetIntIfExists(pt, shortTimeOut, "SVRC.ShortTimeoutSec");
	if (shortTimeOut > 0)
	{
		settings.svrcClientSettings.ShortTimeout = boost::posix_time::seconds(shortTimeOut);
	}
	RegGetIntIfExists(pt, mediumTimeOut, "SVRC.MediumTimeoutSec");
	if (mediumTimeOut > 0)
	{
		settings.svrcClientSettings.MediumTimeout = boost::posix_time::seconds(mediumTimeOut);
	}
	RegGetIntIfExists(pt, longTimeout, "SVRC.LongTimeoutSec");
	if (longTimeout > 0)
	{
		settings.svrcClientSettings.LongTimeout = boost::posix_time::seconds(longTimeout);
	}
}