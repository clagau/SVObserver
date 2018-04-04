#pragma once
#include "SVProtobuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/RPCCLient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVRPCLibrary/Task.h"


namespace SvWsl
{
class SVRCClientService
{
public:
	SVRCClientService(SvRpc::RPCClient& rpc_client);
	~SVRCClientService();
	void GetDeviceMode(SvPb::GetDeviceModeRequest&&, SvRpc::Task<SvPb::GetDeviceModeResponse> task);
	void SetDeviceMode(SvPb::SetDeviceModeRequest&&, SvRpc::Task< SvPb::SetDeviceModeResponse> task);
private:
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetDeviceModeRequest, SvPb::GetDeviceModeResponse> m_get_deviceMode_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetDeviceModeRequest, SvPb::SetDeviceModeResponse> m_set_deviceMode_client;
};

}