//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

	UserDatabase& getUserDatabase() { return m_UserDatabase; }
	const UserDatabase& getUserDatabase() const { return m_UserDatabase; }

	Jwks getJsonWebKeySet() const;

	bool login(const LoginRequest&, LoginResponse&);
	bool loginNoPassword(const LoginRequest&, LoginResponse&);
	bool auth(const AuthRequest&, AuthResponse&);
	bool rpcAuth(const std::string& token, SvAuth::SessionContext& ctx);
	void logout(const LogoutRequest&, LogoutResponse&);

private:
	bool generateAccessToken(const std::string& username, std::string& token);
	bool generateAuthToken(const std::string& username, std::string& token);
	std::string getShortPublicKey() const;

private:
	const AuthManagerSettings& m_rSettings;
	UserDatabase m_UserDatabase;
	JwtFactory::SignatureContext m_SecurityContext;
	JwtFactory m_JwtFactory;
};

} // namespace SvAuth
