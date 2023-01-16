//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file HttpRequest.h
/// All Rights Reserved
//******************************************************************************
/// Class encapsulating http request data.
//******************************************************************************

#pragma once

#include <map>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "url.hpp"

namespace SvHttp
{
class HttpRequest
{
public:
	boost::beast::http::verb Method;
	Url Url;
	std::map<boost::beast::http::field, std::string> Headers = {};
	std::string Body;
};

} // namespace SvHttp
