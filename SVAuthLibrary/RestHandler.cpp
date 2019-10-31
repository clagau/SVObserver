//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "AuthManager.h"
#include "RestHandler.h"
#include "SvHttpLibrary/HttpRequest.h"
#include "SvHttpLibrary/HttpResponse.h"
#include "SVProtoBuf/Protobuf2Rapidjson.h"
#pragma endregion Includes

using namespace Protobuf2Rapidjson;
using boost::beast::http::field;
using boost::beast::http::status;
using boost::beast::http::verb;

namespace SvAuth
{

RestHandler::RestHandler(AuthManager& rAuthManager)
	: m_rAuthManager(rAuthManager)
{
}

bool RestHandler::onRestRequest(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
{
	const auto& path = req.Url.path();
	const auto& method = req.Method;

	if (method == verb::options && (path == "/login" || path == "/auth" || path == "/logout"))
	{
		res.Status = status::ok;
		res.ContentType = "application/json";
		res.Body = "";
		res.Headers[field::access_control_allow_origin] = "*";
		res.Headers[field::access_control_allow_methods] = "POST, GET";
		res.Headers[field::access_control_allow_headers] = "Access-Control-Allow-Headers, Origin, X-Requested-With, Content-Type, Accept";
		return true;
	}

	if (method == verb::post && path == "/login")
	{
		DecodeContext ctx;
		LoginRequest loginReq;
		if (!decode_from_cstring(req.Body.data(), req.Body.size(), &loginReq, ctx))
		{
			res.Status = status::bad_request;
			res.Headers[field::access_control_allow_origin] = "*";
			res.Body = ctx.error;
			return true;
		}

		LoginResponse loginRes;
		if (!m_rAuthManager.login(loginReq, loginRes))
		{
			res.Status = status::unauthorized;
			res.Headers[field::access_control_allow_origin] = "*";
			return true;
		}

		res.Status = status::ok;
		res.Headers[field::access_control_allow_origin] = "*";
		res.ContentType = "application/json";
		res.Body = encode_to_string(loginRes);
		return true;
	}

	if (method == verb::post && path == "/auth")
	{
		DecodeContext ctx;
		AuthRequest authReq;
		if (!decode_from_cstring(req.Body.data(), req.Body.size(), &authReq, ctx))
		{
			res.Status = status::bad_request;
			res.Headers[field::access_control_allow_origin] = "*";
			res.Body = ctx.error;
			return true;
		}

		AuthResponse authRes;
		if (!m_rAuthManager.auth(authReq, authRes))
		{
			res.Status = status::unauthorized;
			res.Headers[field::access_control_allow_origin] = "*";
			return true;
		}

		res.Status = status::ok;
		res.Headers[field::access_control_allow_origin] = "*";
		res.ContentType = "application/json";
		res.Body = encode_to_string(authRes);
		return true;
	}

	if (method == verb::post && path == "/logout")
	{
		DecodeContext ctx;
		LogoutRequest authReq;
		if (!decode_from_cstring(req.Body.data(), req.Body.size(), &authReq, ctx))
		{
			res.Status = status::bad_request;
			res.Headers[field::access_control_allow_origin] = "*";
			res.Body = ctx.error;
			return true;
		}

		LogoutResponse authRes;
		m_rAuthManager.logout(authReq, authRes);

		res.Status = status::ok;
		res.Headers[field::access_control_allow_origin] = "*";
		res.ContentType = "application/json";
		res.Body = encode_to_string(authRes);
		return true;
	}

	if (method == verb::get && path == "/.well-known/jwks.json")
	{
		const auto jwks = m_rAuthManager.getJsonWebKeySet();

		res.Status = status::ok;
		res.ContentType = "application/json";
		res.Headers[field::access_control_allow_origin] = "*";
		res.Body = encode_to_string(jwks);
		return true;
	}

	return false;
}

} // namespace SvAuth
