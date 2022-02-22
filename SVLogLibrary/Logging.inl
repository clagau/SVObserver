//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Logging.inl
/// All Rights Reserved
//******************************************************************************
/// Implementation details for the global logger.
//******************************************************************************

#pragma once

namespace SvLog
{
namespace Impl
{

inline std::string path_to_filename(std::string path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

template<typename ValueType>
inline ValueType set_get_attribute(const char* name, ValueType value)
{
	const auto& thread_attr = boost::log::core::get()->get_thread_attributes()[name];
	auto attr = boost::log::attribute_cast<boost::log::attributes::mutable_constant<ValueType>>(thread_attr);
	if (!attr)
	{
		std::cerr << "The requested logger attribute " << name << " is not available. Make sure you initialized the logging library." << std::endl;
		std::cerr.flush();
		exit(1);
	}
	attr.set(value);
	return attr.get();
}


} // namespace Impl

} // namespace SvLog
