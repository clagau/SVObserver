//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLogLibrary/Logging.h"
#include "Crypto.h"
#include "PermissionHelper.h"
#include "UserDatabase.h"
#include "UserDatabaseLoader.h"
#pragma endregion Includes

namespace SvAuth
{
UserDatabase::UserDatabase(const std::string& userDatabaseXml)
	: m_Entries()
	, m_userDatabaseXml(userDatabaseXml)
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

bool UserDatabase::getUserPermissions(const std::string& username, SvPb::Permissions& permissions)
{
	// TODO mutex protection against parallel update?
	for (const auto& it : m_Groups)
	{
		const auto& groupDetails = it.second;
		if (groupDetails.name == "common" || groupDetails.users.find(username) != groupDetails.users.end())
		{
			permissions.MergeFrom(groupDetails.permissions);
		}
	}
	return true;
}

void UserDatabase::getGroupNames(std::set<std::string>& names)
{
	for (const auto& it : m_Groups)
	{
		names.emplace(it.first);
	}
}

bool UserDatabase::getGroupDetails(const std::string& name, std::set<std::string>& users, SvPb::Permissions& permissions)
{
	const auto it = m_Groups.find(name);
	if (it == m_Groups.end())
	{
		return false;
	}
	users = it->second.users;
	permissions.Clear();
	permissions.MergeFrom(it->second.permissions);
	return true;
}

void UserDatabase::updateGroupPermissions(const std::map<std::string, SvPb::Permissions>& groupPermissions)
{
	if (groupPermissions.empty())
	{
		return;
	}

	for (const auto& gpit : groupPermissions)
	{
		const auto& groupname = gpit.first;
		auto it = m_Groups.find(groupname);
		if (it == m_Groups.end())
		{
			SV_LOG_GLOBAL(warning) << "Unable to update permissions for unknown group " << groupname;
			return;
		}

		auto& permissions = it->second.permissions;
		permissions.Clear();
		permissions.MergeFrom(gpit.second);
	}

	writePermissionsToDisk();
}

void UserDatabase::writePermissionsToDisk()
{
	UserDatabaseLoader::save(m_userDatabaseXml, *this);
}

} // namespace SvAuth
