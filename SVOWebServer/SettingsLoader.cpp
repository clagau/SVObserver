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

namespace SvOws
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

	RegGetStringIfExists(settings.websocketSettings.Host, "WebsocketHost");
	RegGetIntIfExists(settings.websocketSettings.Port, "WebsocketPort");
	RegGetIntIfExists(settings.websocketSettings.ReadBufferSize, "WebsocketReadBufferSize");
	RegGetIntIfExists(settings.websocketSettings.PingIntervalSec, "WebsocketPingIntervalSec");
	RegGetIntIfExists(settings.websocketSettings.PingTimeoutCount, "WebsocketPingTimeoutCount");
	RegGetIntIfExists(settings.websocketSettings.ConnectionCleanupIntervalSec, "WebsocketConnectionCleanupIntervalSec");
	RegGetIntIfExists(settings.websocketSettings.SplitLargePayload, "WebsocketSplitLargePayload");
}
} // namespace RRWS