//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.cpp
/// All Rights Reserved
//******************************************************************************
/// Central class for handling authentication and authorization requests.
//******************************************************************************

#include "stdafx.h"

#include "SVAuthLibrary/AuthClient.h"
#include "SVAuthLibrary/JwtFactory.h"
#include "SVAuthLibrary/Jwt.h"
#include "SVHttpLibrary/HttpClient.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/SVRC.h"

namespace SvAuth
{
AuthClient::AuthClient(boost::asio::io_context& rIoContext, std::string host, uint16_t port)
	: m_HttpClient(rIoContext)
	, m_Host(host)
	, m_Port(port)
{
}

bool AuthClient::login(const std::string& user, const std::string& pass, std::string& access_token)
{
	LoginRequest loginReq;
	loginReq.set_username(user);
	loginReq.set_password(pass);
	LoginResponse loginRes;
	if (!m_HttpClient.post_sync(build_url("/login"), loginReq, loginRes))
	{
		return false;
	}

	access_token = loginRes.accesstoken();
	return true;
}

bool AuthClient::auth(const std::string& access_token, std::string& auth_token)
{
	AuthRequest authReq;
	authReq.set_accesstoken(access_token);
	AuthResponse authRes;
	if (!m_HttpClient.post_sync(build_url("/auth"), authReq, authRes))
	{
		return false;
	}

	auth_token = authRes.authtoken();

	Jwks jwksRes;
	if (!m_HttpClient.get_sync(build_url("/.well-known/jwks.json"), jwksRes))
	{
		return false;
	}

	std::string publicKey;
	for (const auto& jwk : jwksRes.keys())
	{
		if (jwk.kty() != "RSA" || jwk.use() != "sig")
		{
			continue;
		}
		const auto& x5c = jwk.x5c();
		if (x5c.empty())
		{
			continue;
		}

		publicKey = "-----BEGIN PUBLIC KEY-----\n" + *x5c.begin() + "\n-----END PUBLIC KEY-----";
		break;
	}

	if (publicKey.empty())
	{
		SV_LOG_GLOBAL(warning) << "Server did not provide any public key. "
			<< "Unable to verify the jwt signature. Assuming it is valid.";
		return true;
	}

	JwtFactory::SignatureContext ctx;
	ctx.PublicKey = publicKey;
	JwtFactory jwtFactory(ctx);

	auto jwt = Jwt<AccessTokenClaims>();
	const auto rc = jwtFactory.parse(jwt, auth_token);
	if (rc != JwtFactory::Parse_Success)
	{
		SV_LOG_GLOBAL(info) << "Jwt parse error " << rc;
		return false;
	}

	return true;
}

Url AuthClient::build_url(const std::string& path)
{
	return Url()
		.scheme("http")
		.host(m_Host)
		.port(m_Port)
		.path(path);
}

} // namespace SvAuth
