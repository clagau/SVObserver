//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RRSSharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Class for read-only access to SVObserver's shared memory.
//******************************************************************************

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include "RRSSharedMemoryAccessInterface.h"
#pragma warning (push,2)
#include "SVProtoBuf/RunRe.pb.h"
#pragma warning (pop)
#include "SVSharedMemoryLibrary/ShareControl.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/Task.h"

namespace RRWS
{
class RRSSharedMemoryAccess : public RRSSharedMemoryAccessInterface
{
public:
	RRSSharedMemoryAccess();
	~RRSSharedMemoryAccess();

public:
	void GetVersion(const GetVersionRequest&, SVRPC::Task<GetVersionResponse>) override;
	void GetProduct(const GetProductRequest&, SVRPC::Task<GetProductResponse>) override;
	void GetFailstatus(const GetFailStatusRequest&, SVRPC::Task<GetFailStatusResponse>) override;
	void GetImageFromCurId(const GetImageFromCurIdRequest&, SVRPC::Task<GetImageFromCurIdResponse>) override;
	void GetImageStreamFromCurId(const GetImageStreamFromCurIdRequest&,
		SVRPC::Observer<GetImageStreamFromCurIdResponse>) override;
	void GetItems(const GetItemsRequest&, SVRPC::Task<GetItemsResponse>) override;
	void QueryListName(const QueryListNameRequest&, SVRPC::Task<QueryListNameResponse>) override;
	void QueryListItem(const QueryListItemRequest&, SVRPC::Task<QueryListItemResponse>) override;

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_io_work;
	boost::thread m_io_thread;
	GetImageStreamFromCurIdResponse m_unsent_resp;
	std::unique_ptr<SvSml::ShareControl> m_pShareControl;
};
}
