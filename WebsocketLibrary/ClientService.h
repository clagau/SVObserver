//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ClientService.h
/// All Rights Reserved
//******************************************************************************
/// Glue code that takes care of calling the RPCClient, especially wrapping of
/// the given request classes into SVRPC::envelopes and unwrapping the response
/// SVRPC::Envelopes.
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

namespace RRWS
{
class ClientService
{
public:
	ClientService(SVRPC::RPCClient& rpc_client, boost::posix_time::time_duration request_timeout);

	void getVersion(GetVersionRequest&& req, SVRPC::Task<GetVersionResponse> task);
	void queryListName(QueryListNameRequest&& req, SVRPC::Task<QueryListNameResponse> task);
	void queryListItem(QueryListItemRequest&& req, SVRPC::Task<QueryListItemResponse> task);
	void getProduct(GetProductRequest&& req, SVRPC::Task<GetProductResponse> task);
	void getImageFromCurId(GetImageFromCurIdRequest&& req, SVRPC::Task<GetImageFromCurIdResponse> task);
	void getImageStreamFromCurId(GetImageStreamFromCurIdRequest&& req,
		SVRPC::Observer<GetImageStreamFromCurIdResponse> observer);
	void getFailStatus(GetFailStatusRequest&& req, SVRPC::Task<GetFailStatusResponse> task);
	void getItems(GetItemsRequest&& req, SVRPC::Task<GetItemsResponse> task);
	void setMode(SetModeRequest&& req, SVRPC::Task<SetModeResponse> task);
	SVRPC::ClientStreamContext getNotificationStream(GetNotificationStreamRequest&& req,
		SVRPC::Observer<GetNotificationStreamResponse> observer);

private:
	boost::posix_time::time_duration m_request_timeout;
	// keep all clients as members so we do have to initialize them only once
	SVRPC::SimpleClient<RunRejectMessages, GetVersionRequest, GetVersionResponse> m_get_version_client;
	SVRPC::SimpleClient<RunRejectMessages, QueryListNameRequest, QueryListNameResponse> m_query_list_name_client;
	SVRPC::SimpleClient<RunRejectMessages, QueryListItemRequest, QueryListItemResponse> m_query_list_item_client;
	SVRPC::SimpleClient<RunRejectMessages, GetProductRequest, GetProductResponse> m_get_product_client;
	SVRPC::SimpleClient<RunRejectMessages, GetImageFromCurIdRequest, GetImageFromCurIdResponse>
		m_get_image_from_cur_id_client;
	SVRPC::SimpleClient<RunRejectMessages, GetImageStreamFromCurIdRequest, GetImageStreamFromCurIdResponse>
		m_get_image_stream_from_cur_id_client;
	SVRPC::SimpleClient<RunRejectMessages, GetFailStatusRequest, GetFailStatusResponse> m_get_fail_status_client;
	SVRPC::SimpleClient<RunRejectMessages, GetItemsRequest, GetItemsResponse> m_get_items_client;
	SVRPC::SimpleClient<RunRejectMessages, SetModeRequest, SetModeResponse> m_set_mode_client;
	SVRPC::SimpleClient<RunRejectMessages, GetNotificationStreamRequest, GetNotificationStreamResponse>
		m_notification_stream_client;
};

using ClientServicePointer = std::unique_ptr<RRWS::ClientService>;
}
