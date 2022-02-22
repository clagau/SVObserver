//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RCSettingsLoader.h
/// All Rights Reserved 
//*****************************************************************************
// Settings for the Remote control are reads from Remotecontrol.ini
//******************************************************************************
#pragma once



namespace SvRc
{
struct RCSettings;

class RCSettingsLoader
{
public:
	void loadFromIni(LPCSTR IniPath, RCSettings& settings);
};
} //namespace SvRc

