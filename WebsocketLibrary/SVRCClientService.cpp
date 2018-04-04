//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCClientService.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "stdafx.h"
#include "SVRCClientService.h"
#include "SVProtoBuf\SVRC.h"

using time_duration = boost::posix_time::time_duration;
const time_duration TwoSeconds = boost::posix_time::seconds(1);
const  time_duration SixSeconds = boost::posix_time::seconds(6);
const  time_duration TwoMinutes = boost::posix_time::seconds(120);

namespace SvWsl
{

SVRCClientService::SVRCClientService(SvRpc::RPCClient& rpc_client)
	:m_get_deviceMode_client(rpc_client),
	m_set_deviceMode_client(rpc_client)
{

}


SVRCClientService::~SVRCClientService()
{
}


void SVRCClientService::GetDeviceMode(SvPb::GetDeviceModeRequest&& req, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
{
	m_get_deviceMode_client.request(std::move(req), task, SixSeconds);
}

void SVRCClientService::SetDeviceMode(SvPb::SetDeviceModeRequest&& req, SvRpc::Task<SvPb::SetDeviceModeResponse> task)
{
	m_set_deviceMode_client.request(std::move(req), task, TwoMinutes);
}
}