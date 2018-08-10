//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include "stdafx.h"

#include <boost/log/trivial.hpp>

#include "SVAuth/AuthManager.h"
#include "SVAuth/Jwt.h"

namespace SvAuth
{
// TODO Do not use example keys from https://jwt.io/ but generate own keys!
static JwtFactory::SignatureContext build_jwt_io_dummy_context()
{
	JwtFactory::SignatureContext ctx;
	ctx.Secret = "your-256-bit-secret";
	ctx.PublicKey =
		"-----BEGIN PUBLIC KEY-----\n"
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDdlatRjRjogo3WojgGHFHYLugd\n"
		"UWAY9iR3fy4arWNA1KoS8kVw33cJibXr8bvwUAUparCwlvdbH6dvEOfou0/gCFQs\n"
		"HUfQrSDv+MuSUMAe8jzKE4qW+jK+xQU9a03GUnKHkkle+Q0pX/g6jXZ7r1/xAK5D\n"
		"o2kQ+X5xK9cipRgEKwIDAQAB\n"
		"-----END PUBLIC KEY-----\n";
	ctx.PrivateKey =
		"-----BEGIN RSA PRIVATE KEY-----\n"
		"MIICWwIBAAKBgQDdlatRjRjogo3WojgGHFHYLugdUWAY9iR3fy4arWNA1KoS8kVw\n"
		"33cJibXr8bvwUAUparCwlvdbH6dvEOfou0/gCFQsHUfQrSDv+MuSUMAe8jzKE4qW\n"
		"+jK+xQU9a03GUnKHkkle+Q0pX/g6jXZ7r1/xAK5Do2kQ+X5xK9cipRgEKwIDAQAB\n"
		"AoGAD+onAtVye4ic7VR7V50DF9bOnwRwNXrARcDhq9LWNRrRGElESYYTQ6EbatXS\n"
		"3MCyjjX2eMhu/aF5YhXBwkppwxg+EOmXeh+MzL7Zh284OuPbkglAaGhV9bb6/5Cp\n"
		"uGb1esyPbYW+Ty2PC0GSZfIXkXs76jXAu9TOBvD0ybc2YlkCQQDywg2R/7t3Q2OE\n"
		"2+yo382CLJdrlSLVROWKwb4tb2PjhY4XAwV8d1vy0RenxTB+K5Mu57uVSTHtrMK0\n"
		"GAtFr833AkEA6avx20OHo61Yela/4k5kQDtjEf1N0LfI+BcWZtxsS3jDM3i1Hp0K\n"
		"Su5rsCPb8acJo5RO26gGVrfAsDcIXKC+bQJAZZ2XIpsitLyPpuiMOvBbzPavd4gY\n"
		"6Z8KWrfYzJoI/Q9FuBo6rKwl4BFoToD7WIUS+hpkagwWiz+6zLoX1dbOZwJACmH5\n"
		"fSSjAkLRi54PKJ8TFUeOP15h9sQzydI8zJU+upvDEKZsZc/UhT/SySDOxQ4G/523\n"
		"Y0sz/OZtSWcol/UMgQJALesy++GdvoIDLfJX5GBQpuFgFenRiRDabxrE9MNUZ2aP\n"
		"FaFp+DyAe+b4nDwuJaW2LURbr8AEZga7oQj0uYxcYw==\n"
		"-----END RSA PRIVATE KEY-----\n";
	return ctx;
}

AuthManager::AuthManager()
	: m_UserDatabase()
	, m_SecurityContext(build_jwt_io_dummy_context())
	, m_JwtFactory(m_SecurityContext)
{
}

Jwks AuthManager::getJsonWebKeySet() const
{
	Jwks jwks;
	auto& key = *jwks.add_keys();
	key.set_alg(JwtAlgorithm_Name(JwtAlgorithm::RS256));
	key.set_kty("RSA");
	key.set_kid("jwt.io-example-key");
	key.set_use("sig");
	// TODO: direve from m_SecurityContext.PublicKey
	key.add_x5c(
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDdlatRjRjogo3WojgGHFHYLugd"
		"UWAY9iR3fy4arWNA1KoS8kVw33cJibXr8bvwUAUparCwlvdbH6dvEOfou0/gCFQs"
		"HUfQrSDv+MuSUMAe8jzKE4qW+jK+xQU9a03GUnKHkkle+Q0pX/g6jXZ7r1/xAK5D"
		"o2kQ+X5xK9cipRgEKwIDAQAB");
	return jwks;
}

bool AuthManager::login(const LoginRequest& req, LoginResponse& res)
{
	res.Clear();

	if (!m_UserDatabase.checkPassword(req.username(), req.password()))
	{
		return false;
	}

	auto accessToken = std::string();
	if (!generateAccessToken(req.username(), accessToken))
	{
		BOOST_LOG_TRIVIAL(info) << "Error while generating access token for user " << req.username();
		return false;
	}

	res.set_accesstoken(accessToken);

	return true;
}

bool AuthManager::auth(const AuthRequest& req, AuthResponse& res)
{
	const auto& accessToken = req.accesstoken();
	if (accessToken.empty())
	{
		return false;
	}

	auto jwt = Jwt<AccessTokenClaims>();
	const auto rc = m_JwtFactory.parse(jwt, accessToken);
	if (rc != JwtFactory::Parse_Success)
	{
		BOOST_LOG_TRIVIAL(info) << "Jwt parse error " << rc;
		return false;
	}

	const auto& username = jwt.getPayload().name();

	auto authToken = std::string();
	if (!generateAuthToken(username, authToken))
	{
		BOOST_LOG_TRIVIAL(info) << "Error while generating auth token for user " << username;
		return false;
	}

	res.set_authtoken(authToken);

	return true;
}

bool AuthManager::generateAccessToken(const std::string& username, std::string& token)
{
	auto jwt = Jwt<AccessTokenClaims>();
	jwt.setAlgorithm(HS256);
	jwt.setValidityInSeconds(3600);
	jwt.getPayload().set_name(username);

	return m_JwtFactory.generate(token, jwt);
}

bool AuthManager::generateAuthToken(const std::string& username, std::string& token)
{
	auto jwt = Jwt<AuthTokenClaims>();
	jwt.setAlgorithm(RS256);
	jwt.setValidityInSeconds(3600);
	
	if (!m_UserDatabase.getUserClaims(username, jwt.getPayload()))
	{
		return false;
	}

	return m_JwtFactory.generate(token, jwt);
}

} // namespace SvAuth
