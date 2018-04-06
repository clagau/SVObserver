//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCCommand.h
/// All Rights Reserved 
//*****************************************************************************
/// Mapping from incoming RPC requests to SVObserver
//******************************************************************************
#pragma once
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/RPCServer.h"
class SVRCCommand 
{
public:
	SVRCCommand();
	~SVRCCommand();
	void GetDeviceMode(const SvPb::GetDeviceModeRequest&, SvRpc::Task<SvPb::GetDeviceModeResponse>);
	void SetDeviceMode(const SvPb::SetDeviceModeRequest&, SvRpc::Task<SvPb::SetDeviceModeResponse>);
private:
	bool GetDeviceMode(const SvPb::GetDeviceModeRequest& request,SvPb::GetDeviceModeResponse& resp, SvPenv::Error& err);
	bool  SetDeviceMode(const SvPb::SetDeviceModeRequest& request,SvPb::SetDeviceModeResponse& response, SvPenv::Error& err);
};

