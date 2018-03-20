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
#include "SVProtoBuf/RunRe.h"

using time_duration = boost::posix_time::time_duration;

const time_duration TwoSeconds = boost::posix_time::seconds(1);
const  time_duration FourSeconds = boost::posix_time::seconds(4);

namespace SvWsl
{
ClientService::ClientService(SvRpc::RPCClient& rpc_client, boost::posix_time::time_duration request_timeout)
	: m_request_timeout(request_timeout), 
	m_get_version_client(rpc_client), 
	m_query_list_name_client(rpc_client),
	m_query_list_item_client(rpc_client),
	m_get_product_client(rpc_client),
	m_get_image_from_cur_id_client(rpc_client),
	m_get_image_stream_from_cur_id_client(rpc_client),
	m_get_fail_status_client(rpc_client),
	m_get_items_client(rpc_client),
	m_set_mode_client(rpc_client),
	m_notification_stream_client(rpc_client)
{
}

void ClientService::getVersion(SvPb::GetVersionRequest&& req, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	m_get_version_client.request(std::move(req), task, TwoSeconds);
}

void ClientService::queryListName(SvPb::QueryListNameRequest&& req, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	m_query_list_name_client.request(std::move(req), task, FourSeconds);
}

void ClientService::queryListItem(SvPb::QueryListItemRequest&& req, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	m_query_list_item_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getProduct(SvPb::GetProductRequest&& req, SvRpc::Task<SvPb::GetProductResponse> task)
{
	m_get_product_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getImageFromCurId(SvPb::GetImageFromCurIdRequest&& req, SvRpc::Task<SvPb::GetImageFromCurIdResponse> task)
{
	m_get_image_from_cur_id_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getImageStreamFromCurId(SvPb::GetImageStreamFromCurIdRequest&& req,
	SvRpc::Observer<SvPb::GetImageStreamFromCurIdResponse> observer)
{
	m_get_image_stream_from_cur_id_client.stream(std::move(req), observer);
}

void ClientService::getFailStatus(SvPb::GetFailStatusRequest&& req, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	m_get_fail_status_client.request(std::move(req), task, FourSeconds);
}

void ClientService::getItems(SvPb::GetItemsRequest&& req, SvRpc::Task<SvPb::GetItemsResponse> task)
{
	m_get_items_client.request(std::move(req), task, FourSeconds);
}

void ClientService::setMode(SvPb::SetModeRequest&& req, SvRpc::Task<SvPb::SetModeResponse> task)
{
	m_set_mode_client.request(std::move(req), task, FourSeconds);
}

SvRpc::ClientStreamContext ClientService::getNotificationStream(SvPb::GetNotificationStreamRequest&& req,
	SvRpc::Observer<SvPb::GetNotificationStreamResponse> observer)
{
	return m_notification_stream_client.stream(std::move(req), observer);
}

}
