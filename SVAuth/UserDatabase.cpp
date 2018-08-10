//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include "stdafx.h"

#include "SVAuth/UserDatabase.h"

namespace SvAuth
{
static void add_dummy_entries(std::map<std::string, UserDatabaseEntry>& db)
{
	{
		UserDatabaseEntry entry;
		entry.set_username("asphere");
		entry.set_password("asphere");
		entry.set_email("asphere@seidenader.de");
		entry.set_level(1);
		entry.set_firstname("A");
		entry.set_lastname("Sphere");
		db.emplace("asphere", std::move(entry));
	}
	{
		UserDatabaseEntry entry;
		entry.set_username("sdmAdmin");
		entry.set_password("sdmAdmin");
		entry.set_email("sdmAdmin@seidenader.de");
		entry.set_level(2);
		entry.set_firstname("Admin");
		entry.set_lastname("Istrator");
		db.emplace("sdmAdmin", std::move(entry));
	}
	{
		UserDatabaseEntry entry;
		entry.set_username("user1");
		entry.set_password("1");
		entry.set_email("user1@seidenader.de");
		entry.set_level(45);
		entry.set_firstname("User");
		entry.set_lastname("Level1");
		db.emplace("user1", std::move(entry));
	}
	{
		UserDatabaseEntry entry;
		entry.set_username("user2");
		entry.set_password("2");
		entry.set_email("user2@seidenader.de");
		entry.set_level(35);
		entry.set_firstname("User");
		entry.set_lastname("Level2");
		db.emplace("user2", std::move(entry));
	}
	{
		UserDatabaseEntry entry;
		entry.set_username("user3");
		entry.set_password("3");
		entry.set_email("user3@seidenader.de");
		entry.set_level(25);
		entry.set_firstname("User");
		entry.set_lastname("Level3");
		db.emplace("user3", std::move(entry));
	}
	{
		UserDatabaseEntry entry;
		entry.set_username("user4");
		entry.set_password("4");
		entry.set_email("user4@seidenader.de");
		entry.set_level(15);
		entry.set_firstname("User");
		entry.set_lastname("Level4");
		db.emplace("user4", std::move(entry));
	}
	{
		UserDatabaseEntry entry;
		entry.set_username("user5");
		entry.set_password("5");
		entry.set_email("user5@seidenader.de");
		entry.set_level(5);
		entry.set_firstname("User");
		entry.set_lastname("Level5");
		db.emplace("user5", std::move(entry));
	}
}

UserDatabase::UserDatabase()
	: m_Entries()
{
	add_dummy_entries(m_Entries);
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
