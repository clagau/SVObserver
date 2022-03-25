//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file HttpClient.h
/// All Rights Reserved
//******************************************************************************
/// Very simple blocking http client based on boost::beast.
//******************************************************************************

#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace SvHttp
{

class HttpClient
{
public:
	explicit HttpClient(boost::asio::io_context&);

	HttpResponse request_sync(HttpRequest);

	template<class TRes>
	bool get_sync(Url, TRes& res);

	template<class TReq, class TRes>
	bool post_sync(Url, const TReq& req, TRes& res);

private:
	boost::asio::io_context& m_rIoContext;
};

} // namespace SvHttp

#include "HttpClient.inl"
