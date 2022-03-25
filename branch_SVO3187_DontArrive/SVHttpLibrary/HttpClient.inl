//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file HttpClient.inl
/// All Rights Reserved
//******************************************************************************
/// Very simple blocking http client based on boost::beast.
//******************************************************************************

#pragma once

#include <boost/beast/http/field.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/verb.hpp>

#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/Protobuf2Rapidjson.h"

namespace SvHttp
{

template<class TRes>
bool HttpClient::get_sync(Url url, TRes& res)
{
	using namespace Protobuf2Rapidjson;
	using namespace boost::beast;

	HttpRequest httpReq;
	httpReq.Method = http::verb::get;
	httpReq.Url = std::move(url);

	auto httpRes = request_sync(std::move(httpReq));
	if (httpRes.Status != http::status::ok)
	{
		return false;
	}

	DecodeContext ctx;
	const auto& resBody = httpRes.Body;
	if (!decode_from_cstring(resBody.data(), resBody.size(), &res, ctx))
	{
		SV_LOG_GLOBAL(error) << "Http response deserialization error: " << ctx.error;
		return false;
	}

	return true;
}

template<class TReq, class TRes>
bool HttpClient::post_sync(Url url, const TReq& req, TRes& res)
{
	using namespace Protobuf2Rapidjson;
	using namespace boost::beast;

	auto reqBody = encode_to_string(req);

	HttpRequest httpReq;
	httpReq.Method = http::verb::post;
	httpReq.Url = std::move(url);
	httpReq.Body = reqBody;
	httpReq.Headers[http::field::content_type] = "application/json";

	auto httpRes = request_sync(std::move(httpReq));
	if (httpRes.Status != http::status::ok)
	{
		return false;
	}

	DecodeContext ctx;
	const auto& resBody = httpRes.Body;
	if (!decode_from_cstring(resBody.data(), resBody.size(), &res, ctx))
	{
		SV_LOG_GLOBAL(error) << "Http response deserialization error: " << ctx.error;
		return false;
	}

	return true;
}

} // namespace SvHttp
