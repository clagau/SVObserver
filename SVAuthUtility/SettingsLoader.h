//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SettingsLoader.h
/// All Rights Reserved
//******************************************************************************
/// This class respects the default settings and only overwrites the values that
/// are configured.
//******************************************************************************

#pragma once

namespace SvAuth
{
struct Settings;

class SettingsLoader
{
public:
	void loadFromIni(Settings&);
};
}
