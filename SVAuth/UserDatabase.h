//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.h
/// All Rights Reserved
//******************************************************************************
/// User database.
//******************************************************************************

#pragma once

#include <map>

#include "SVProtoBuf/SVAuth.h"

namespace SvAuth
{

class UserDatabase
{
public:
	UserDatabase();

	bool checkPassword(const std::string& username, const std::string& password) const;
	bool getUserClaims(const std::string& username, AuthTokenClaims&) const;

private:
	bool findUser(const std::string& username, const UserDatabaseEntry**) const;

private:
	std::map<std::string, UserDatabaseEntry> m_Entries;
};

} // namespace SvAuth
