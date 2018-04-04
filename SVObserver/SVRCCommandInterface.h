//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCCommandInterface.h
/// All Rights Reserved 
//*****************************************************************************
/// Pure virtual interface to the SVRC Functions in SVobserver
//******************************************************************************
#pragma once
#include "SVProtobuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/Task.h"

class SVRCCommandInterface
{
public:
	virtual ~SVRCCommandInterface() {};
	virtual void GetDeviceMode(const SvPb::GetDeviceModeRequest&, SvRpc::Task<SvPb::GetDeviceModeResponse>) = 0;
	virtual void SetDeviceMode(const SvPb::SetDeviceModeRequest&, SvRpc::Task<SvPb::SetDeviceModeResponse>) = 0;
};