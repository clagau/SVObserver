#pragma once
//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ClientFrontEndApi.h
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
	class ClientFrontEndApi
	{
	public:
		struct ClientFrontEndApi_Impl;

		ClientFrontEndApi(std::unique_ptr<ClientFrontEndApi_Impl>&&);
		~ClientFrontEndApi();
		static std::unique_ptr<ClientFrontEndApi> Init(boost::asio::io_service& io_service /*,  uint16_t port,const std::string &ipAdress*/);
		void Connect(uint16_t port, const std::string &ipAdress);
		void DisConnect();
		bool IsConnected();
		long  GetClientId();


	public:
		std::future<GetVersionResponse> GetVersion(const GetVersionRequest&) ;
		std::future<GetProductResponse> GetProduct(const GetProductRequest&);
		std::future<GetProductResponse> GetReject(const GetProductRequest&);
		std::future<GetFailStatusResponse> GetFailStatus(const GetFailStatusRequest&);
		std::future<GetImageFromCurIdResponse> GetImageFromCurId(const GetImageFromCurIdRequest&);
		std::future<GetItemsResponse> GetItems(const GetItemsRequest&);
		std::future<QueryListNameResponse> QueryListName(const QueryListNameRequest&);
		std::future<QueryListItemResponse> QueryListItem(const QueryListItemRequest&);
	
	private:
		int GetNextMessageId()
		{
			static int MessageId = 0;
			return ++MessageId;
		}
		
		std::unique_ptr<ClientFrontEndApi_Impl> m_pImpl;
	};
}
