//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RRSSharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// A class that implements the RRSSharedMemoryAccessInterface and returns
/// dummy data instead of connecting to the shared memory.
//******************************************************************************

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include "RRSSharedMemoryAccessInterface.h"

namespace RRWS
{

class RRSSharedMemoryAccessMock : public RRSSharedMemoryAccessInterface
{
public:
	RRSSharedMemoryAccessMock();
	virtual ~RRSSharedMemoryAccessMock();

	void GetVersion(const GetVersionRequest&, SVRPC::Task<GetVersionResponse>) override;
	void GetProduct(const GetProductRequest&, SVRPC::Task<GetProductResponse>) override;
	void GetFailstatus(const GetFailStatusRequest&, SVRPC::Task<GetFailStatusResponse>) override;
	void GetImageFromCurId(const GetImageFromCurIdRequest&, SVRPC::Task<GetImageFromCurIdResponse>) override;
	void GetImageStreamFromCurId(const GetImageStreamFromCurIdRequest&,
		SVRPC::Observer<GetImageStreamFromCurIdResponse>, SVRPC::ServerStreamContext::Ptr) override;
	void GetItems(const GetItemsRequest&, SVRPC::Task<GetItemsResponse>) override;
	void QueryListName(const QueryListNameRequest&, SVRPC::Task<QueryListNameResponse>) override;
	void QueryListItem(const QueryListItemRequest&, SVRPC::Task<QueryListItemResponse>) override;
	void GetNotificationStream(const GetNotificationStreamRequest&,
		SVRPC::Observer<GetNotificationStreamResponse>, SVRPC::ServerStreamContext::Ptr) override;

private:
	void getProduct(Product& product, bool name_in_response);
	void getImageById(RRWS::Image& img, const RRWS::CurImageId& imageId, bool rotated = false);
	void getImageStreamFromCurIdStep(int iterations,
		const CurImageId& id,
		SVRPC::Observer<GetImageStreamFromCurIdResponse> observer,
		SVRPC::ServerStreamContext::Ptr ctx);
	void getNotificationStreamImpl(
		SVRPC::Observer<GetNotificationStreamResponse> observer,
		SVRPC::ServerStreamContext::Ptr ctx);

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_io_work;
	boost::thread m_io_thread;
};
}