//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManager.h
/// All Rights Reserved
//******************************************************************************
/// Central class for handling authentication and authorization requests.
//******************************************************************************

#pragma once

#include "SVAuthLibrary/AuthManagerSettings.h"
#include "SVAuthLibrary/JwtFactory.h"
#include "SVAuthLibrary/UserDatabase.h"
#include "SVProtoBuf/SVAuth.h"

namespace SvAuth
{

class AuthManager
{
public:
	AuthManager(const AuthManagerSettings&);

	Jwks getJsonWebKeySet() const;

	bool login(const LoginRequest&, LoginResponse&);
	bool auth(const AuthRequest&, AuthResponse&);

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
