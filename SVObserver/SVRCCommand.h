//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCCommand.h
/// All Rights Reserved 
//*****************************************************************************
/// Mapping from incoming RPC requests to SVObserver
//******************************************************************************
#pragma once
#include "SVRCCommandInterface.h"
class SVRCCommand :public SVRCCommandInterface
{
public:
	SVRCCommand();
	~SVRCCommand();
	virtual void GetDeviceMode(const SvPb::GetDeviceModeRequest&, SvRpc::Task<SvPb::GetDeviceModeResponse>)override;
	virtual void SetDeviceMode(const SvPb::SetDeviceModeRequest&, SvRpc::Task<SvPb::SetDeviceModeResponse>)override;
private:
	bool GetDeviceMode(const SvPb::GetDeviceModeRequest& request,SvPb::GetDeviceModeResponse& resp, SvPenv::Error& err);
	bool SetDeviceMode(const SvPb::SetDeviceModeRequest& request,SvPb::SetDeviceModeResponse& response, SvPenv::Error& err);
};

