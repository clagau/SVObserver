//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file UserDatabaseLoader.h
/// All Rights Reserved
//******************************************************************************
/// User database loader.
//******************************************************************************

#pragma once

#include <filesystem>

#include <SVAuthLibrary/UserDatabase.h>

namespace SvAuth
{

class UserDatabaseLoader
{
public:
	static UserDatabase load(std::experimental::filesystem::path);
};

} // namespace SvAuth
