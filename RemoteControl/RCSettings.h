//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RCSettings.h
/// All Rights Reserved 
//*****************************************************************************
/// Settings for the Remote´control. 
//******************************************************************************
#pragma once

#include "SVLogLibrary/LogSettings.h"
#include "SVHttpLibrary/WebsocketClientSettings.h"
#include "WebsocketLibrary/SVRCClientServiceSettings.h"

namespace SvRc
{
struct RCSettings
{
	RCSettings()
	{
		m_logSettings.FileLogEnabled = false;
		m_logSettings.StdoutLogEnabled = true;
		m_logSettings.StdoutLogLevel = "debug";
		m_logSettings.WindowsEventLogEnabled = true;
		m_logSettings.WindowsEventLogLevel = "info";
		m_logSettings.eventLogFacility = SvLog::EventLogFacility::RemoteCtrl;
	}
	SvLog::LogSettings m_logSettings;
	SvHttp::WebsocketClientSettings m_httpClientSettings;
	SvWsl::SVRCClientServiceSetting   m_svrcClientSettings;
};
} //namespace SvRc
