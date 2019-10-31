//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file WebSocketSettingsLoader.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Log and http Settings are loaded  from SVObserverIni with WebsocketSettingsLoader 
//******************************************************************************
#include "stdafx.h"

#include <algorithm>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "SVStatusLibrary/GlobalPath.h"
#include "SVLogLibrary/Logging.h"

#include "WebsocketSettings.h"
#include "WebSocketSettingsLoader.h"


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

void WebSocketSettingsLoader::loadFromIni(WebSocketSettings& settings)
{
	boost::property_tree::ptree pt;
	const auto path = SvStl::GlobalPath::Inst().GetSVObserverIniPath();
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

	RegGetIntIfExists(pt, settings.shareControlSettings.DelayBeforeCreateShare, "ShareControl.DelayBeforeCreateShareInMilliSeconds");
	RegGetIntIfExists(pt, settings.shareControlSettings.DelayBeforeClearShare, "ShareControl.DelayBeforeClearShareInMilliSeconds");
	m_iniPath = path;
}

std::string  WebSocketSettingsLoader::GetIni()
{
	return m_iniPath;
}