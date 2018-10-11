//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Logging.cpp
/// All Rights Reserved
//******************************************************************************
/// Logging utils for configuring the Boost.Log V2 logger.
//Backend to write Entries to WindowEventLog using Entries from SVMessage.dll
//******************************************************************************

#include "stdafx.h"
#include "Logging.h"

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <comdef.h>
#include <tchar.h>

// ptime and posix time_duration use different types (uint32 vs int64) for storing the hours
#pragma warning(push)
#pragma warning(disable : 4244)
#include <boost/log/support/date_time.hpp>
#pragma warning(pop)

#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "Definitions/StringTypeDef.h"
#include "SVMessage/SVMessage.h"

using namespace boost::log;
using namespace boost::log::sinks;
using boost::log::trivial::severity_level;

namespace SvLog
{
namespace
{

static std::string extract_file(record_view const& rec)
{
	static const auto default_file = "<unknown_file>";
	auto file_attr = rec["File"];
	if (!file_attr)
	{
		return default_file;
	}

	const auto fullpath = file_attr.extract_or_default<std::string, void, std::string>(default_file);
	if (fullpath == default_file)
	{
		return default_file;
	}

	return boost::filesystem::path(fullpath).filename().string();
}

static int extract_line(record_view const& rec)
{
	static const auto default_line = 0;
	auto line_attr = rec["Line"];
	if (!line_attr)
	{
		return default_line;
	}
	return line_attr.extract_or_default<int, void, int>(default_line);
}
/// Log backend that uses SVMessage but not  SVStatusLibrary for logging to 
//Windows EventLog
class windows_event_log_backend
	: public basic_formatted_sink_backend<char, combine_requirements<synchronized_feeding>::type>
{
public:
	BOOST_LOG_API windows_event_log_backend(const std::string& source, SvLog::EventLogFacility facility, severity_level MinSeverity)
		: m_hdl(RegisterEventSource(NULL, source.c_str())), m_level(MinSeverity)
	{
		if (!m_hdl)
		{
			auto err = get_last_error_as_string();
			auto msg = "Unable to register event source: " + err;
			SV_LOG_GLOBAL(error) << msg;
		}
		switch (facility)
		{
			case SvLog::EventLogFacility::Gateway:
				m_MessageInfo = SVMSG_SVGateway_2_GENERAL_INFORMATIONAL;
				m_MessageWarning = SVMSG_SVGateway_1_GENERAL_WARNING;
				m_MessageError = SVMSG_SVGateway_0_GENERAL_ERROR;
				break;
			case SvLog::EventLogFacility::RemoteCtrl:
				m_MessageInfo = SVMSG_REMOTECTRL_2_GENERAL_INFORMATIONAL;
				m_MessageWarning = SVMSG_REMOTECTRL_1_GENERAL_WARNING;
				m_MessageError = SVMSG_REMOTECTRL_0_GENERAL_ERROR;
				break;
			case SvLog::EventLogFacility::Websocket:
				m_MessageInfo = SVMSG_SVO_2_GENERAL_INFORMATIONAL;
				m_MessageWarning = SVMSG_SVO_1_GENERAL_WARNING;
				m_MessageError = SVMSG_SVO_0_GENERAL_ERROR;
				break;
			default:
				m_MessageInfo = SVMSG_SVGateway_2_GENERAL_INFORMATIONAL;
				m_MessageWarning = SVMSG_SVGateway_1_GENERAL_WARNING;
				m_MessageError = SVMSG_SVGateway_0_GENERAL_ERROR;
				break;
		}

	}

	BOOST_LOG_API ~windows_event_log_backend()
	{
		DeregisterEventSource(m_hdl);
	}

	BOOST_LOG_API void consume(record_view const& rec, const std::string& msg)
	{

		auto severity = extract_severity(rec);
		if (severity >= m_level)
		{
			const auto EventId = map_to_EventId(severity);
			auto type = map_to_type(severity);
			const auto file = extract_file(rec);
			const auto line = extract_line(rec);
			log_impl(EventId, type, file, line, msg);
		}
	}

private:
	severity_level extract_severity(record_view const& rec)
	{
		auto default_severity = trivial::info;
		auto severity_attr = rec["Severity"];
		if (!severity_attr)
		{
			return default_severity;
		}
		return severity_attr.extract_or_default<severity_level, void, severity_level>(default_severity);
	}

	DWORD map_to_EventId(severity_level severity_level)
	{
		switch (severity_level)
		{
			case trivial::trace:
			case trivial::debug:
			case trivial::info:
				return m_MessageInfo;
			case trivial::warning:
				return  m_MessageWarning;
			case trivial::error:
			case trivial::fatal:
				return m_MessageError;
			default:
				return m_MessageInfo;
		}
	}

	WORD map_to_type(trivial::severity_level severity_level)
	{
		switch (severity_level)
		{
			case trivial::trace:
			case trivial::debug:
			case trivial::info:
				return EVENTLOG_INFORMATION_TYPE;

			case trivial::warning:
				return EVENTLOG_WARNING_TYPE;

			case trivial::error:
			case trivial::fatal:
				return EVENTLOG_ERROR_TYPE;

			default:
				return EVENTLOG_INFORMATION_TYPE;
		}
	}

	WORD Event2Category(DWORD EventId)
	{
		return ((EventId & 0x0fff0000) >> 16) & 0x00ff;
	}

	bool log_impl(DWORD eventId, WORD type, const std::string& file, int line, const std::string& Message)
	{
		PSID user = NULL;      // use process' user
		DWORD binDataSize = 0; // no additional binary data provided
		LPVOID binData = NULL;
		std::string lineStr;
		lineStr = boost::str(boost::format("%d") % line);
		const WORD numStrings = 9;
		LPCTSTR strings[9] = {"\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
		strings[1] = file.c_str();
		strings[2] = lineStr.c_str();
		strings[8] = Message.c_str();
		return (TRUE == ReportEvent(m_hdl, type, Event2Category(eventId), eventId, user, numStrings, binDataSize, strings, binData));
	}

	std::string get_last_error_as_string()
	{
		DWORD errorMessageID = ::GetLastError();
		if (errorMessageID == 0)
		{
			return std::string();
		}

		DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
		DWORD languageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
		LPSTR messageBuffer = NULL;
		size_t size = FormatMessageA(flags, NULL, errorMessageID, languageId, (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);
		return message;
	}

private:
	DWORD m_MessageInfo {SVMSG_SVGateway_2_GENERAL_INFORMATIONAL};
	DWORD m_MessageWarning {SVMSG_SVGateway_1_GENERAL_WARNING};
	DWORD m_MessageError {SVMSG_SVGateway_0_GENERAL_ERROR};
	trivial::severity_level m_level;
	HANDLE m_hdl;
};
}

static std::map<std::string, trivial::severity_level> severity_level_name_map()
{
	std::map<std::string, trivial::severity_level> map;
	map[trivial::to_string(trivial::trace)] = trivial::trace;
	map[trivial::to_string(trivial::debug)] = trivial::debug;
	map[trivial::to_string(trivial::info)] = trivial::info;
	map[trivial::to_string(trivial::warning)] = trivial::warning;
	map[trivial::to_string(trivial::error)] = trivial::error;
	map[trivial::to_string(trivial::fatal)] = trivial::fatal;
	return map;
}

static bool parse_severity(const std::string& str, trivial::severity_level& level)
{
	static auto map = severity_level_name_map();
	auto it = map.find(str);
	if (it == map.end())
	{
		return false;
	}

	level = it->second;
	return true;
}

void log_formatter(record_view const& rec, formatting_ostream& strm)
{
	strm
		<< expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%dT%H:%M:%S.%f")(rec)
		<< " "
		<< rec[trivial::severity]
		<< " "
		<< rec[expressions::smessage];
}

void bootstrap_logging()
{
	core::get()->set_filter(trivial::severity >= trivial::debug);
	core::get()->remove_all_sinks(); // remove all default sinks

	typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
	boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
	sink->locked_backend()->add_stream(stream);
	add_common_attributes(); // adds common log attributes like timestamp
	sink->set_formatter(&log_formatter);
	core::get()->add_sink(sink);
}

void init_logging(const LogSettings& settings)
{
	core::get()->remove_all_sinks(); // remove all default sinks
	add_common_attributes(); // adds common log attributes like timestamp
	auto min_LogLevel = trivial::info;
	if (settings.StdoutLogEnabled)
	{
		auto stdout_log_level = trivial::info;
		if (!settings.StdoutLogLevel.empty())
		{
			if (!parse_severity(settings.StdoutLogLevel, stdout_log_level))
			{
				auto msg = "Unknown stdout log level " + settings.StdoutLogLevel;
				throw std::runtime_error(msg.c_str());
			}
		}

		auto sink = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();
		auto stream = boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter());
		sink->locked_backend()->add_stream(stream);
		sink->set_formatter(&log_formatter);
		sink->set_filter(trivial::severity >= stdout_log_level);
		core::get()->add_sink(sink);
		min_LogLevel = min_LogLevel < stdout_log_level ? min_LogLevel : stdout_log_level;
	}

	if (settings.FileLogEnabled && !settings.FileLogLocation.empty())
	{
		auto file_log_level = trivial::info;
		if (!settings.FileLogLevel.empty())
		{
			if (!parse_severity(settings.FileLogLevel, file_log_level))
			{
				auto msg = "Unknown file log level " + settings.FileLogLevel;
				throw std::runtime_error(msg.c_str());
			}
		}

		auto sink = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();
		auto stream = boost::shared_ptr<std::ostream>(new std::ofstream(settings.FileLogLocation.c_str()));
		sink->locked_backend()->add_stream(stream);
		sink->locked_backend()->auto_flush(true);
		sink->set_formatter(&log_formatter);
		sink->set_filter(trivial::severity >= file_log_level);
		core::get()->add_sink(sink);
		min_LogLevel = min_LogLevel < file_log_level ? min_LogLevel : file_log_level;
	}

	if (settings.WindowsEventLogEnabled)
	{
		auto event_log_level = trivial::info;
		if (!settings.WindowsEventLogLevel.empty())
		{
			if (!parse_severity(settings.WindowsEventLogLevel, event_log_level))
			{
				auto msg = "Unknown event log level " + settings.WindowsEventLogLevel;
				throw std::runtime_error(msg.c_str());
			}
		}
		auto facility = settings.eventLogFacility;

		auto backend = boost::make_shared<windows_event_log_backend>("SVException", facility, event_log_level);
		auto sink = boost::make_shared<synchronous_sink<windows_event_log_backend>>(backend);
		core::get()->add_sink(sink);
		min_LogLevel = min_LogLevel < event_log_level ? min_LogLevel : event_log_level;
	}
	core::get()->set_filter(trivial::severity >= min_LogLevel);
}

} // namespace SvLog
