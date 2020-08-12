//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
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
};

} // namespace SvAuth
