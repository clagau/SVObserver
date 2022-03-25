//******************************************************************************
/// \copyright (c) 2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file PermissionHelper.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable : 4127)
#include <google/protobuf/reflection.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 5054)
#include <rapidjson/document.h>
#pragma warning(pop)
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>

#include "PermissionHelper.h"
#pragma endregion Includes

namespace SvAuth
{

static std::string getNameForViewType(SvPb::Permissions::WebApp::ViewType viewType)
{
	return Permissions_WebApp_ViewType_Name(viewType);
}

static SvPb::Permissions::WebApp::ViewType getViewTypeForParameter(const std::string& params)
{
	auto prefix = params.substr(0, 6);
	auto suffix = params.substr(params.size() - 1);
	if (prefix != "[type=" || suffix != "]")
	{
		// TODO should be false :|
		return SvPb::Permissions::WebApp::AnyView;
	}

	const auto name = params.substr(6, params.size() - 7);
	SvPb::Permissions::WebApp::ViewType result;
	if (SvPb::Permissions_WebApp_ViewType_Parse(name, &result))
	{
		return result;
	}

	return SvPb::Permissions::WebApp::AnyView;
}

void PermissionHelper::serializePermissions(google::protobuf::RepeatedPtrField<std::string>& permissions, const SvPb::Permissions& p)
{
	auto addPermissionIf = [&permissions](bool b, const char* permission)
	{
		if (b)
		{
			permissions.Add(permission);
		}
	};

	addPermissionIf(p.webapp().configuration().save(), "webapp.configuration.save");
	addPermissionIf(p.webapp().logs().read(), "webapp.logs.read");
	addPermissionIf(p.webapp().viewset().add(), "webapp.viewset.add");
	addPermissionIf(p.webapp().viewset().read(), "webapp.viewset.read");
	addPermissionIf(p.webapp().viewset().edit(), "webapp.viewset.edit");
	addPermissionIf(p.webapp().viewset().remove(), "webapp.viewset.remove");

	addPermissionIf(p.svobserver().clientlogs().store(), "svobserver.clientlogs.store");
	addPermissionIf(p.svobserver().command().execute(), "svobserver.command.execute");
	addPermissionIf(p.svobserver().configuration().read(), "svobserver.configuration.read");
	addPermissionIf(p.svobserver().configuration().write(), "svobserver.configuration.write");
	addPermissionIf(p.svobserver().file().read(), "svobserver.file.read");
	addPermissionIf(p.svobserver().file().write(), "svobserver.file.write");
	addPermissionIf(p.svobserver().inspectionstate().edit(), "svobserver.inspectionstate.edit");
	addPermissionIf(p.svobserver().inspectionstate().read(), "svobserver.inspectionstate.read");
	addPermissionIf(p.svobserver().machinestate().set(), "svobserver.machinestate.set");
	addPermissionIf(p.svobserver().mode().edit(), "svobserver.mode.edit");
	addPermissionIf(p.svobserver().mode().read(), "svobserver.mode.read");
	addPermissionIf(p.svobserver().notifications().subscribe(), "svobserver.notifications.subscribe");
	addPermissionIf(p.svobserver().tool().add(), "svobserver.tool.add");
	addPermissionIf(p.svobserver().tool().edit(), "svobserver.tool.edit");
	addPermissionIf(p.svobserver().tool().read(), "svobserver.tool.read");
	addPermissionIf(p.svobserver().tool().remove(), "svobserver.tool.remove");
	addPermissionIf(p.svobserver().value().edit(), "svobserver.value.edit");
	addPermissionIf(p.svobserver().value().read(), "svobserver.value.read");

	addPermissionIf(p.usermanagement().userpermissions().edit(), "usermanagement.userpermissions.edit");
	addPermissionIf(p.usermanagement().userpermissions().read(), "usermanagement.userpermissions.read");

	auto addPermissionWithPrefixIf = [&permissions](bool b, const char* prefix, const std::string& infix, const char* remainder)
	{
		if (b)
		{
			auto permission = prefix + infix + remainder;
			permissions.Add(permission.c_str());
		}
	};

	// TODO optimize any view, so we dont have semantic duplicates
	for (const auto& view : p.webapp().view())
	{
		const auto parameters = "type=" + getNameForViewType(view.type());
		addPermissionWithPrefixIf(view.add(), "webapp.view[", parameters, "].add");
		addPermissionWithPrefixIf(view.read(), "webapp.view[", parameters, "].read");
		addPermissionWithPrefixIf(view.edit(), "webapp.view[", parameters, "].edit");
		addPermissionWithPrefixIf(view.remove(), "webapp.view[", parameters, "].remove");
	}
}

static ::google::protobuf::Message* findOrAddArrayEntry(
	::google::protobuf::Message& parent,
	const ::google::protobuf::Reflection& parentRefl,
	const ::google::protobuf::FieldDescriptor& repeatedField,
	const std::string& params)
{
	// TODO make this more generic
	auto viewType = getViewTypeForParameter(params);

	int len = parentRefl.FieldSize(parent, &repeatedField);
	for (int i = 0; i < len; ++i)
	{
		auto entry = parentRefl.MutableRepeatedMessage(&parent, &repeatedField, i);
		if (!entry)
		{
			continue;
		}

		auto desc = entry->GetDescriptor();
		auto refl = entry->GetReflection();
		if (!desc || !refl)
		{
			continue;
		}

		auto typeField = desc->FindFieldByName("type");
		if (!typeField)
		{
			continue;
		}

		int entryViewType = refl->GetEnumValue(*entry, typeField);
		if (entryViewType == viewType)
		{
			return entry;
		}
	}

	auto newEntry = parentRefl.AddMessage(&parent, &repeatedField);
	if (newEntry)
	{
		auto newEntryDesc = newEntry->GetDescriptor();
		auto newEntryRefl = newEntry->GetReflection();
		if (newEntryDesc  && newEntryRefl)
		{
			auto viewTypeField = newEntryDesc->FindFieldByName("type");
			if (viewTypeField)
			{
				newEntryRefl->SetEnumValue(newEntry, viewTypeField, viewType);
				return newEntry;
			}
		}
	}

	return NULL;
}

bool PermissionHelper::parsePermission(SvPb::Permissions& dest, const std::string& permission)
{
	std::vector<std::string> splitContainer;
	boost::algorithm::split(splitContainer, permission, boost::algorithm::is_any_of("."), boost::algorithm::token_compress_on);

	::google::protobuf::Message* msg = &dest;
	while (msg && !splitContainer.empty())
	{
		auto name = splitContainer.front();
		splitContainer.erase(splitContainer.begin());

		auto paramIdx = name.find('[');
		auto isArray = paramIdx != std::string::npos;
		std::string arrayParams = "";
		if (paramIdx != std::string::npos)
		{
			arrayParams = name.substr(paramIdx);
			name = name.substr(0, paramIdx);
		}

		auto desc = msg->GetDescriptor();
		auto ref = msg->GetReflection();
		if (!desc || !ref)
		{
			return false;
		}

		auto field = desc->FindFieldByName(name);
		if (!field)
		{
			return false;
		}

		if (isArray)
		{
			if (!field->is_repeated())
			{
				return false;
			}
			
			msg = findOrAddArrayEntry(*msg, *ref, *field, arrayParams);
			if (!msg)
			{
				return false;
			}
			continue;
 		}
		else
		{
			switch (field->type())
			{
			case ::google::protobuf::FieldDescriptor::Type::TYPE_BOOL:
				if (!splitContainer.empty())
				{
					return false;
				}
				ref->SetBool(msg, field, true);
				break;

			case ::google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE:
				msg = ref->MutableMessage(msg, field, nullptr);
				break;

			default:
				return false;
			}
		}

	}
	
	return true;
}

} // namespace SvAuth
