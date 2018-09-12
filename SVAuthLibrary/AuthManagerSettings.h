//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthManagerSettings.h
/// All Rights Reserved
//******************************************************************************
/// Global settings for the AuthManager
//******************************************************************************

#pragma once

#include <string>

namespace SvAuth
{

struct AuthManagerSettings
{
	/// Full path to the xml user database file.
	std::string UserSettingsFile {"C:\\SVObserver\\UserSettings.xml"};

	std::string JwtHmacSecret;
	std::string JwtRsaPublicKeyFile;
	std::string JwtRsaPrivateKeyFile;
};

} // namespace SvAuth
