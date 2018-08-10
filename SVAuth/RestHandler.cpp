//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include "stdafx.h"

#include "SVAuth/RestHandler.h"
#include "SVProtoBuf/Protobuf2Rapidjson.h"

using namespace Protobuf2Rapidjson;

namespace SvAuth
{

RestHandler::RestHandler(AuthManager& rAuthManager)
	: m_rAuthManager(rAuthManager)
{
}

bool RestHandler::onRestRequest(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
{
	if (req.Method == boost::beast::http::verb::post && req.Url.path() == "/login")
	{
		DecodeContext ctx;
		LoginRequest loginReq;
		if (!Protobuf2Rapidjson::decode_from_cstring(req.Body.data(), req.Body.size(), &loginReq, ctx))
		{
			res.Status = boost::beast::http::status::bad_request;
			res.Body = ctx.error;
			return true;
		}

		LoginResponse loginRes;
		if (!m_rAuthManager.login(loginReq, loginRes))
		{
			res.Status = boost::beast::http::status::unauthorized;
			return true;
		}

		res.Status = boost::beast::http::status::ok;
		res.ContentType = "application/json";
		res.Body = Protobuf2Rapidjson::encode_to_string(loginRes);
		return true;
	}

	if (req.Method == boost::beast::http::verb::post && req.Url.path() == "/auth")
	{
		DecodeContext ctx;
		AuthRequest authReq;
		if (!Protobuf2Rapidjson::decode_from_cstring(req.Body.data(), req.Body.size(), &authReq, ctx))
		{
			res.Status = boost::beast::http::status::bad_request;
			res.Body = ctx.error;
			return true;
		}

		AuthResponse authRes;
		if (!m_rAuthManager.auth(authReq, authRes))
		{
			res.Status = boost::beast::http::status::unauthorized;
			return true;
		}

		res.Status = boost::beast::http::status::ok;
		res.ContentType = "application/json";
		res.Body = Protobuf2Rapidjson::encode_to_string(authRes);
		return true;
	}

	if (req.Method == boost::beast::http::verb::get && req.Url.path() == "/.well-known/jwks.json")
	{
		const auto jwks = m_rAuthManager.getJsonWebKeySet();

		res.Status = boost::beast::http::status::ok;
		res.ContentType = "application/json";
		res.Body = Protobuf2Rapidjson::encode_to_string(jwks);
		return true;
	}

	return false;
}

} // namespace SvAuth
