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

#include "Registry.h"
#include "SettingsLoader.h"

#define RRWS_KEY HKEY_LOCAL_MACHINE
#define RRWS_SUBKEY "Software\\Seidenader\\WebsocketServer\\"

namespace SvOgw
{
static void RegGetStringIfExists(std::string& dst, const std::string& name)
{
	try
	{
		auto v = win32::RegGetString(RRWS_KEY, _T(RRWS_SUBKEY), name);
		dst = v;
	}
	catch (const win32::RegistryError&)
	{
	}
}
static void RegGetPathIfExists(std::experimental::filesystem::path& dst, const std::string& name)
{
	try
	{
		auto v = win32::RegGetString(RRWS_KEY, _T(RRWS_SUBKEY), name);
		dst = std::experimental::filesystem::path(v);
	}
	catch (const win32::RegistryError&)
	{
	}
}
template <typename T> static void RegGetIntIfExists(T& dst, const std::string& name)
{
	try
	{
		auto v = win32::RegGetDword(RRWS_KEY, _T(RRWS_SUBKEY), name);
		dst = static_cast<T>(v);
	}
	catch (const win32::RegistryError&)
	{
	}
}
template <> static void RegGetIntIfExists(bool& dst, const std::string& name)
{
	try
	{
		auto v = win32::RegGetDword(RRWS_KEY, _T(RRWS_SUBKEY), name);
		dst = v != 0;
	}
	catch (const win32::RegistryError&)
	{
	}
}
void SettingsLoader::loadFromRegistry(Settings& settings)
{
	try
	{
		win32::RegOpenKey(RRWS_KEY, RRWS_SUBKEY, KEY_READ);
	}
	catch (const win32::RegistryError&)
	{
		return; // registry key does not exist. stick to defaults.
	}

	RegGetIntIfExists(settings.dummySharedMemory, "DummySharedMemory");

	RegGetStringIfExists(settings.logSettings.log_level, "LogLevel");
	RegGetIntIfExists(settings.logSettings.log_to_stdout_enabled, "LogToStdoutEnabled");
	RegGetIntIfExists(settings.logSettings.windows_event_log_enabled, "EventLogEnabled");
	RegGetStringIfExists(settings.logSettings.windows_event_log_source, "EventLogSource");
	RegGetStringIfExists(settings.logSettings.windows_event_log_level, "EventLogLevel");

	RegGetStringIfExists(settings.httpSettings.Host, "WebsocketHost");
	RegGetIntIfExists(settings.httpSettings.Port, "WebsocketPort");
	RegGetIntIfExists(settings.httpSettings.ReadBufferSize, "WebsocketReadBufferSize");
	RegGetIntIfExists(settings.httpSettings.WriteBufferSize, "WebsocketWriteBufferSize");
	RegGetIntIfExists(settings.httpSettings.ConnectionCleanupIntervalSec, "WebsocketConnectionCleanupIntervalSec");
	RegGetIntIfExists(settings.httpSettings.bEnableFileServing, "WebsocketEnableFileServing");
	RegGetPathIfExists(settings.httpSettings.DataDir, "WebsocketDataDir");
	RegGetStringIfExists(settings.httpSettings.DefaultIndexHtmlFile, "WebsocketDefaultIndexHtmlFile");
	RegGetStringIfExists(settings.httpSettings.DefaultErrorHtmlFile, "WebsocketDefaultErrorHtmlFile");
}
}// namespace SvOgw
