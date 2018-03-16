//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCClient.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCClient is a thin wrapper around the WebsocketClient. It assumes all
/// incoming messages are binary and are serialized protobuf envelopes.
//******************************************************************************

#include "stdafx.h"

#include "SVRPCLibrary/ClientStreamContext.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/RPCClient.h"

#include <boost/log/trivial.hpp>

namespace SVRPC
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
	if (m_IsStopped.load())
	{
		return;
	}

	BOOST_LOG_TRIVIAL(debug) << "Shut down of rpc client started";
	if (m_IsConnected && m_WebsocketClient)
	{
		m_WebsocketClient->disconnect();
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
	m_IsStopped.store(true);
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

void RPCClient::request(Envelope&& request, Task<Envelope> task)
{
	if (!isConnected())
	{
		task.error(build_error(ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_NextTransactionId;
	request_impl(std::move(request), task, tx_id);
}

void RPCClient::request(Envelope&& request, Task<Envelope> task, boost::posix_time::time_duration timeout)
{
	if (!isConnected())
	{
		task.error(build_error(ErrorCode::ServiceUnavailable));
		return;
	}
	auto tx_id = ++m_NextTransactionId;
	schedule_timeout(tx_id, timeout);
	request_impl(std::move(request), task, tx_id);
}

void RPCClient::request_impl(Envelope&& request, Task<Envelope> task, uint64_t tx_id)
{
	request.set_transaction_id(tx_id);
	request.set_type(SVRPC::MessageType::Request);

	m_PendingRequests.insert({tx_id, task});

	send_envelope(std::move(request));
}

void RPCClient::schedule_timeout(uint64_t tx_id, boost::posix_time::time_duration timeout)
{
	// use shared_ptr because boost::asio::deadline_timer is neither movable nor copyable
	auto timer = std::make_shared<boost::asio::deadline_timer>(m_IoService);
	timer->expires_from_now(timeout);
	timer->async_wait(std::bind(&RPCClient::on_request_timeout, this, std::placeholders::_1, tx_id));
	m_PendingRequestsTimer.emplace(tx_id, timer);
}

ClientStreamContext RPCClient::stream(Envelope&& request, Observer<Envelope> observer)
{
	if (!isConnected())
	{
		observer.error(build_error(ErrorCode::ServiceUnavailable));
		return ClientStreamContext(nullptr);
	}
	auto tx_id = ++m_NextTransactionId;
	request.set_transaction_id(tx_id);
	request.set_type(SVRPC::MessageType::StreamRequest);

	m_PendingStreams.insert({tx_id, observer});

	send_envelope(std::move(request));

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

		cb.error(build_error(ErrorCode::Timeout));
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
		task.error(build_error(ErrorCode::ServiceUnavailable, "Connection lost. Please retry."));
		m_PendingRequests.erase(it);
	}
}

void RPCClient::cancel_all_pending_streams()
{
	while (!m_PendingStreams.empty())
	{
		auto it = m_PendingStreams.begin();
		auto& observer = it->second;
		observer.error(build_error(ErrorCode::ServiceUnavailable, "Connection lost. Please retry."));
		m_PendingStreams.erase(it);
	}
}

void RPCClient::on_response(Envelope&& response)
{
	auto tx_id = response.transaction_id();
	cancel_request_timeout(tx_id);
	auto it = m_PendingRequests.find(tx_id);
	if (it != m_PendingRequests.end())
	{
		auto cb = it->second;
		m_PendingRequests.erase(it);
		cb.finish(std::move(response));
	}
}

void RPCClient::on_error_response(Envelope&& response)
{
	auto tx_id = response.transaction_id();
	cancel_request_timeout(tx_id);
	auto it = m_PendingRequests.find(tx_id);
	if (it != m_PendingRequests.end())
	{
		auto cb = it->second;
		m_PendingRequests.erase(it);
		cb.error(response.error());
	}
}

void RPCClient::on_stream_response(Envelope&& response)
{
	auto it = m_PendingStreams.find(response.transaction_id());
	if (it != m_PendingStreams.end())
	{
		it->second.onNext(std::move(response));
	}
}

void RPCClient::on_stream_error_response(Envelope&& response)
{
	auto it = m_PendingStreams.find(response.transaction_id());
	if (it != m_PendingStreams.end())
	{
		auto cb = it->second;
		m_PendingStreams.erase(it);
		cb.error(response.error());
	}
}

void RPCClient::on_stream_finish(Envelope&& response)
{
	auto it = m_PendingStreams.find(response.transaction_id());
	if (it != m_PendingStreams.end())
	{
		auto cb = it->second;
		m_PendingStreams.erase(it);
		cb.finish();
	}
}

void RPCClient::cancel_stream(uint64_t txId)
{
	Envelope request;
	request.set_transaction_id(txId);
	request.set_type(SVRPC::MessageType::StreamCancel);
	send_envelope(std::move(request));
}

void RPCClient::send_envelope(Envelope&& envelope)
{
	auto reqSize = envelope.ByteSize();
	std::vector<char> buf;
	buf.resize(reqSize);
	envelope.SerializeToArray(buf.data(), reqSize);

	m_WebsocketClient->sendBinaryMessage(buf);
}

} // namespace SVRPC
