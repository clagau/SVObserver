//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpClient.h
/// All Rights Reserved
//******************************************************************************
/// Very simple blocking http client based on boost::beast.
//******************************************************************************

#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "SVHttpLibrary/HttpRequest.h"
#include "SVHttpLibrary/HttpResponse.h"

namespace SvHttp
{

class HttpClient
{
public:
	HttpClient(boost::asio::io_context&);

	HttpResponse request_sync(HttpRequest);

	template<class TRes>
	bool get_sync(Url, TRes& res);

	template<class TReq, class TRes>
	bool post_sync(Url, const TReq& req, TRes& res);

private:
	boost::asio::io_context& m_rIoContext;
};

} // namespace SvHttp

#include <SVHttpLibrary/HttpClient.inl>
