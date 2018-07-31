//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpRequest.h
/// All Rights Reserved
//******************************************************************************
/// Class encapsulating http request data.
//******************************************************************************

#pragma once

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
	boost::beast::string_view Body;
};

} // namespace SvHttp
