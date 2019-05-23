//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.h
/// All Rights Reserved
//******************************************************************************
/// User database.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace SvAuth
{

class UserDatabase
{
private:
	// use UserDatabaseLoader for loading the UserDatabase
	UserDatabase();

public:
	bool checkPassword(const std::string& username, const std::string& password) const;
	bool getUserClaims(const std::string& username, AuthTokenClaims&) const;

private:
	bool findUser(const std::string& username, const UserDatabaseEntry**) const;
	std::string hashPassword(const std::string& password, const std::string& salt) const;

private:
	friend class UserDatabaseLoader;
	std::map<std::string, UserDatabaseEntry> m_Entries;
};

} // namespace SvAuth
