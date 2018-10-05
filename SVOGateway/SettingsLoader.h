//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SettingsLoader.h
/// All Rights Reserved
//******************************************************************************
/// This class respects the default settings and only overwrites the values that
/// are configured.
//******************************************************************************

#pragma once

#include "Settings.h"
namespace SvOgw
{
class SettingsLoader
{
public:
	void loadFromIni(Settings&);
	std::string GetIni();
private:
	std::string m_iniPath {"not loaded"};

};

}