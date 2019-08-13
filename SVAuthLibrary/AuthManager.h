//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.h
/// All Rights Reserved
//******************************************************************************
/// Central class for handling authentication and authorization requests.
//******************************************************************************

#pragma once

#pragma region Includes
#include "JwtFactory.h"
#include "UserDatabase.h"
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace SvAuth
{
struct AuthManagerSettings;

class AuthManager
{
public:
	explicit AuthManager(const AuthManagerSettings&);

	Jwks getJsonWebKeySet() const;

	bool login(const LoginRequest&, LoginResponse&);
	bool auth(const AuthRequest&, AuthResponse&);
	bool rpcAuth(const std::string& token);
	void logout(const LogoutRequest&, LogoutResponse&);

private:
	bool generateAccessToken(const std::string& username, std::string& token);
	bool generateAuthToken(const std::string& username, std::string& token);
	std::string getShortPublicKey() const;

private:
	const AuthManagerSettings& m_rSettings;
	const UserDatabase m_UserDatabase;
	JwtFactory::SignatureContext m_SecurityContext;
	JwtFactory m_JwtFactory;
};

} // namespace SvAuth
