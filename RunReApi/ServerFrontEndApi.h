//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ServerFrontEndApi.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
#include <boost/asio/io_service.hpp>
#include <thread>
#include <future>
#include "format.pb.h"
namespace RRApi
{
	class IRRServer
	{
	public:
		virtual void GetVersion(const GetVersionRequest&, GetVersionResponse&) = 0;
		virtual void GetProduct(const GetProductRequest&, GetProductResponse&) = 0;
		virtual void GetReject(const GetProductRequest&, GetProductResponse&) = 0;
		virtual void GetFailstatus(const GetFailStatusRequest&, GetFailStatusResponse&) = 0;
		virtual void GetImageFromCurId(const GetImageFromCurIdRequest&, GetImageFromCurIdResponse&) = 0;
		virtual void GetItems(const GetItemsRequest&, GetItemsResponse&) = 0;
		virtual void QueryListName(const QueryListNameRequest&, QueryListNameResponse&) = 0;
		virtual void QueryListItem(const QueryListItemRequest&, QueryListItemResponse&) = 0;
	};
	
	class ServerFrontEndApi
	{
	public:
		struct ServerFrontEndApi_Impl;
		ServerFrontEndApi(std::unique_ptr<ServerFrontEndApi_Impl>&&);
		~ServerFrontEndApi();
		static std::unique_ptr<ServerFrontEndApi> Init(boost::asio::io_service& io_service, IRRServer* server, uint16_t port);
	private:
		std::unique_ptr<ServerFrontEndApi_Impl> m_pImpl;
	};
} 