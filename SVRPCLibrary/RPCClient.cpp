//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCClient.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCClient is a thin wrapper around the WebsocketClient. It assumes all
/// incoming messages are binary and are serialized protobuf envelopes.
//******************************************************************************

#include "stdafx.h"

#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/RPCClient.h"

#include <boost/log/trivial.hpp>

namespace SVRPC
{
RPCClient::RPCClient(std::string host, uint16_t port)
	: m_io_service(1), 
	m_io_work(std::make_unique<boost::asio::io_service::work>(m_io_service)), 
	m_websocket_client_factory(host,port), 
	m_reconnect_timer(m_io_service)
{
	m_io_thread = std::thread([this]() { m_io_service.run(); });
	//m_websocket_client = m_websocket_client_factory.create(m_io_service, this);
}

RPCClient::~RPCClient()
{
	stop();
}

void RPCClient::Connect(std::string host, uint16_t port)
{
	if (false == m_connected.load())
	{
		m_websocket_client_factory.m_host = host;
		m_websocket_client_factory.m_port = port;
		//schedule_reconnect(boost::posix_time::seconds(1));
		BOOST_LOG_TRIVIAL(info) << "Trying to connect.";
		m_websocket_client = m_websocket_client_factory.create(m_io_service, this);
		m_TryToReconnect.store(true);
	}
}
void RPCClient::Disconnect()
{
	m_connected.store(false);
	BOOST_LOG_TRIVIAL(info) << "RPCClient Disconnect";
	cancel_all_pending();
	if(m_websocket_client.get())
	m_websocket_client->disconnect();
	m_TryToReconnect.store(false);
	
	//m_websocket_client.reset();
}

void RPCClient::stop()
{
	if (m_stopped.load())
	{
		return;
	}

	BOOST_LOG_TRIVIAL(debug) << "Shut down of rpc client started";
	if (m_connected && m_websocket_client)
	{
		m_websocket_client->disconnect();
	}
	m_io_work.reset();
	if (!m_io_service.stopped())
	{
		m_io_service.stop();
	}
	if (m_io_thread.joinable())
	{
		m_io_thread.join();
	}
	m_stopped.store(true);
	BOOST_LOG_TRIVIAL(debug) << "Shut down of rpc client completed";
}

bool RPCClient::isConnected()
{
	return m_connected.load();
}

bool  RPCClient::waitForConnect(int time_in_ms)
{
	if (isConnected())
	{
		return true;
	}
	std::chrono::milliseconds time(time_in_ms);

	std::unique_lock<std::mutex> lk(m_connect_mutex);
	m_connect_cv.wait_for(lk,time , [this] { return m_connected.load(); });
	if (!isConnected())
	{
		m_TryToReconnect.store(false);
	}
	return isConnected();
}

void RPCClient::request(Envelope&& request, Task<Envelope> task)
{
	if (!isConnected())
	{
		task.error(build_error(ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_next_transaction_id;
	request_impl(std::move(request), task, tx_id);
}

void RPCClient::request(Envelope&& request, Task<Envelope> task, boost::posix_time::time_duration timeout)
{
	if (!isConnected())
	{
		task.error(build_error(ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_next_transaction_id;
	schedule_timeout(tx_id, timeout);
	request_impl(std::move(request), task, tx_id);
}

void RPCClient::request_impl(Envelope&& request, Task<Envelope> task, uint64_t tx_id)
{
	request.set_transaction_id(tx_id);
	request.set_type(SVRPC::MessageType::Request);

	m_pending_requests.insert({tx_id, task});

	auto reqSize = request.ByteSize();
	std::vector<char> buf;
	buf.assign(reqSize, '\0');
	request.SerializeToArray(buf.data(), reqSize);

	m_websocket_client->sendBinaryMessage(buf);
}

void RPCClient::schedule_timeout(uint64_t tx_id, boost::posix_time::time_duration timeout)
{
	// use shared_ptr because boost::asio::deadline_timer is neither movable nor copyable
	auto timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
	timer->expires_from_now(timeout);
	timer->async_wait(std::bind(&RPCClient::on_request_timeout, this, std::placeholders::_1, tx_id));
	m_pending_requests_timer.emplace(tx_id, timer);
}

void RPCClient::stream(Envelope&& request, Observer<Envelope> observer)
{
	if (!isConnected())
	{
		observer.error(build_error(ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_next_transaction_id;
	request.set_transaction_id(tx_id);
	request.set_type(SVRPC::MessageType::StreamRequest);

	m_pending_streams.insert({tx_id, observer});

	auto reqSize = request.ByteSize();
	std::vector<char> buf;
	buf.resize(reqSize);
	request.SerializeToArray(buf.data(), reqSize);

	m_websocket_client->sendBinaryMessage(buf);
}

void RPCClient::onConnect()
{
	{
		std::lock_guard<std::mutex> lk(m_connect_mutex);
		m_connected.store(true);
	}
	m_connect_cv.notify_all();
}

void RPCClient::onDisconnect()
{
	m_connected.store(false);
	BOOST_LOG_TRIVIAL(info) << "RPCClient received disconnect event. Trying to reconnect.";
	cancel_all_pending();
	//m_websocket_client.reset(); ///!!!! seems to cause an crash
	m_websocket_client->disconnect(); 
	if(m_TryToReconnect.load())
		schedule_reconnect(boost::posix_time::seconds(1));
}

void RPCClient::onTextMessage(const std::vector<char>&)
{
	throw std::runtime_error("only binary messages expected!");
}

void RPCClient::onBinaryMessage(const std::vector<char>& buf)
{
	if (buf.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
	{
		BOOST_LOG_TRIVIAL(error) << "Message too large " << buf.size() << ". Must not be larger than "
			<< std::numeric_limits<int>::max() << ".";
		return;
	}

	Envelope response;
	response.ParseFromArray(buf.data(), static_cast<int>(buf.size()));

	auto type = response.type();
	switch (type)
	{
		case MessageType::Response:
			on_response(std::move(response));
			break;

		case MessageType::ErrorResponse:
			on_error_response(std::move(response));
			break;

		case MessageType::StreamResponse:
			on_stream_response(std::move(response));
			break;

		case MessageType::StreamErrorResponse:
			on_stream_error_response(std::move(response));
			break;

		case MessageType::StreamFinish:
			on_stream_finish(std::move(response));
			break;

		default:
			BOOST_LOG_TRIVIAL(error) << "Invalid message type " << type;
			break;
	}
}

void RPCClient::schedule_reconnect(boost::posix_time::time_duration timeout)
{
	BOOST_LOG_TRIVIAL(debug) << "Scheduling reconnect in " << timeout.seconds() << " seconds.";
	m_reconnect_timer.expires_from_now(timeout);
	m_reconnect_timer.async_wait(std::bind(&RPCClient::on_reconnect, this, std::placeholders::_1));
}

void RPCClient::on_reconnect(const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}

	if (error)
	{
		BOOST_LOG_TRIVIAL(warning) << error;
		return;
	}

	BOOST_LOG_TRIVIAL(info) << "Trying to reconnect.";
	m_websocket_client = m_websocket_client_factory.create(m_io_service, this);
}

void RPCClient::on_request_timeout(const boost::system::error_code& error, uint64_t tx_id)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}

	if (error)
	{
		BOOST_LOG_TRIVIAL(warning) << error;
		return;
	}

	m_pending_requests_timer.erase(tx_id);

	auto it = m_pending_requests.find(tx_id);
	if (it != m_pending_requests.end())
	{
		auto cb = it->second;
		m_pending_requests.erase(it);

		cb.error(build_error(ErrorCode::Timeout));
	}
}

void RPCClient::cancel_request_timeout(uint64_t tx_id)
{
	auto it = m_pending_requests_timer.find(tx_id);
	if (it != m_pending_requests_timer.end())
	{
		auto timer_ptr = it->second;
		m_pending_requests_timer.erase(it);
		timer_ptr->cancel();
	}
}

void RPCClient::cancel_all_pending()
{
	cancel_all_pending_requests();
	cancel_all_pending_streams();
}

void RPCClient::cancel_all_pending_requests()
{
	while (!m_pending_requests.empty())
	{
		auto it = m_pending_requests.begin();
		auto tx_id = it->first;
		cancel_request_timeout(tx_id);
		auto& task = it->second;
		task.error(build_error(ErrorCode::ServiceUnavailable, "Connection lost. Please retry."));
		m_pending_requests.erase(it);
	}
}

void RPCClient::cancel_all_pending_streams()
{
	while (!m_pending_streams.empty())
	{
		auto it = m_pending_streams.begin();
		auto& observer = it->second;
		observer.error(build_error(ErrorCode::ServiceUnavailable, "Connection lost. Please retry."));
		m_pending_streams.erase(it);
	}
}

void RPCClient::on_response(Envelope&& response)
{
	auto tx_id = response.transaction_id();
	cancel_request_timeout(tx_id);
	auto it = m_pending_requests.find(tx_id);
	if (it != m_pending_requests.end())
	{
		auto cb = it->second;
		m_pending_requests.erase(it);
		cb.finish(std::move(response));
	}
}

void RPCClient::on_error_response(Envelope&& response)
{
	auto tx_id = response.transaction_id();
	cancel_request_timeout(tx_id);
	auto it = m_pending_requests.find(tx_id);
	if (it != m_pending_requests.end())
	{
		auto cb = it->second;
		m_pending_requests.erase(it);
		cb.error(response.error());
	}
}

void RPCClient::on_stream_response(Envelope&& response)
{
	auto it = m_pending_streams.find(response.transaction_id());
	if (it != m_pending_streams.end())
	{
		it->second.onNext(std::move(response));
	}
}

void RPCClient::on_stream_error_response(Envelope&& response)
{
	auto it = m_pending_streams.find(response.transaction_id());
	if (it != m_pending_streams.end())
	{
		auto cb = it->second;
		m_pending_streams.erase(it);
		cb.error(response.error());
	}
}

void RPCClient::on_stream_finish(Envelope&& response)
{
	auto it = m_pending_streams.find(response.transaction_id());
	if (it != m_pending_streams.end())
	{
		auto cb = it->second;
		m_pending_streams.erase(it);
		cb.finish();
	}
}
}
