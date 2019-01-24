//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file SVRCClientServiceSettings.h
/// All Rights Reserved 
//*****************************************************************************
//Struct with parameter for SVRC 
//******************************************************************************
#pragma once

namespace SvWsl
{
struct SVRCClientServiceSetting
{
	boost::posix_time::time_duration ShortTimeout = boost::posix_time::seconds(6); // 6s default
	boost::posix_time::time_duration MediumTimeout = boost::posix_time::seconds(120); // 2min default
	boost::posix_time::time_duration  LongTimeout = boost::posix_time::seconds(300); // 5 min default
};
}