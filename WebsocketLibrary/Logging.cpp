//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Logging.cpp
/// All Rights Reserved
//******************************************************************************
/// Logging utils for configuring the Boost.Log V2 logger.
//******************************************************************************

#include "stdafx.h"

#include "WebsocketLibrary/Logging.h"

#include <Windows.h>

#include <iostream>

// ptime and posix time_duration use different types (uint32 vs int64) for storing the hours
#pragma warning(push)
#pragma warning(disable : 4244)
#include <boost/log/support/date_time.hpp>
#pragma warning(pop)

#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using namespace boost::log;
using namespace boost::log::sinks;

namespace RRWS
{

namespace
{
/// There is already a Windows Event Log backend in Boost.Log, but it requires dynmiac linking
/// of the boost_log library (because it exports some log source details via its dll interface).
/// But we do not need this functionality, so we have this simple backend for writing logs to
/// the Windows Event Log.
class windows_event_log_backend
	: public basic_formatted_sink_backend<char, combine_requirements<synchronized_feeding>::type>
{
public:
	BOOST_LOG_API windows_event_log_backend(const std::string& source, trivial::severity_level level)
		: m_hdl(RegisterEventSource(NULL, source.c_str())), m_level(level)
	{
		if (!m_hdl)
		{
			auto err = get_last_error_as_string();
			auto msg = "Unable to register event source: " + err;
			throw std::runtime_error(msg.c_str());
		}
	}

	BOOST_LOG_API ~windows_event_log_backend()
	{
		DeregisterEventSource(m_hdl);
	}

	BOOST_LOG_API void consume(record_view const& rec, const std::string& formatted_record)
	{
		auto event_id = 1000;
		auto severity = extract_severity(rec);
		if (severity >= m_level)
		{
			auto type = map_to_type(severity);
			log_impl(m_hdl, event_id, type, formatted_record);
		}
	}

private:
	trivial::severity_level extract_severity(record_view const& rec)
	{
		auto default_severity = trivial::info;
		auto severity_attr = rec["Severity"];
		if (!severity_attr)
		{
			return default_severity;
		}
		return severity_attr.extract_or_default<trivial::severity_level, void, trivial::severity_level>(
			default_severity);
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

	bool log_impl(HANDLE hdl, DWORD eventId, WORD type, const std::string& message)
	{
		WORD category = 0;     // no category
		PSID user = NULL;      // use process' user
		DWORD binDataSize = 0; // no additional binary data provided
		LPVOID binData = NULL;
		const WORD numStrings = 1; // we just log the provided message
		LPCTSTR strings[numStrings];
		strings[0] = message.c_str();
		BOOL success = ReportEvent(hdl, type, category, eventId, user, numStrings, binDataSize, strings, binData);
		return success != FALSE;
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

void init_logging(const LogSettings& settings)
{
	auto log_level = trivial::info;
	if (!settings.log_level.empty())
	{
		if (!parse_severity(settings.log_level, log_level))
		{
			auto msg = "Unknown log level " + settings.log_level;
			throw std::runtime_error(msg.c_str());
		}
	}
	core::get()->set_filter(trivial::severity >= log_level);
	core::get()->remove_all_sinks(); // remove all default sinks
	if (settings.log_to_stdout_enabled)
	{
		typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
		boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
		boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
		sink->locked_backend()->add_stream(stream);
		add_common_attributes(); // adds common log attributes like timestamp
		sink->set_formatter(expressions::stream << expressions::format_date_time<boost::posix_time::ptime>(
			"TimeStamp", "%Y-%m-%dT%H:%M:%S.%f")
			<< " " << trivial::severity << " " << expressions::smessage);
		core::get()->add_sink(sink);
	}

	if (settings.windows_event_log_enabled)
	{
		if (settings.windows_event_log_source.empty())
		{
			throw std::runtime_error("You must provide a source name when enabling Windows EventLog");
		}
		auto event_log_level = trivial::info;
		if (!settings.windows_event_log_level.empty())
		{
			if (!parse_severity(settings.windows_event_log_level, event_log_level))
			{
				auto msg = "Unknown log level " + settings.windows_event_log_level;
				throw std::runtime_error(msg.c_str());
			}
		}
		auto backend =
			boost::make_shared<windows_event_log_backend>(settings.windows_event_log_source, event_log_level);
		core::get()->add_sink(boost::make_shared<synchronous_sink<windows_event_log_backend>>(backend));
	}
}

} // namespace RRWS