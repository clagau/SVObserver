//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file WebSocketSettingsLoader.h
/// All Rights Reserved 
//*****************************************************************************
/// Log and http Settings are loaded  from SVObserverIni with WebsocketSettingsLoader 
//******************************************************************************

#pragma once

struct WebSocketSettings;

class WebSocketSettingsLoader
{
public:
	void loadFromIni(WebSocketSettings&);
	std::string GetIni();
private:
	std::string m_iniPath {"not loaded"};

};