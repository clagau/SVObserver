//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file RCSettingsLoader.h
/// All Rights Reserved 
//*****************************************************************************
// Setttings for the SVRemotecontrol are reads from SVRemotecontrol.ini
//******************************************************************************
#pragma once

#include "RCSettings.h"

class RCSettingsLoader
{
public:
	void loadFromIni(LPCSTR IniPath, RCSettings& settings);
};

