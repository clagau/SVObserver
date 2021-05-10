//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabase.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include <atlstr.h>
#include <comutil.h>
#include <codecvt>
#include <locale>

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include "PermissionHelper.h"
#include "UserDatabaseLoader.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/SVAuth.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVXmlLibrary/SVSimpleXml.h"
#pragma endregion Includes

static std::mutex s_writeMutex;

namespace SvAuth
{

static void loadUserAttributes(UserDatabaseEntry& entry, const SvXml::XmlElement& userEle)
{
	for (const auto& attr : userEle.attributes)
	{
		if (attr.name == L"name")
		{
			const auto name = SvUl::to_utf8(attr.value);
			entry.set_firstname(name);
			entry.set_username(name); // will be overwritten in case we find a passwordAuth node
		}
		else if (attr.name == L"level")
		{
			try
			{
				entry.set_level(boost::lexical_cast<int>(attr.value));
			}
			catch (const boost::bad_lexical_cast&)
			{
				SV_LOG_GLOBAL(warning) << "Error while parsing user level " << attr.value;
			}
		}
	}
}

static void loadPasswordAttributes(UserDatabaseEntry& entry, const SvXml::XmlElement& userEle)
{
	SvXml::iterateElements(userEle, L"passwordAuth", [&entry](const SvXml::XmlElement& rPasswordAuth)
	{
		for (const auto& attr : rPasswordAuth.attributes)
		{
			if (attr.name == L"userName")
			{
				entry.set_username(SvUl::to_utf8(attr.value));
			}
			else if (attr.name == L"passwordCoded")
			{
				entry.set_password(SvUl::to_utf8(attr.value));
			}
		}
	});
}

static bool isValidUser(const UserDatabaseEntry& entry, std::string& errMsg)
{
	if (entry.username().empty())
	{
		errMsg = "empty username";
		return false;
	}
	if (entry.password().empty())
	{
		errMsg = "empty password";
		return false;
	}
	return true;
}

static void loadGroupAttributes(GroupDetails& group, const SvXml::XmlElement& userGroupEle)
{
	for (const auto& attr : userGroupEle.attributes)
	{
		if (attr.name == L"name")
		{
			group.name = SvUl::to_utf8(attr.value);
		}
	}
}

static void loadGroupUserAssignments(GroupDetails& group, const SvXml::XmlElement& userGroupEle)
{
	SvXml::iterateElements(userGroupEle, L"users.refName", [&group](const SvXml::XmlElement& rUserRef)
	{
		if (!rUserRef.content.empty())
		{
			group.users.emplace(SvUl::to_utf8(rUserRef.content));
		}
	});
}

static void loadGroupPermissionAttributes(GroupDetails& group, const SvXml::XmlElement& userGroupEle)
{
	SvXml::iterateElements(userGroupEle, L"workEnvironment.permissions.permission", [&group](const SvXml::XmlElement& rPermission)
	{
		for (const auto& attr : rPermission.attributes)
		{
			if (attr.name == L"name")
			{
				auto permission = SvUl::to_utf8(attr.value);
				if (SvAuth::PermissionHelper::parsePermission(group.permissions, permission))
				{
					SV_LOG_GLOBAL(debug) << "Grop " << group.name << " has permission " << permission;
				}
				else
				{
					SV_LOG_GLOBAL(debug) << "Unable to parse permission \"" << permission << "\" for group " << group.name;
				}
			}
		}
	});
}

static void setAllPermissions(SvPb::Permissions& permissions)
{
	// allow all webapp permissions
	permissions.mutable_webapp()->mutable_configuration()->set_save(true);
	permissions.mutable_webapp()->mutable_logs()->set_read(true);
	auto& anyView = *permissions.mutable_webapp()->add_view();
	anyView.set_type(SvPb::Permissions::WebApp::AnyView);
	anyView.set_add(true);
	anyView.set_read(true);
	anyView.set_edit(true);
	anyView.set_remove(true);
	permissions.mutable_webapp()->mutable_viewset()->set_add(true);
	permissions.mutable_webapp()->mutable_viewset()->set_read(true);
	permissions.mutable_webapp()->mutable_viewset()->set_edit(true);
	permissions.mutable_webapp()->mutable_viewset()->set_remove(true);

	// allow all svobserver permissions
	permissions.mutable_svobserver()->mutable_clientlogs()->set_store(true);
	permissions.mutable_svobserver()->mutable_configuration()->set_read(true);
	permissions.mutable_svobserver()->mutable_configuration()->set_write(true);
	permissions.mutable_svobserver()->mutable_file()->set_read(true);
	permissions.mutable_svobserver()->mutable_file()->set_write(true);
	permissions.mutable_svobserver()->mutable_inspectionstate()->set_edit(true);
	permissions.mutable_svobserver()->mutable_inspectionstate()->set_read(true);
	permissions.mutable_svobserver()->mutable_machinestate()->set_set(true);
	permissions.mutable_svobserver()->mutable_mode()->set_edit(true);
	permissions.mutable_svobserver()->mutable_mode()->set_read(true);
	permissions.mutable_svobserver()->mutable_notifications()->set_subscribe(true);
	permissions.mutable_svobserver()->mutable_tool()->set_add(true);
	permissions.mutable_svobserver()->mutable_tool()->set_edit(true);
	permissions.mutable_svobserver()->mutable_tool()->set_read(true);
	permissions.mutable_svobserver()->mutable_tool()->set_remove(true);
	permissions.mutable_svobserver()->mutable_value()->set_edit(true);
	permissions.mutable_svobserver()->mutable_value()->set_read(true);

	// allow all user management permissions
	permissions.mutable_usermanagement()->mutable_userpermissions()->set_edit(true);
	permissions.mutable_usermanagement()->mutable_userpermissions()->set_read(true);
}

static bool isValidGroup(const GroupDetails& group, std::string& errMsg)
{
	if (group.name.empty())
	{
		errMsg = "group name is empty";
		return false;
	}
	return true;
}

static bool parseUserDatabaseXml(std::map<std::string, UserDatabaseEntry>& rUserDb, std::map<std::string, GroupDetails>& rGroupDb, SvXml::XmlElement& rDoc)
{
	SvXml::iterateElements(rDoc, L"asphereDatabase.userDatabase.users.user", [&rUserDb](const SvXml::XmlElement& rUser)
	{
		UserDatabaseEntry userEntry;
		loadUserAttributes(userEntry, rUser);
		loadPasswordAttributes(userEntry, rUser);

		std::string errMsg;
		if (!isValidUser(userEntry, errMsg))
		{
			SV_LOG_GLOBAL(debug) << "Found invalid user in user database: " << errMsg;
			return;
		}

		rUserDb.insert(std::make_pair(userEntry.username(), userEntry));
	});
	SvXml::iterateElements(rDoc, L"asphereDatabase.userDatabase.userGroups.userGroup", [&rGroupDb](const SvXml::XmlElement& rUserGroup)
		{
			GroupDetails group;
			loadGroupAttributes(group, rUserGroup);
			loadGroupUserAssignments(group, rUserGroup);
			loadGroupPermissionAttributes(group, rUserGroup);
			if (group.name == "sdm")
			{
				setAllPermissions(group.permissions);
			}

			std::string errMsg;
			if (!isValidGroup(group, errMsg))
			{
				SV_LOG_GLOBAL(debug) << "Found invalid group in user database: " << errMsg;
				return;
			}

			SV_LOG_GLOBAL(debug) << "Permissions for group " << group.name << ": " << group.permissions.DebugString();

			rGroupDb.insert(std::make_pair(group.name, group));
		});
	return true;
}

static SvXml::XmlElement* getOrCreateChild(SvXml::XmlElement& ele, const std::wstring& name)
{
	auto& children = ele.children;
	auto it = std::find_if(children.begin(), children.end(), [&name](const auto& child) { return child.name == name;  });
	if (it != children.end())
	{
		return &(*it);
	}
	return ele.addChild(name);
}

static bool isKnownPermission(const SvXml::XmlElement& rPermissionEle)
{
	auto iter = std::find_if(rPermissionEle.attributes.begin(), rPermissionEle.attributes.end(), [](const auto& rEntry) {return rEntry.name == L"name"; });
	if (rPermissionEle.attributes.end() != iter)
	{
		SvPb::Permissions permissions;
		return SvAuth::PermissionHelper::parsePermission(permissions, SvUl::to_utf8(iter->value));
	}
	return false;
}

static void removeKnownPermissions(std::vector<SvXml::XmlElement>& rPermissionElements)
{
	for (auto it = rPermissionElements.begin(); it != rPermissionElements.end();)
	{
		if (isKnownPermission(*it))
		{
			it = rPermissionElements.erase(it);
		}
		else
		{
			++it;
		}
	}
}

static void patchPermissionsImpl(SvXml::XmlElement& rUser, const SvPb::Permissions& rPermissions)
{
	auto& workEnv = *getOrCreateChild(rUser, L"workEnvironment");
	auto& permissions = *getOrCreateChild(workEnv, L"permissions");
	
	google::protobuf::RepeatedPtrField<std::string> permissionList;
	PermissionHelper::serializePermissions(permissionList, rPermissions);

	// remove known ones, keep unknown ones. then re-add only the known ones that are currently assigned
	removeKnownPermissions(permissions.children);
	for (const auto& permission : permissionList)
	{
		auto p = permissions.addChild(L"permission");
		p->addAttribute(L"name", SvUl::to_utf16(permission));
	}
}

static void patchPermissions(SvXml::XmlElement& rDoc, const std::map<std::string, GroupDetails>& rGroupDb)
{
	SvXml::iterateElements(rDoc, L"asphereDatabase.userDatabase.userGroups.userGroup", [&rGroupDb](SvXml::XmlElement& rUserGroup)
	{
		GroupDetails group;
		loadGroupAttributes(group, rUserGroup); // done to determine the current group name

		auto it = rGroupDb.find(group.name);
		if (it == rGroupDb.end())
		{
			return;
		}
		
		const auto& newGroupEntry = it->second;
		patchPermissionsImpl(rUserGroup, newGroupEntry.permissions);
	});
}

UserDatabase UserDatabaseLoader::load(std::filesystem::path path)
{
	UserDatabase db(path.string());
	
	SvXml::XmlElement doc;
	if (SvXml::readFromXmlFile(doc, path.string()))
	{
		if (parseUserDatabaseXml(db.m_Entries, db.m_Groups, doc))
		{
			SV_LOG_GLOBAL(info) << "Successfully loaded " << db.m_Entries.size() << " users from " << path;
		}
		else
		{
			SV_LOG_GLOBAL(error) << "Unable to parse user database from " << path;
		}
	}
	else
	{
		SV_LOG_GLOBAL(error) << "Unable to load user database from " << path;
	}

	return db;
}

void UserDatabaseLoader::save(std::filesystem::path path, const UserDatabase& rDb)
{
	std::lock_guard<std::mutex> lk(s_writeMutex);
	{
		SvXml::XmlElement doc;
		if (!SvXml::readFromXmlFile(doc, path.string()))
		{
			return;
		}

		patchPermissions(doc, rDb.m_Groups);

		SvXml::writeToFile(path.string(), doc);
	}
}


} // namespace SvAuth
