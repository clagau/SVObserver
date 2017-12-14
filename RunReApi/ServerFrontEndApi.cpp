//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ServerFrontEndApi.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "StdAfx.h"
#include "ServerFrontendApi.h" 
#include "TcpConnection.h"
namespace RRApi
{
	ServerFrontEndApi::ServerFrontEndApi(std::unique_ptr<ServerFrontEndApi_Impl>&& pimpl) : 
		m_pImpl(std::move(pimpl))
	{


	}
	ServerFrontEndApi::~ServerFrontEndApi() = default;
	
	std::unique_ptr<ServerFrontEndApi> ServerFrontEndApi::Init(boost::asio::io_service& io_service, IRRServer* server, uint16_t port)
	{
		auto pimpl = std::make_unique<ServerFrontEndApi_Impl>(io_service,server,port);
		return std::unique_ptr<ServerFrontEndApi>(new ServerFrontEndApi(std::move(pimpl)));
	 }

	struct ServerFrontEndApi::ServerFrontEndApi_Impl
	{
		boost::asio::io_service& io_service_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::shared_ptr<TcpConnection> connection_;
		IRRServer* iserver;

		ServerFrontEndApi_Impl(boost::asio::io_service& io_service, IRRServer* pServer, uint16_t port)
			: io_service_(io_service),
			iserver(pServer),
			acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		{
			start_accept();
		}
	private:
		void start_accept()
		{
			auto new_connection = std::make_shared<TcpConnection>(io_service_, iserver);

			acceptor_.async_accept(new_connection->socket(),
				std::bind(&ServerFrontEndApi_Impl::handle_accept, this, new_connection, std::placeholders::_1));
		}

		void handle_accept(std::shared_ptr<TcpConnection> new_connection, const boost::system::error_code& error)
		{
			if (!error)
			{
				connection_ = new_connection;
				connection_->start();
				start_accept();
			}
		}

	};
		

}