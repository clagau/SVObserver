//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ClientService.h
/// All Rights Reserved
//******************************************************************************
/// Glue code that takes care of calling the RPCClient, especially wrapping of
/// the given request classes into SvPenv::Envelopes and unwrapping the response
/// SvPenv::Envelopes.
/// All requests are called with the provided request timeout. Streams are not
/// using the timeout.
//******************************************************************************

#pragma once

#include "SVProtobuf/RunRe.h"
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
	ClientService(SvRpc::RPCClient& rpc_client, boost::posix_time::time_duration request_timeout);

	void getVersion(SvPb::GetVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task);
	void queryListName(SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task);
	void queryListItem(SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task);
	void getProduct(SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task);
	void getImageFromCurId(SvPb::GetImageFromCurIdRequest&& req, SvRpc::Task<SvPb::GetImageFromCurIdResponse> task);
	void getImageStreamFromCurId(SvPb::GetImageStreamFromCurIdRequest&& req,
		SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse> observer);
	void getFailStatus(SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task);
	void getItems(SvPb::GetItemsRequest&& req, SvRpc::Task<SvPb::GetItemsResponse> task);
	void setMode(SvPb::SetModeRequest&& req, SvRpc::Task<SvPb::SetModeResponse> task);
	SvRpc::ClientStreamContext getNotificationStream(SvPb::GetNotificationStreamRequest&& req,
		SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer);

private:
	boost::posix_time::time_duration m_request_timeout;
	// keep all clients as members so we do have to initialize them only once
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetVersionRequest, SvPb::GetVersionResponse> m_get_version_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::QueryListNameRequest, SvPb::QueryListNameResponse> m_query_list_name_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::QueryListItemRequest, SvPb::QueryListItemResponse> m_query_list_item_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetProductRequest, SvPb::GetProductResponse> m_get_product_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetImageFromCurIdRequest, SvPb::GetImageFromCurIdResponse>
		m_get_image_from_cur_id_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetImageStreamFromCurIdRequest, SvPb::GetImageStreamFromCurIdResponse>
		m_get_image_stream_from_cur_id_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetFailStatusRequest, SvPb::GetFailStatusResponse> m_get_fail_status_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetItemsRequest, SvPb::GetItemsResponse> m_get_items_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::SetModeRequest, SvPb::SetModeResponse> m_set_mode_client;
	SvRpc::SimpleClient<SvPb::RunRejectMessages, SvPb::GetNotificationStreamRequest, SvPb::GetNotificationStreamResponse> m_notification_stream_client;
};

using ClientServicePointer = std::unique_ptr<SvWsl::ClientService>;
}
