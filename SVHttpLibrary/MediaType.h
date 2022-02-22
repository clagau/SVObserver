//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MediaType.h
/// All Rights Reserved
//******************************************************************************
/// Helper for determining the media type (also known as content type) for a
/// given file.
///
/// See https://www.w3.org/TR/html401/types.html#type-content-type
/// See http://www.ietf.org/rfc/rfc2045.txt (Format of Internet Message Bodies)
/// See http://www.ietf.org/rfc/rfc2046.txt (Media Types)
//******************************************************************************

#pragma once

#include <filesystem>
#include <string>

namespace SvHttp
{
class MediaType
{
public:
	std::string determine(const std::filesystem::path& path) const;
};

} // namespace SvHttp
