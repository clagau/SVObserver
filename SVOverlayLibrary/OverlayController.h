//******************************************************************************
/// \copyright (c) 2017,2019 by Seidenader Maschinenbau GmbH
/// \file OverlayController.h
/// All Rights Reserved
//******************************************************************************

#pragma once

#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/Overlay.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVSystemLibrary/SVFuture.h"

namespace SvOv
{
class OverlayController
{
public:
	OverlayController(boost::asio::io_context& io_context, SvRpc::RPCClient& rpcClient);

	SvSyl::SVFuture<SvPb::OverlayDesc> getOverlays(GUID inspectionId, GUID imageId);

public:
	boost::asio::io_context& m_io_context;
	SvRpc::RPCClient& m_rpc_client;
	boost::posix_time::seconds m_timeout {2};
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetSVObserverVersionRequest, SvPb::GetVersionResponse> m_version_client;
};

} // namespace SvRpc
