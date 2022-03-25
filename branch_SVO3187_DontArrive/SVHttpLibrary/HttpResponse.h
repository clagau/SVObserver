//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file HttpResponse.h
/// All Rights Reserved
//******************************************************************************
/// Class encapsulating http response data.
//******************************************************************************

#pragma once

#include <map>

#include <boost/beast/http.hpp>

namespace SvHttp
{
class HttpResponse
{
public:
	boost::beast::http::status Status = boost::beast::http::status::ok;
	std::map<boost::beast::http::field, std::string> Headers = {};
	std::string ContentType = {"text/html"};
	std::string Body;
};

} // namespace SvHttp
