//******************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file LogSettings.h
/// All Rights Reserved
//******************************************************************************
/// Settings for the global logger
//******************************************************************************

#pragma once

#include <string>

namespace SvLog
{

enum class EventLogFacility
{
	Gateway, RemoteCtrl, Websocket
};

struct LogSettings
{
	/// Set to true if you want the logs sent to stdout.
	/// Log format is as follows: $iso_8601_date_time $log_level $message
	bool StdoutLogEnabled {true};

	/// Minimum log level that is allowed to log. All other levels will be
	/// filtered. Allowed log levels are (from higher to lower):
	/// fatal, error, warning, info, debug, trace
	std::string StdoutLogLevel {"error"};

	/// Set to true if you want logs written to a file on disk.
	/// Make sure to also configure file_log_location!
	bool FileLogEnabled {false};

	/// Minimum log level for messages written to the persistent log file.
	/// See stdout_log_level for available log levels.
	std::string FileLogLevel {"debug"};

	/// Absolute path to the persistent log file. Make sure it is writable.
	std::string FileLogLocation {""};

	/// Set to true if you want the logs sent to the Windows Event Logs.
	/// See other windows_event_log_* settings for detailed configuration.
	/// Log levels are mapped as follows:
	///   [fatal, error]       -> EVENTLOG_ERROR_TYPE
	///   [warning]            -> EVENTLOG_WARNING_TYPE
	///   [info, debug, trace] -> EVENTLOG_INFORMATION_TYPE
	bool WindowsEventLogEnabled {false};

	/// Same as stdout_log_level but just for the Windows Event Log. Set
	/// this to a higher level to see e.g. info logs on stdout and send
	/// error and warning logs to the Event Log.
	/// See stdout_log_level for available log levels.
	std::string WindowsEventLogLevel {"info"};

	///Set the facility for the eventlog
	EventLogFacility  eventLogFacility {EventLogFacility::Gateway};
};
} // namespace SvLog
  