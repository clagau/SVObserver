//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Class for read-only access to SVObserver's shared memory.
//******************************************************************************

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include "SharedMemoryAccessInterface.h"
#include "SVProtobuf/SVRC.h"
#include "SVSharedMemoryLibrary/ShareControl.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/Task.h"

namespace SvOgw
{
class SharedMemoryAccess : public SharedMemoryAccessInterface
{
public:
	SharedMemoryAccess();
	~SharedMemoryAccess();

public:
	void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) override;
	void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>) override;
	void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>) override;
	void GetImageFromCurId(const SvPb::GetImageFromCurIdRequest&, SvRpc::Task<SvPb::GetImageFromCurIdResponse>) override;
	void GetImageStreamFromCurId(const SvPb::GetImageStreamFromCurIdRequest&,
		SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse>, SvRpc::ServerStreamContext::Ptr) override;
	void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>) override;
	void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>) override;
	void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>) override;
	void GetNotificationStream(const SvPb::GetNotificationStreamRequest&,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse>, SvRpc::ServerStreamContext::Ptr) override;

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_io_work;
	boost::thread m_io_thread;
	SvPb::GetImageStreamFromCurIdResponse m_unsent_resp;
	std::unique_ptr<SvSml::ShareControl> m_pShareControlInstance;
	
};
}// namespace SvOgw
