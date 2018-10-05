//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file WebSocketSettingsLoader.h
/// All Rights Reserved 
//*****************************************************************************
/// Log and http Settings are loaded  from SVObserverIni with WebsocketSettingsLoader 
//******************************************************************************

#pragma once

#include "WebsocketSettings.h"

class WebSocketSettingsLoader
{
public:
	void loadFromIni(WebSocketSettings&);
	std::string GetIni();
private:
	std::string m_iniPath {"not loaded"};

};