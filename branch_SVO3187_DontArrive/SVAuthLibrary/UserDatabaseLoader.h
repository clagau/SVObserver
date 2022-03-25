//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file UserDatabaseLoader.h
/// All Rights Reserved
//******************************************************************************
/// User database loader.
//******************************************************************************

#pragma once

#pragma region Includes
#include "UserDatabase.h"
#pragma endregion Includes

namespace SvAuth
{

class UserDatabaseLoader
{
public:
	static UserDatabase load(std::filesystem::path);
	static void save(std::filesystem::path, const UserDatabase&);
};

} // namespace SvAuth
