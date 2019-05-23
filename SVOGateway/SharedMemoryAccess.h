//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Class for read-only access to SVObserver's shared memory.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/asio/io_service.hpp>
//Moved to precompiled header: #include <boost/thread.hpp>

#include "SharedMemoryAccessInterface.h"
#include "SVProtobuf/SVRC.h"
#include "SVSharedMemoryLibrary/ShareControl.h"
#include "SVRPCLibrary/Task.h"
#pragma endregion Includes

namespace SvSml
{
struct ShareControlSettings;
}

namespace SvOgw
{
class SharedMemoryAccess : public SharedMemoryAccessInterface
{
public:
	SharedMemoryAccess(const SvSml::ShareControlSettings& ControlParameter);
	~SharedMemoryAccess();

public:
	void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) override;
	void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>) override;
	void GetReject(const SvPb::GetRejectRequest&, SvRpc::Task<SvPb::GetRejectResponse>) override;
	void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>) override;
	void GetImageFromId(const SvPb::GetImageFromIdRequest&, SvRpc::Task<SvPb::GetImageFromIdResponse>) override;
	void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>) override;
	void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>) override;
	void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>) override;
	void StoreClientLogs(const SvPb::StoreClientLogsRequest&, SvRpc::Task<SvPb::EmptyResponse>) override;

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_io_work;
	boost::thread m_io_thread;
	std::unique_ptr<SvSml::ShareControl> m_pShareControlInstance;
};
}// namespace SvOgw
