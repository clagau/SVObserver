//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RCSettingsLoader.cpp
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//*****************************************************************************
#include "stdafx.h"

#include "RCSettingsLoader.h"
#include "RCSettings.h"
#include "SVLogLibrary/Logging.h"


static void RegGetStringIfExists(const boost::property_tree::ptree& pt, std::string& dst, const std::string& path)
{
	const auto v = pt.get_optional<std::string>(path);
	if (v)
	{
		dst = *v;
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

namespace SvRc
{
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


	
	
	RegGetBoolIfExists(pt, settings.m_logSettings.StdoutLogEnabled, "Logger.StdoutLogEnabled");
	RegGetStringIfExists(pt, settings.m_logSettings.StdoutLogLevel, "Logger.StdoutLogLevel");
	RegGetBoolIfExists(pt, settings.m_logSettings.FileLogEnabled, "Logger.FileLogEnabled");
	RegGetStringIfExists(pt, settings.m_logSettings.FileLogLevel, "Logger.FileLogLevel");
	RegGetStringIfExists(pt, settings.m_logSettings.FileLogLocation, "Logger.FileLogLocation");
	RegGetBoolIfExists(pt, settings.m_logSettings.WindowsEventLogEnabled, "Logger.WindowsEventLogEnabled");
	RegGetStringIfExists(pt, settings.m_logSettings.WindowsEventLogLevel, "Logger.WindowsEventLogLevel");



	RegGetStringIfExists(pt, settings.m_logSettings.WindowsEventLogLevel, "Logger.WindowsEventLogLevel");

	RegGetStringIfExists(pt, settings.m_httpClientSettings.Host, "Http.Host");
	RegGetIntIfExists(pt, settings.m_httpClientSettings.Port, "Http.Port");
	RegGetIntIfExists(pt, settings.m_httpClientSettings.ReadBufferSize, "Http.ReadBufferSize");
	RegGetIntIfExists(pt, settings.m_httpClientSettings.WriteBufferSize, "Http.WriteBufferSize");
	RegGetIntIfExists(pt, settings.m_httpClientSettings.MaxMessageSize, "Http.MaxMessageSize");
	RegGetIntIfExists(pt, settings.m_httpClientSettings.PingIntervalSec, "Http.PingIntervalSec");
	RegGetIntIfExists(pt, settings.m_httpClientSettings.PingTimeoutCount, "Http.PingTimeoutCount");

	
	unsigned int shortTimeOut {0}, mediumTimeOut {0}, longTimeout {0};
	RegGetIntIfExists(pt, shortTimeOut, "SVRC.ShortTimeoutSec");
	if (shortTimeOut > 0)
	{
		settings.m_svrcClientSettings.ShortTimeout = boost::posix_time::seconds(shortTimeOut);
	}
	RegGetIntIfExists(pt, mediumTimeOut, "SVRC.MediumTimeoutSec");
	if (mediumTimeOut > 0)
	{
		settings.m_svrcClientSettings.MediumTimeout = boost::posix_time::seconds(mediumTimeOut);
	}
	RegGetIntIfExists(pt, longTimeout, "SVRC.LongTimeoutSec");
	if (longTimeout > 0)
	{
		settings.m_svrcClientSettings.LongTimeout = boost::posix_time::seconds(longTimeout);
	}
}
} //namespace SvRc
