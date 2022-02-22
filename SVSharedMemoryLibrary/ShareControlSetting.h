//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ShareControlSetting.h
/// All Rights Reserved 
//*****************************************************************************
/// Struct with parameter for the sharecontrol. The parameter will be read from the ini file
//******************************************************************************
#pragma once
namespace SvSml
{


struct ShareControlSettings
{
	/// time delay in server between set the Events and removing shared memory
	DWORD DelayBeforeCreateShare {60};
	/// time delay im client between receiving event and destroying  monitor store
	DWORD DelayBeforeClearShare {30};
};

}
