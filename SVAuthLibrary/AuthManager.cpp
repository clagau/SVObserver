//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include "stdafx.h"

#include <fstream>
#include <sstream>

#include <boost/algorithm/string/replace.hpp>

#include "SVAuthLibrary/AuthManager.h"
#include "SVAuthLibrary/UserDatabaseLoader.h"
#include "SVAuthLibrary/Jwt.h"
#include "SVLogLibrary/Logging.h"

namespace SvAuth
{
static std::string loadFileContents(const std::string& fname)
{
	std::ifstream t(fname.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

static JwtFactory::SignatureContext build_jwt_io_dummy_context(const AuthManagerSettings& rSettings)
{
	// TODO catch cases where these settings are not configured correctly!
	JwtFactory::SignatureContext ctx;
	ctx.Secret = rSettings.JwtHmacSecret;
	ctx.PublicKey = loadFileContents(rSettings.JwtRsaPublicKeyFile);
	ctx.PrivateKey = loadFileContents(rSettings.JwtRsaPrivateKeyFile);
	return ctx;
}

AuthManager::AuthManager(const AuthManagerSettings& rSettings)
	: m_rSettings(rSettings)
	, m_UserDatabase(UserDatabaseLoader::load(rSettings.UserSettingsFile))
	, m_SecurityContext(build_jwt_io_dummy_context(rSettings))
	, m_JwtFactory(m_SecurityContext)
{
	getJsonWebKeySet();
}

Jwks AuthManager::getJsonWebKeySet() const
{
	Jwks jwks;
	auto& key = *jwks.add_keys();
	key.set_alg(JwtAlgorithm_Name(JwtAlgorithm::RS256));
	key.set_kty("RSA");
	key.set_kid("svogateway-key");
	key.set_use("sig");
	key.add_x5c(getShortPublicKey());
	return jwks;
}

std::string AuthManager::getShortPublicKey() const
{
	auto pubKey = m_SecurityContext.PublicKey;
	boost::replace_all(pubKey, "\n", "");
	boost::replace_all(pubKey, "\r", "");
	boost::replace_all(pubKey, "-----BEGIN PUBLIC KEY-----", "");
	boost::replace_all(pubKey, "-----END PUBLIC KEY-----", "");
	return pubKey;
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
		SV_LOG_GLOBAL(info) << "Error while generating access token for user " << req.username();
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
		SV_LOG_GLOBAL(info) << "Jwt parse error " << rc;
		return false;
	}

	const auto& username = jwt.getPayload().name();

	auto authToken = std::string();
	if (!generateAuthToken(username, authToken))
	{
		SV_LOG_GLOBAL(info) << "Error while generating auth token for user " << username;
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