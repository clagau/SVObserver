//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpResponse.h
/// All Rights Reserved
//******************************************************************************
/// Class encapsulating http response data.
//******************************************************************************

#pragma once

#include <boost/beast/http.hpp>

namespace SvHttp
{
class HttpResponse
{
public:
	boost::beast::http::status Status = boost::beast::http::status::ok;
	std::string ContentType = {"text/html"};
	std::string Body;
};

} // namespace SvHttp
