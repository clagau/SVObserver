//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ClientFrontEndApi.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "StdAfx.h"
#include "ClientFrontEndApi.h"
#include "Helper.h"
#include <future>
//#include <boost\thread\future.hpp>
namespace RRApi
{
	ClientFrontEndApi::ClientFrontEndApi(std::unique_ptr<ClientFrontEndApi_Impl>&& pimpl) :
		m_pImpl(std::move(pimpl))
	{


	}
	ClientFrontEndApi::~ClientFrontEndApi() = default;

	std::unique_ptr<ClientFrontEndApi> ClientFrontEndApi::Init(boost::asio::io_service& io_service /*, uint16_t port, const std::string &ipadress */)
	{
		auto pimpl = std::make_unique<ClientFrontEndApi_Impl>(io_service /*,  port, ipadress*/);
		return std::unique_ptr<ClientFrontEndApi>(new ClientFrontEndApi(std::move(pimpl)));
	}

	
	struct ClientFrontEndApi::ClientFrontEndApi_Impl
	{
		using MessageEnvelopeConsumer = std::function<void(const MessageEnvelope&)>;
		boost::asio::io_service& io_service_;
		boost::asio::ip::tcp::socket socket_;
		std::map<int, MessageEnvelopeConsumer> pending_requests_;
		boost::array<int, 1> buf_header_;
		std::string buf_envelope_;
		std::vector<char> buf_envelope2_;
		bool m_isConnected = false;

		ClientFrontEndApi_Impl(boost::asio::io_service& io_service/*, uint16_t port, const std::string& ipAdress*/  ) 
			: io_service_(io_service), socket_(io_service)
		{
			boost::asio::ip::address host;
			/*if (ipAdress.size() == 0)
				host = boost::asio::ip::address::from_string("127.0.0.1");
			else
				host = boost::asio::ip::address::from_string(ipAdress);

			auto endpoint = boost::asio::ip::tcp::endpoint(host, port);
			socket_.connect(endpoint);*/
		}
		
		bool IsConnected()
		{
			return m_isConnected;
		}
		long  GetClientId()
		{
			boost::system::error_code ec;
			auto res = socket_.local_endpoint(ec);
			if (ec)
			{
				return 0;
			}
			else
			{
				return res.address().to_v4().to_ulong();
			}
		}

		void Connect(uint16_t port, const std::string& ipAdress) 
		{
			try
			{
				boost::asio::ip::address host;
				if (ipAdress.size() == 0)
					host = boost::asio::ip::address::from_string("127.0.0.1");
				else
					host = boost::asio::ip::address::from_string(ipAdress);

				auto endpoint = boost::asio::ip::tcp::endpoint(host, port);
				socket_.connect(endpoint);
				m_isConnected = true;
			}
			catch (boost::system::system_error &e)
			{
				std::cerr << "Connect Ecode =" << e.code() << " Message: " << e.what() << std::endl;
				m_isConnected = false;
			}
			
		}
		void DisConnect()
		{
			
			//return;
			try
			{
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
				socket_.close();
			}
			catch (boost::system::system_error &e)
			{
				std::cerr << "DisConnect Ecode =" << e.code() << " Message: " << e.what() << std::endl;
			}
			m_isConnected = false;
		
		}

		void sendRequest(MessageEnvelope& envelope, MessageEnvelopeConsumer cb)
		{
			pending_requests_[envelope.id()] = cb;
			envelope.SerializeToString(&buf_envelope_);
			buf_header_[0] = envelope.ByteSize();

			boost::asio::async_write(socket_,
				boost::array<boost::asio::const_buffer, 2>{boost::asio::buffer(buf_header_),
				boost::asio::buffer(buf_envelope_)},
				std::bind(&ClientFrontEndApi_Impl::handle_write, this, std::placeholders::_1, std::placeholders::_2));
		}

	private:
		void handle_write(const boost::system::error_code& error, size_t bytes_written)
		{
			if (error)
			{
				std::cerr << "client connection error: " << error << std::endl;
				return;
			}

			boost::asio::async_read(
				socket_,
				boost::asio::buffer(buf_header_),
				std::bind(&ClientFrontEndApi_Impl::handle_header_read, this, std::placeholders::_1, std::placeholders::_2));
		}

		void handle_header_read(const boost::system::error_code& error, size_t bytes_read)
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

			read_envelope(buf_header_[0]);
		}

		void read_envelope(int size)
		{
			buf_envelope2_.assign(size, '\0');
			boost::asio::async_read(
				socket_,
				boost::asio::buffer(buf_envelope2_),
				boost::asio::transfer_exactly(size),
				std::bind(&ClientFrontEndApi_Impl::handle_envelope_read, this, std::placeholders::_1, std::placeholders::_2));
		}

		void handle_envelope_read(const boost::system::error_code& error, size_t bytes_read)
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

			MessageEnvelope envelope;
			envelope.ParseFromArray(buf_envelope2_.data(), static_cast<int>(bytes_read));
			auto id = envelope.id();
			auto it = pending_requests_.find(id);
			if (it == pending_requests_.end())
			{
				std::cerr << "received response envelope for unknown id " << id << std::endl;
				return;
			}

			it->second(envelope);
		}

	};


	std::future<GetVersionResponse> ClientFrontEndApi::GetVersion(const GetVersionRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::GetVersion, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<GetVersionResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			GetVersionResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	
	}
	std::future<GetProductResponse> ClientFrontEndApi::GetProduct(const GetProductRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::GetProduct, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<GetProductResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			GetProductResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();

	}

	std::future<GetProductResponse>  ClientFrontEndApi::GetReject(const GetProductRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::GetReject, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<GetProductResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			GetProductResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	}
	std::future<GetFailStatusResponse> ClientFrontEndApi::GetFailStatus(const GetFailStatusRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::GetFailStatus, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<GetFailStatusResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			GetFailStatusResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	}
	std::future<GetImageFromCurIdResponse> ClientFrontEndApi::GetImageFromCurId(const GetImageFromCurIdRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::GetImageFromCurId, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<GetImageFromCurIdResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			GetImageFromCurIdResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	}
	std::future<GetItemsResponse> ClientFrontEndApi::GetItems(const GetItemsRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::GetItems, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<GetItemsResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			GetItemsResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	}
	std::future<QueryListNameResponse> ClientFrontEndApi::QueryListName(const QueryListNameRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::QueryListName, GetNextMessageId(), req);
		auto promise = std::make_shared<std::promise<QueryListNameResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			QueryListNameResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	
	}
	std::future<QueryListItemResponse> ClientFrontEndApi::QueryListItem(const QueryListItemRequest& req)
	{
		MessageEnvelope reqEnvelope;
		Helper::wrap(reqEnvelope, MessageType::QueryListItem, GetNextMessageId(),req);
		auto promise = std::make_shared<std::promise<QueryListItemResponse>>();
		m_pImpl->sendRequest(reqEnvelope, [promise](const MessageEnvelope& resEnvelope) {
			QueryListItemResponse response;
			Helper::unwrap(resEnvelope, response);
			promise->set_value(response);
		});
		return promise->get_future();
	
	}

	void ClientFrontEndApi::Connect(uint16_t port, const std::string &ipAdress)
	{
		return m_pImpl->Connect(port, ipAdress);
	}

	void ClientFrontEndApi::DisConnect()
	{
		return m_pImpl->DisConnect();
	}

	bool ClientFrontEndApi::IsConnected()
	{
		return m_pImpl->IsConnected();
	}
	long  ClientFrontEndApi::GetClientId()
	{
		return m_pImpl->GetClientId();
		
	}
	
	
}


