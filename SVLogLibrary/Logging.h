//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Logging.h
/// All Rights Reserved
//******************************************************************************
/// Helper for initializing the global Boost.Log V2 logger.
///
/// When initialized, you can use this macro to via the global logger
///   SV_LOG_GLOBAL(info) << "hello world";
//******************************************************************************

#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include <SVLogLibrary/LogSettings.h>

namespace SvLog
{

#define SV_LOG_GLOBAL(sev) \
	BOOST_LOG_SEV(::boost::log::trivial::logger::get(), ::boost::log::trivial::sev) \
		<< ::boost::log::add_value("Line", __LINE__)      \
		<< ::boost::log::add_value("File", __FILE__)       \
		<< ::boost::log::add_value("Function", BOOST_CURRENT_FUNCTION)

// bootstrap logging as the very first step you have minimal logging
// while loading settings and settings up the logger itself.
extern void bootstrap_logging();

extern void init_logging(const LogSettings& settings);

} // namespace SvLog

#include <SVLogLibrary/Logging.inl>
