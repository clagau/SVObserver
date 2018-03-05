//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ClientService.h
/// All Rights Reserved
//******************************************************************************
/// Glue code for calling the RPCClient.
///
/// TODO: the RPC library should provide most of this file as utils,
///       especially the two impl functions.
//******************************************************************************

#pragma once

#include "stdafx.h"

#include "ClientService.h"
#pragma warning (push,2)
#include  "SVProtoBuf/RunRe.pb.h"
#pragma warning (pop)
using time_duration = boost::posix_time::time_duration;

const time_duration TwoSeconds = boost::posix_time::seconds(1);
const  time_duration FourSeconds = boost::posix_time::seconds(4);

namespace RRWS
{
ClientService::ClientService(SVRPC::RPCClient& rpc_client, boost::posix_time::time_duration request_timeout)
	: m_request_timeout(request_timeout), 
	m_get_version_client(rpc_client), 
	m_query_list_name_client(rpc_client),
	m_query_list_item_client(rpc_client),
	m_get_product_client(rpc_client),
	m_get_image_from_cur_id_client(rpc_client),
	m_get_image_stream_from_cur_id_client(rpc_client),
	m_get_fail_status_client(rpc_client),
	m_get_items_client(rpc_client),
	m_set_mode_client(rpc_client)
{
}

void ClientService::getVersion(GetVersionRequest&& req, SVRPC::Task<GetVersionResponse> task)
{
	m_get_version_client.request(std::move(req), task, TwoSeconds);
}

void ClientService::queryListName(QueryListNameRequest&& req, SVRPC::Task<QueryListNameResponse> task)
{
	m_query_list_name_client.request(std::move(req), task, FourSeconds);
}

void ClientService::queryListItem(QueryListItemRequest&& req, SVRPC::Task<QueryListItemResponse> task)
{
	m_query_list_item_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getProduct(GetProductRequest&& req, SVRPC::Task<GetProductResponse> task)
{
	m_get_product_client.request(std::move(req), task, FourSeconds);
}


void ClientService::getImageFromCurId(GetImageFromCurIdRequest&& req, SVRPC::Task<GetImageFromCurIdResponse> task)
{
	m_get_image_from_cur_id_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getImageStreamFromCurId(GetImageStreamFromCurIdRequest&& req,
	SVRPC::Observer<GetImageStreamFromCurIdResponse> observer)
{
	m_get_image_stream_from_cur_id_client.stream(std::move(req), observer);
}

void ClientService::getFailStatus(GetFailStatusRequest&& req, SVRPC::Task<GetFailStatusResponse> task)
{
	m_get_fail_status_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getItems(GetItemsRequest&& req, SVRPC::Task<GetItemsResponse> task)
{
	m_get_items_client.request(std::move(req), task, FourSeconds);
}

void ClientService::setMode(SetModeRequest&& req, SVRPC::Task<SetModeResponse> task)
{
	m_set_mode_client.request(std::move(req), task, FourSeconds);
}





}
