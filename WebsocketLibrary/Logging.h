//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Logging.h
/// All Rights Reserved
//******************************************************************************
/// Helper for initializing the global Boost.Log V2 logger.
///
/// When initialized, you can use the common boost log macros, e.g.
///   BOOST_LOG_TRIVIAL(info) << "hello world";
//******************************************************************************

#pragma once

namespace RRWS
{
struct LogSettings
{
	/// Minimum log level that is allowed to log. All other levels will be
	/// filtered. Allowed log levels are (from higher to lower):
	/// fatal, error, warning, info, debug, trace
	std::string log_level {"debug"};

	/// Set to true if you want the logs also sent to stdout.
	/// Log format is as follows: $iso_8601_date_time $log_level $message
	bool log_to_stdout_enabled {true};

	/// Set to true if you want the logs sent to the Windows Event Logs.
	/// See other windows_event_log_* settings for detailed configuration.
	/// Log levels are mapped as follows:
	///   [fatal, error]       -> EVENTLOG_ERROR_TYPE
	///   [warning]            -> EVENTLOG_WARNING_TYPE
	///   [info, debug, trace] -> EVENTLOG_INFORMATION_TYPE
	bool windows_event_log_enabled {false};

	/// Same as log_level but just for the Windows Event Log. Set this to
	/// a higher level to see e.g. info logs on stdout and send error and
	/// warning logs to the Event Log.
	std::string windows_event_log_level {"info"};

	/// The source used for the Event Log events.
	std::string windows_event_log_source {""};
};

extern void init_logging(const LogSettings& settings);
} // namespace RRWS