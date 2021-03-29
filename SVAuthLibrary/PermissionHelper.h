//******************************************************************************
/// \copyright (c) 2020 by Seidenader Maschinenbau GmbH
/// \file PermissionHelper.h
/// All Rights Reserved
//******************************************************************************
/// Helpers for handling permissions.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/Permissions.h"
#pragma endregion Includes

namespace SvAuth
{
	class PermissionHelper
	{
	public:
		static void serializePermissions(google::protobuf::RepeatedPtrField<std::string>&, const SvPb::Permissions&);
		static bool parsePermission(SvPb::Permissions& dest, const std::string& permission);
	};

} // namespace SvAuth
