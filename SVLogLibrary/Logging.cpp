//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Logging.cpp
/// All Rights Reserved
//******************************************************************************
/// Logging utils for configuring the Boost.Log V2 logger.
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
#include <boost/function.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "Definitions/StringTypeDef.h"

#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVStatusLibrary/MessageManager.h"

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

/// Log backend that uses SVMessage and SVStatusLibrary for logging.
class sv_message_log_backend
	: public basic_formatted_sink_backend<char, combine_requirements<synchronized_feeding>::type>
{
public:
	BOOST_LOG_API sv_message_log_backend(severity_level MinSeverity)
		: m_MinSeverity(MinSeverity)
		, m_MessageManager(SvStl::LogOnly)
	{
	}

	BOOST_LOG_API void consume(record_view const& rec, const std::string& formatted_record)
	{
		auto severity = extract_severity(rec);
		if (severity >= m_MinSeverity)
		{
			const auto type = map_to_type(severity);
			const auto file = extract_file(rec);
			const auto line = extract_line(rec);
			log_impl(type, file, line, formatted_record);
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

	DWORD map_to_type(severity_level severity_level)
	{
		switch (severity_level)
		{
			case trivial::trace:
			case trivial::debug:
			case trivial::info:
				return SVMSG_SVGateway_2_GENERAL_INFORMATIONAL;

			case trivial::warning:
				return SVMSG_SVGateway_1_GENERAL_WARNING;

			case trivial::error:
			case trivial::fatal:
				return SVMSG_SVGateway_0_GENERAL_ERROR;

			default:
				return SVMSG_SVGateway_2_GENERAL_INFORMATIONAL;
		}
	}

	void log_impl(DWORD type, const std::string& file, int line, const std::string& Message)
	{
		SvDef::StringVector MessageParams;
		MessageParams.push_back(Message);
		SvStl::SourceFileParams SourceFileParams(StdMessageParams);
		SourceFileParams.m_FileName = file;
		SourceFileParams.m_Line = line;
		m_MessageManager.setMessage(type, SvStl::Tid_Default, MessageParams, SourceFileParams);
	}

private:
	trivial::severity_level m_MinSeverity;
	SvStl::MessageMgrStd m_MessageManager;
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
		sink->set_formatter(&log_formatter);
		core::get()->add_sink(sink);
	}

	if (settings.windows_event_log_enabled)
	{
		auto event_log_level = trivial::info;
		if (!settings.windows_event_log_level.empty())
		{
			if (!parse_severity(settings.windows_event_log_level, event_log_level))
			{
				auto msg = "Unknown log level " + settings.windows_event_log_level;
				throw std::runtime_error(msg.c_str());
			}
		}
		auto backend = boost::make_shared<sv_message_log_backend>(event_log_level);
		core::get()->add_sink(boost::make_shared<synchronous_sink<sv_message_log_backend>>(backend));
	}
}

} // namespace SvLog
