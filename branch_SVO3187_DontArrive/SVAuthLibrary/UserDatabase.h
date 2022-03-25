//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file UserDatabase.h
/// All Rights Reserved
//******************************************************************************
/// User database.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/SVAuth.h"
#include "SVProtoBuf/Permissions.h"
#pragma endregion Includes

namespace SvAuth
{
struct GroupDetails
{
	std::string name;
	std::set<std::string> users;
	SvPb::Permissions permissions;
};

class UserDatabase
{
private:
	// use UserDatabaseLoader for loading the UserDatabase
	explicit UserDatabase(const std::string& userDatabaseXml);

public:
	bool checkPassword(const std::string& username, const std::string& password) const;
	bool getUserClaims(const std::string& username, AuthTokenClaims&) const;
	bool getUserPermissions(const std::string& username, SvPb::Permissions& permissions);
	void getGroupNames(std::set<std::string>&);
	bool getGroupDetails(const std::string&, std::set<std::string>&, SvPb::Permissions&);
	void updateGroupPermissions(const std::map<std::string, SvPb::Permissions>& groupPermissions);

private:
	bool findUser(const std::string& username, const UserDatabaseEntry**) const;
	std::string hashPassword(const std::string& password, const std::string& salt) const;
	void writePermissionsToDisk();

private:
	friend class UserDatabaseLoader;
	std::map<std::string, GroupDetails> m_Groups;
	std::map<std::string, UserDatabaseEntry> m_Entries;
	const std::string m_userDatabaseXml;
};

} // namespace SvAuth
