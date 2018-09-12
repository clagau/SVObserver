//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Settings.h
/// All Rights Reserved
//******************************************************************************
/// All server settings in one place.
//******************************************************************************

#pragma once

#include "SVAuthLibrary/AuthManagerSettings.h"
#include "SVHttpLibrary/HttpServerSettings.h"
#include "SVLogLibrary/LogSettings.h"

namespace SvOgw
{
struct Settings
{
	bool dummySharedMemory {false};
	SvLog::LogSettings logSettings;
	SvHttp::HttpServerSettings httpSettings;
	SvAuth::AuthManagerSettings authSettings;
};
}// namespace SvOgw
