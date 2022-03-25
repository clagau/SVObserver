//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MediaType.cpp
/// All Rights Reserved
//******************************************************************************
/// Helper for determining the media type (also known as content type) for a
/// given file.
//******************************************************************************

#include "stdafx.h"

#include <boost/algorithm/string/predicate.hpp>

#include "MediaType.h"

namespace SvHttp
{
std::string MediaType::determine(const std::filesystem::path& path) const
{
	auto const ext = [&path]
	{
		auto const str = path.string();
		auto const pos = str.rfind(".");
		if (pos == std::string::npos)
			return std::string {};
		return str.substr(pos);
	}();

	using boost::algorithm::iequals;
	if (iequals(ext, ".htm"))   return "text/html";
	if (iequals(ext, ".html"))  return "text/html";
	if (iequals(ext, ".php"))   return "text/html";
	if (iequals(ext, ".css"))   return "text/css";
	if (iequals(ext, ".txt"))   return "text/plain";
	if (iequals(ext, ".js"))    return "application/javascript";
	if (iequals(ext, ".json"))  return "application/json";
	if (iequals(ext, ".proto")) return "application/protobuf";
	if (iequals(ext, ".xml"))   return "application/xml";
	if (iequals(ext, ".swf"))   return "application/x-shockwave-flash";
	if (iequals(ext, ".flv"))   return "video/x-flv";
	if (iequals(ext, ".png"))   return "image/png";
	if (iequals(ext, ".jpe"))   return "image/jpeg";
	if (iequals(ext, ".jpeg"))  return "image/jpeg";
	if (iequals(ext, ".jpg"))   return "image/jpeg";
	if (iequals(ext, ".gif"))   return "image/gif";
	if (iequals(ext, ".bmp"))   return "image/bmp";
	if (iequals(ext, ".ico"))   return "image/vnd.microsoft.icon";
	if (iequals(ext, ".tiff"))  return "image/tiff";
	if (iequals(ext, ".tif"))   return "image/tiff";
	if (iequals(ext, ".svg"))   return "image/svg+xml";
	if (iequals(ext, ".svgz"))  return "image/svg+xml";
	return "application/text";
}

} // namespace SvHttp
