//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file TcpConnection.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "stdafx.h"
#include "TcpConnection.h"

namespace RRApi
{
		void dispatch(const MessageEnvelope& reqEnvelope, MessageEnvelope& resEnvelope, IRRServer* iserver);

		TcpConnection::TcpConnection(boost::asio::io_service& io_service, IRRServer* iserver) 
			: m_socket(io_service), m_server(iserver)
		{
		}

		boost::asio::ip::tcp::socket& TcpConnection::socket()
		{
			return m_socket;
		}

		void  TcpConnection::start()
		{
			read_header();
		}

	
		void TcpConnection::read_header()
		{
			boost::asio::async_read(
				m_socket,
				boost::asio::buffer(m_buf_header),
				std::bind(&TcpConnection::handle_header_read, this, std::placeholders::_1, std::placeholders::_2));
		}

		void TcpConnection::handle_header_read(const boost::system::error_code& error, size_t bytes_read)
		{
			if (error == boost::asio::error::eof)
			{
				std::cout << "client disconnected :(" << std::endl;
				return;
			}

			if (error)
			{
				std::cerr << "client connection error: " << error << std::endl;
				return;
			}

			read_envelope(m_buf_header[0]);
		}

		void TcpConnection::read_envelope(int size)
		{
			m_buf_envelope.assign(size, '\0');
			boost::asio::async_read(
				m_socket,
				boost::asio::buffer(m_buf_envelope),
				boost::asio::transfer_exactly(size),
				std::bind(&TcpConnection::handle_envelope_read, this, std::placeholders::_1, std::placeholders::_2));
		}

		void TcpConnection::handle_envelope_read(const boost::system::error_code& error, size_t bytes_read)
		{
			if (error == boost::asio::error::eof)
			{
				std::cout << "client disconnected :(" << std::endl;
				return;
			}

			if (error)
			{
				std::cerr << "client connection error: " << error << std::endl;
				return;
			}

			MessageEnvelope reqEnvelope;
			reqEnvelope.ParseFromArray(m_buf_envelope.data(), static_cast<int>(m_buf_envelope.size()));
			MessageEnvelope resEnvelope;
			dispatch(reqEnvelope, resEnvelope, m_server);

			auto resSize = resEnvelope.ByteSize();
			m_buf_envelope.assign(resSize, '\0');
			resEnvelope.SerializeToArray(m_buf_envelope.data(), resSize);
			m_buf_header[0] = resSize;

			boost::asio::async_write(
				m_socket,
				boost::array<boost::asio::const_buffer, 2>{boost::asio::buffer(m_buf_header),
				boost::asio::buffer(m_buf_envelope.data(), resSize)},
				std::bind(&TcpConnection::handle_response_sent, this, std::placeholders::_1, std::placeholders::_2));
		}

		void TcpConnection::handle_response_sent(const boost::system::error_code& error, size_t bytes_sent)
		{
			if (error == boost::asio::error::eof)
			{
				std::cout << "client disconnected :(" << std::endl;
				return;
			}

			read_header();
		}

}