//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RRSSharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Pure virtual interface that allows plugging either the real shared memory
/// access class or a mock which returns dummy data only.
//******************************************************************************

#pragma once
#pragma warning (push,2)
#include "SVProtoBuf/RunRe.pb.h"
#pragma warning (pop)
#include "SVSharedMemoryLibrary/ShareControl.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/Task.h"

namespace RRWS
{

class RRSSharedMemoryAccessInterface
{
public:
	virtual ~RRSSharedMemoryAccessInterface() {}

	virtual void GetVersion(const GetVersionRequest&, SVRPC::Task<GetVersionResponse>) = 0;
	virtual void GetProduct(const GetProductRequest&, SVRPC::Task<GetProductResponse>) = 0;
	virtual void GetFailstatus(const GetFailStatusRequest&, SVRPC::Task<GetFailStatusResponse>) = 0;
	virtual void GetImageFromCurId(const GetImageFromCurIdRequest&, SVRPC::Task<GetImageFromCurIdResponse>) = 0;
	virtual void GetImageStreamFromCurId(const GetImageStreamFromCurIdRequest&,
		SVRPC::Observer<GetImageStreamFromCurIdResponse>) = 0;
	virtual void GetItems(const GetItemsRequest&, SVRPC::Task<GetItemsResponse>) = 0;
	virtual void QueryListName(const QueryListNameRequest&, SVRPC::Task<QueryListNameResponse>) = 0;
	virtual void QueryListItem(const QueryListItemRequest&, SVRPC::Task<QueryListItemResponse>) = 0;
};
}