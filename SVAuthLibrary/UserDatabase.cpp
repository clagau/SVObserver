//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include "stdafx.h"

#include "SVAuthLibrary/UserDatabase.h"

namespace SvAuth
{
UserDatabase::UserDatabase()
	: m_Entries()
{
}

bool UserDatabase::checkPassword(const std::string& username, const std::string& password) const
{
	const UserDatabaseEntry* entry = nullptr;
	if (!findUser(username, &entry))
	{
		return false;
	}

	// TODO: we should use hashed & salted passwords
	return password == entry->password();
}

bool UserDatabase::getUserClaims(const std::string& username, AuthTokenClaims& claims) const
{
	const UserDatabaseEntry* entry = nullptr;
	if (!findUser(username, &entry))
	{
		return false;
	}

	claims.set_username(entry->username());
	claims.set_fullname(entry->firstname() + " " + entry->lastname());
	claims.set_email(entry->email());
	claims.set_level(entry->level());

	return true;
}

bool UserDatabase::findUser(const std::string& username, const UserDatabaseEntry** entry) const
{
	if (username.empty())
	{
		return false;
	}

	auto it = m_Entries.find(username);
	if (it == m_Entries.end())
	{
		return false;
	}

	if (!entry)
	{
		return false;
	}

	*entry = &it->second;
	return true;
}

} // namespace SvAuth
