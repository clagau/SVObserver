//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ClientService.h
/// All Rights Reserved
//******************************************************************************
/// Glue code that takes care of calling the RPCClient, especially wrapping of
/// the given request classes into SvPenv::Envelopes and unwrapping the response
/// SvPenv::Envelopes.
/// Streams are not using the timeout.
//******************************************************************************

#pragma once

#include "SVProtobuf/SVRC.h"
#include "SVRPCLibrary/ClientStreamContext.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/RPCCLient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVRPCLibrary/Task.h"

namespace SvWsl
{
class ClientService
{
public:
	ClientService(SvRpc::RPCClient& rpc_client);

	void getVersion(SvPb::GetVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task);
	void queryListName(SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task);
	void queryListItem(SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task);
	void getProduct(SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task);
	void getImageFromCurId(SvPb::GetImageFromCurIdRequest&& req, SvRpc::Task<SvPb::GetImageFromCurIdResponse> task);
	void getImageStreamFromCurId(SvPb::GetImageStreamFromCurIdRequest&& req,
		SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse> observer);
	void getFailStatus(SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task);
	void getItems(SvPb::GetItemsRequest&& req, SvRpc::Task<SvPb::GetItemsResponse> task);
	SvRpc::ClientStreamContext getNotificationStream(SvPb::GetNotificationStreamRequest&& req,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer);

private:
	// keep all clients as members so we do have to initialize them only once
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetVersionRequest, SvPb::GetVersionResponse> m_get_version_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::QueryListNameRequest, SvPb::QueryListNameResponse> m_query_list_name_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::QueryListItemRequest, SvPb::QueryListItemResponse> m_query_list_item_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetProductRequest, SvPb::GetProductResponse> m_get_product_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetImageFromCurIdRequest, SvPb::GetImageFromCurIdResponse>
		m_get_image_from_cur_id_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetImageStreamFromCurIdRequest, SvPb::GetImageStreamFromCurIdResponse>
		m_get_image_stream_from_cur_id_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetFailStatusRequest, SvPb::GetFailStatusResponse> m_get_fail_status_client;
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetItemsRequest, SvPb::GetItemsResponse> m_get_items_client;
	
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetNotificationStreamRequest, SvPb::GetNotificationStreamResponse> m_notification_stream_client;
};

using ClientServicePointer = std::unique_ptr<SvWsl::ClientService>;
}
