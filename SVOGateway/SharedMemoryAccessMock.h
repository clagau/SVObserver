//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// A class that implements the SharedMemoryAccessInterface and returns
/// dummy data instead of connecting to the shared memory.
//******************************************************************************

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include "SharedMemoryAccessInterface.h"

namespace SvOgw
{

class SharedMemoryAccessMock : public SharedMemoryAccessInterface
{
public:
	SharedMemoryAccessMock();
	virtual ~SharedMemoryAccessMock();

	void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) override;
	void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>) override;
	void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>) override;
	void GetImageFromId(const SvPb::GetImageFromIdRequest&, SvRpc::Task<SvPb::GetImageFromIdResponse>) override;
	void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>) override;
	void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>) override;
	void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>) override;
	void StoreClientLogs(const SvPb::StoreClientLogsRequest&, SvRpc::Task<SvPb::EmptyResponse>) override;

private:
	void getProduct(SvPb::Product& product, bool name_in_response);
	void getImageById(SvPb::Image& img, const SvPb::ImageId& imageId, bool rotated = false);

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_io_work;
	boost::thread m_io_thread;
};
}// namespace SvOgw
