//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Crypto.h"
#include "UserDatabase.h"
#pragma endregion Includes

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

	return hashPassword(password, username) == entry->password();
}

// You can use the following bash one-liner to manually generate hashes
//   echo -n user:pass | md5sum | cut -f1 -d' ' | xxd -r -p | base64
std::string UserDatabase::hashPassword(const std::string& password, const std::string& salt) const
{
	Crypto crypto;
	const auto msg = salt + ":" + password;
	const auto md5 = crypto.md5sum(msg);
	const auto b64 = crypto.encodeBase64(md5);
	return b64;
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
