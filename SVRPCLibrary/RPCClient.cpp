//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCClient.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCClient is a thin wrapper around the WebsocketClient. It assumes all
/// incoming messages are binary and are serialized protobuf envelopes.
//******************************************************************************

#include "stdafx.h"

#include "ClientStreamContext.h"
#include "ErrorUtil.h"
#include "RPCClient.h"

#include <boost/log/trivial.hpp>

namespace SvRpc
{
RPCClient::RPCClient(std::string host, uint16_t port)
	: m_IoService(1)
	, m_IoWork(std::make_unique<boost::asio::io_service::work>(m_IoService))
	, m_WebsocketClientFactory(host, port)
	, m_ReconnectTimer(m_IoService)
{
	m_IoThread = std::thread([this]() { m_IoService.run(); });
	m_WebsocketClient = m_WebsocketClientFactory.create(m_IoService, this);
}

RPCClient::~RPCClient()
{
	stop();
}

void RPCClient::stop()
{
	auto IsAlreadyStopping = m_IsStopping.exchange(true);
	if (IsAlreadyStopping)
	{
		return;
	}

	BOOST_LOG_TRIVIAL(debug) << "Shut down of rpc client started";
	// capture shared_ptr in local copy to avoid races
	auto client = m_WebsocketClient;
	if (m_IsConnected && client)
	{
		client->disconnect();
	}
	m_IoWork.reset();
	if (!m_IoService.stopped())
	{
		m_IoService.stop();
	}
	if (m_IoThread.joinable())
	{
		m_IoThread.join();
	}
	BOOST_LOG_TRIVIAL(debug) << "Shut down of rpc client completed";
}

bool RPCClient::isConnected()
{
	return m_IsConnected.load();
}

bool RPCClient::waitForConnect(int time_in_ms)
{
	if (isConnected())
	{
		return true;
	}
	std::chrono::milliseconds time(time_in_ms);
	std::unique_lock<std::mutex> lk(m_ConnectMutex);
	m_ConnectCV.wait_for(lk, time, [this] { return m_IsConnected.load(); });
	return isConnected();
}

void RPCClient::request(SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task)
{
	if (!isConnected())
	{
		Task.error(build_error(SvPenv::ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_NextTransactionId;
	request_impl(std::move(Request), Task, tx_id);
}

void RPCClient::request(SvPenv::Envelope&& request, Task<SvPenv::Envelope> task, boost::posix_time::time_duration timeout)
{
	if (!isConnected())
	{
		task.error(build_error(SvPenv::ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_NextTransactionId;
	schedule_timeout(tx_id, timeout);
	request_impl(std::move(request), task, tx_id);
}

void RPCClient::request_impl(SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task, uint64_t tx_id)
{
	Request.set_transaction_id(tx_id);
	Request.set_type(SvPenv::MessageType::Request);

	m_PendingRequests.insert({tx_id, Task});

	send_envelope(std::move(Request));
}

void RPCClient::schedule_timeout(uint64_t tx_id, boost::posix_time::time_duration timeout)
{
	// use shared_ptr because boost::asio::deadline_timer is neither movable nor copyable
	auto timer = std::make_shared<boost::asio::deadline_timer>(m_IoService);
	timer->expires_from_now(timeout);
	timer->async_wait(std::bind(&RPCClient::on_request_timeout, this, std::placeholders::_1, tx_id));
	m_PendingRequestsTimer.emplace(tx_id, timer);
}

ClientStreamContext RPCClient::stream(SvPenv::Envelope&& Request, Observer<SvPenv::Envelope> Observer)
{
	if (!isConnected())
	{
		Observer.error(build_error(SvPenv::ErrorCode::ServiceUnavailable));
		return ClientStreamContext(nullptr);
	}
	auto tx_id = ++m_NextTransactionId;
	Request.set_transaction_id(tx_id);
	Request.set_type(SvPenv::MessageType::StreamRequest);

	m_PendingStreams.insert({tx_id, Observer});

	send_envelope(std::move(Request));

	return ClientStreamContext(std::bind(&RPCClient::cancel_stream, this, tx_id));
}

void RPCClient::onConnect()
{
	{
		std::lock_guard<std::mutex> lk(m_ConnectMutex);
		m_IsConnected.store(true);
	}
	m_ConnectCV.notify_all();
}

void RPCClient::onDisconnect()
{
	m_IsConnected.store(false);
	BOOST_LOG_TRIVIAL(info) << "RPCClient received disconnect event. Trying to reconnect.";
	cancel_all_pending();
	m_WebsocketClient.reset();
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

	SvPenv::Envelope Response;
	Response.ParseFromArray(buf.data(), static_cast<int>(buf.size()));

	auto type = Response.type();
	switch (type)
	{
		case SvPenv::MessageType::Response:
			on_response(std::move(Response));
			break;

		case SvPenv::MessageType::ErrorResponse:
			on_error_response(std::move(Response));
			break;

		case SvPenv::MessageType::StreamResponse:
			on_stream_response(std::move(Response));
			break;

		case SvPenv::MessageType::StreamErrorResponse:
			on_stream_error_response(std::move(Response));
			break;

		case SvPenv::MessageType::StreamFinish:
			on_stream_finish(std::move(Response));
			break;

		default:
			BOOST_LOG_TRIVIAL(error) << "Invalid message type " << type;
			break;
	}
}

void RPCClient::schedule_reconnect(boost::posix_time::time_duration timeout)
{
	if (m_IsStopping.load())
	{
		return;
	}
	BOOST_LOG_TRIVIAL(debug) << "Scheduling reconnect in " << timeout.seconds() << " seconds.";
	m_ReconnectTimer.expires_from_now(timeout);
	m_ReconnectTimer.async_wait(std::bind(&RPCClient::on_reconnect, this, std::placeholders::_1));
}

void RPCClient::on_reconnect(const boost::system::error_code& error)
{
	if (boost::asio::error::operation_aborted == error)
	{
		return;
	}

	if (error)
	{
		BOOST_LOG_TRIVIAL(warning) << error;
		return;
	}

	BOOST_LOG_TRIVIAL(info) << "Trying to reconnect.";
	m_WebsocketClient = m_WebsocketClientFactory.create(m_IoService, this);
}

void RPCClient::on_request_timeout(const boost::system::error_code& error, uint64_t tx_id)
{
	if (boost::asio::error::operation_aborted == error)
	{
		return;
	}

	if (error)
	{
		BOOST_LOG_TRIVIAL(warning) << error;
		return;
	}

	m_PendingRequestsTimer.erase(tx_id);

	auto it = m_PendingRequests.find(tx_id);
	if (it != m_PendingRequests.end())
	{
		auto cb = it->second;
		m_PendingRequests.erase(it);

		cb.error(build_error(SvPenv::ErrorCode::Timeout));
	}
}

void RPCClient::cancel_request_timeout(uint64_t tx_id)
{
	auto it = m_PendingRequestsTimer.find(tx_id);
	if (it != m_PendingRequestsTimer.end())
	{
		auto timer_ptr = it->second;
		m_PendingRequestsTimer.erase(it);
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
	while (!m_PendingRequests.empty())
	{
		auto it = m_PendingRequests.begin();
		auto tx_id = it->first;
		cancel_request_timeout(tx_id);
		auto& task = it->second;
		task.error(build_error(SvPenv::ErrorCode::ServiceUnavailable, "Connection lost. Please retry."));
		m_PendingRequests.erase(it);
	}
}

void RPCClient::cancel_all_pending_streams()
{
	while (!m_PendingStreams.empty())
	{
		auto it = m_PendingStreams.begin();
		auto& observer = it->second;
		observer.error(build_error(SvPenv::ErrorCode::ServiceUnavailable, "Connection lost. Please retry."));
		m_PendingStreams.erase(it);
	}
}

void RPCClient::on_response(SvPenv::Envelope&& Response)
{
	auto tx_id = Response.transaction_id();
	cancel_request_timeout(tx_id);
	auto it = m_PendingRequests.find(tx_id);
	if (it != m_PendingRequests.end())
	{
		auto cb = it->second;
		m_PendingRequests.erase(it);
		cb.finish(std::move(Response));
	}
}

void RPCClient::on_error_response(SvPenv::Envelope&& Response)
{
	auto tx_id = Response.transaction_id();
	cancel_request_timeout(tx_id);
	auto it = m_PendingRequests.find(tx_id);
	if (it != m_PendingRequests.end())
	{
		auto cb = it->second;
		m_PendingRequests.erase(it);
		cb.error(Response.error());
	}
}

void RPCClient::on_stream_response(SvPenv::Envelope&& Response)
{
	auto it = m_PendingStreams.find(Response.transaction_id());
	if (it != m_PendingStreams.end())
	{
		it->second.onNext(std::move(Response));
	}
}

void RPCClient::on_stream_error_response(SvPenv::Envelope&& Response)
{
	auto it = m_PendingStreams.find(Response.transaction_id());
	if (it != m_PendingStreams.end())
	{
		auto cb = it->second;
		m_PendingStreams.erase(it);
		cb.error(Response.error());
	}
}

void RPCClient::on_stream_finish(SvPenv::Envelope&& Response)
{
	auto it = m_PendingStreams.find(Response.transaction_id());
	if (it != m_PendingStreams.end())
	{
		auto cb = it->second;
		m_PendingStreams.erase(it);
		cb.finish();
	}
}

void RPCClient::cancel_stream(uint64_t txId)
{
	SvPenv::Envelope Request;
	Request.set_transaction_id(txId);
	Request.set_type(SvPenv::MessageType::StreamCancel);
	send_envelope(std::move(Request));
}

void RPCClient::send_envelope(SvPenv::Envelope&& Envelope)
{
	auto reqSize = Envelope.ByteSize();
	std::vector<char> buf;
	buf.resize(reqSize);
	Envelope.SerializeToArray(buf.data(), reqSize);

	// capture shared_ptr in local copy to avoid races
	auto client = m_WebsocketClient;
	if (client)
	{
		client->sendBinaryMessage(buf);
	}
	else
	{
		// TODO: check whether reconnecting right now and enqueue current envelope
	}
}

} // namespace SvRpc
