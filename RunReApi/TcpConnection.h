//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file TcpConnection.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
#include "ServerFrontEndApi.h"
namespace RRApi
{
	class TcpConnection
	{
	public:
		TcpConnection(boost::asio::io_service& io_service, IRRServer* server);
		boost::asio::ip::tcp::socket& socket();
		void start();
	private:
		void read_header();
		void handle_header_read(const boost::system::error_code& error, size_t bytes_read);
		void read_envelope(int size);
		void handle_envelope_read(const boost::system::error_code& error, size_t bytes_read);
		void handle_response_sent(const boost::system::error_code& error, size_t bytes_sent);
	private:
		boost::asio::ip::tcp::socket m_socket;
		boost::array<int, 1> m_buf_header;
		std::vector<char> m_buf_envelope;
		IRRServer* m_server;
	};
}

