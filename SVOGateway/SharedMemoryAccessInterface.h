//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Pure virtual interface that allows plugging either the real shared memory
/// access class or a mock which returns dummy data only.
//******************************************************************************

#pragma once
#include "SVProtobuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#include "SVRPCLibrary/Task.h"
#include "SVSharedMemoryLibrary/ShareControl.h"

namespace SvOgw
{

class SharedMemoryAccessInterface
{
public:
	virtual ~SharedMemoryAccessInterface() {}

	virtual void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) = 0;
	virtual void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>) = 0;
	virtual void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>) = 0;
	virtual void GetImageFromCurId(const SvPb::GetImageFromCurIdRequest&, SvRpc::Task<SvPb::GetImageFromCurIdResponse>) = 0;
	virtual void GetImageStreamFromCurId(const SvPb::GetImageStreamFromCurIdRequest&,
		SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse>, SvRpc::ServerStreamContext::Ptr) = 0;
	virtual void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>) = 0;
	virtual void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>) = 0;
	virtual void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>) = 0;
	virtual void GetNotificationStream(const SvPb::GetNotificationStreamRequest&,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse>, SvRpc::ServerStreamContext::Ptr) = 0;
};
}// namespace SvOgw
