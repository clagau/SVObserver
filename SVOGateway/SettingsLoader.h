//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SettingsLoader.h
/// All Rights Reserved
//******************************************************************************
/// This class respects the default settings and only overwrites the values that
/// are configured in the Windows Registry. See below for possible options.
///
/// Registry key:
///   HKLM/Software/Seidenader/WebsocketServer
///
/// And values mapped as follows:
///   DummySharedMemory = dummySharedMemory
///   LogLevel = logSettings.log_level
///   LogToStdoutEnabled = logSettings.log_to_stdout_enabled
///   EventLogEnabled = logSettings.windows_event_log_enabled
///   EventLogSource = logSettings.windows_event_log_source
///   EventLogLevel = logSettings.windows_event_log_level
///   WebsocketHost = websocketSettings.host
///   WebsocketPort = websocketSettings.port
///   WebsocketReadBufferSize = websocketSettings.read_buffer_size
///   WebsocketWriteBufferSize = websocketSettings.write_buffer_size
///   WebsocketPingIntervalSec = websocketSettings.ping_interval_sec
///   WebsocketPingTimeoutCount = websocketSettings.ping_timeout_count
///   WebsocketConnectionCleanupIntervalSec =
///     websocketSettings.connection_cleanup_interval_sec
//******************************************************************************

#pragma once

#include "Settings.h"

namespace SvOgw
{
class SettingsLoader
{
public:
	void loadFromRegistry(Settings&);
};
}// namespace SvOgw
