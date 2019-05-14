//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCClient.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCClient is a thin wrapper around the WebsocketClient. It assumes all
/// incoming messages are binary and are serialized protobuf envelopes.
//******************************************************************************

#include "stdafx.h"
#include "SVLogLibrary/Logging.h"
#include "ClientStreamContext.h"
#include "ErrorUtil.h"
#include "RPCClient.h"

namespace SvRpc
{
RPCClient::RPCClient(SvHttp::WebsocketClientSettings& rSettings, std::function<void(ClientStatus)> StatusCallback)
	: m_IoContex(1)
	, m_IoWork(std::make_unique<boost::asio::io_context::work>(m_IoContex))
	, m_WebsocketClientFactory(rSettings)
	, m_ReconnectTimer(m_IoContex)
	, m_pStatusCallback(StatusCallback)
{
	m_IoThread = std::thread([this]() { m_IoContex.run(); });
	m_WebsocketClient = m_WebsocketClientFactory.create(this);
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

	SV_LOG_GLOBAL(debug) << "Shut down of rpc client started";

	{ // capture shared_ptr in local copy to avoid races
		auto client = m_WebsocketClient;
		if (m_IsConnected && client)
		{
			client->disconnect();
		}
	}
	m_WebsocketClient.reset();
	m_IoWork.reset();
	if (!m_IoContex.stopped())
	{
		m_IoContex.stop();
	}
	if (m_IoThread.joinable())
	{
		m_IoThread.join();
	}
	SV_LOG_GLOBAL(debug) << "Shut down of rpc client completed";
}

bool RPCClient::isConnected()
{
	return m_IsConnected.load();
}

bool RPCClient::waitForConnect(boost::posix_time::time_duration posix_timeout)
{
	if (isConnected())
	{
		return true;
	}
	std::unique_lock<std::mutex> lk(m_ConnectMutex);
	std::chrono::milliseconds timeout(posix_timeout.total_milliseconds());
	m_ConnectCV.wait_for(lk, timeout, [this] { return m_IsConnected.load(); });
	return isConnected();
}

void RPCClient::request(SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task)
{
	if (!isConnected())
	{
		Task.error(build_error(SvPenv::ErrorCode::serviceUnavailable));
		return;
	}
	auto tx_id = ++m_NextTransactionId;
	request_impl(std::move(Request), Task, tx_id);
}

void RPCClient::request(SvPenv::Envelope&& request, Task<SvPenv::Envelope> task, boost::posix_time::time_duration timeout)
{
	if (!isConnected())
	{
		task.error(build_error(SvPenv::ErrorCode::serviceUnavailable));
		return;
	}
	auto tx_id = ++m_NextTransactionId;
	schedule_timeout(tx_id, timeout);
	request_impl(std::move(request), task, tx_id);
}

void RPCClient::request_impl(SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task, uint64_t tx_id)
{
	Request.set_transactionid(tx_id);
	Request.set_type(SvPenv::MessageType::request);

	m_PendingRequests.insert({tx_id, Task});

	send_envelope(std::move(Request));
}

void RPCClient::schedule_timeout(uint64_t tx_id, boost::posix_time::time_duration timeout)
{
	// use shared_ptr because boost::asio::deadline_timer is neither movable nor copyable
	auto timer = std::make_shared<boost::asio::deadline_timer>(m_IoContex);
	timer->expires_from_now(timeout);
	timer->async_wait(std::bind(&RPCClient::on_request_timeout, this, std::placeholders::_1, tx_id));
	m_PendingRequestsTimer.emplace(tx_id, timer);
}

ClientStreamContext RPCClient::stream(SvPenv::Envelope&& Request, Observer<SvPenv::Envelope> Observer)
{
	if (!isConnected())
	{
		Observer.error(build_error(SvPenv::ErrorCode::serviceUnavailable));
		return ClientStreamContext(nullptr);
	}
	auto txId = ++m_NextTransactionId;
	Request.set_transactionid(txId);
	Request.set_type(SvPenv::MessageType::streamRequest);

	m_PendingStreams.insert({txId, Observer});
	SV_LOG_GLOBAL(debug) << "RPCClient::stream TransactionId: " << txId << " Pending Nr " << m_PendingStreams.size() << std::endl;

	send_envelope(std::move(Request));

	return ClientStreamContext(std::bind(&RPCClient::cancel_stream, this, txId));
}

void RPCClient::onConnect()
{
	m_IoContex.dispatch(std::bind(&RPCClient::on_connect, this));
}

void RPCClient::on_connect()
{
	{
		std::lock_guard<std::mutex> lk(m_ConnectMutex);
		m_IsConnected.store(true);
	}
	m_ConnectCV.notify_all();

	if (nullptr != m_pStatusCallback)
	{
		m_pStatusCallback(ClientStatus::Connected);
	}
}

void RPCClient::onDisconnect()
{
	m_IoContex.dispatch(std::bind(&RPCClient::on_disconnect, this));
}

void RPCClient::on_disconnect()
{
	m_IsConnected.store(false);
	SV_LOG_GLOBAL(info) << "RPCClient received disconnect event. Trying to reconnect.";
	cancel_all_pending();
	m_WebsocketClient.reset();
	schedule_reconnect(boost::posix_time::seconds(1));

	if (nullptr != m_pStatusCallback)
	{
		m_pStatusCallback(ClientStatus::Disconnected);
	}
}

void RPCClient::onTextMessage(std::vector<char>&&)
{
	SV_LOG_GLOBAL(error) << "Received a text message, but only binary messages expected!";
}

void RPCClient::onBinaryMessage(std::vector<char>&& buf)
{
	auto ptr = std::make_shared<std::vector<char>>(std::move(buf));
	m_IoContex.dispatch(std::bind(&RPCClient::on_binary_message, this, ptr));
}

void RPCClient::on_binary_message(std::shared_ptr<std::vector<char>> ptr)
{
	const auto& buf = *ptr;
	if (buf.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
	{
		SV_LOG_GLOBAL(error) << "Message too large " << buf.size() << ". Must not be larger than "
			<< std::numeric_limits<int>::max() << ".";
		return;
	}

	SvPenv::Envelope Response;
	Response.ParseFromArray(buf.data(), static_cast<int>(buf.size()));

	auto type = Response.type();
	switch (type)
	{
		case SvPenv::MessageType::response:
			on_response(std::move(Response));
			break;

		case SvPenv::MessageType::errorResponse:
			on_error_response(std::move(Response));
			break;

		case SvPenv::MessageType::streamResponse:
			on_stream_response(std::move(Response));
			break;

		case SvPenv::MessageType::streamErrorResponse:
			on_stream_error_response(std::move(Response));
			break;

		case SvPenv::MessageType::streamFinish:
			on_stream_finish(std::move(Response));
			break;

		default:
			SV_LOG_GLOBAL(error) << "Invalid message type " << type;
			break;
	}
}

void RPCClient::schedule_reconnect(boost::posix_time::time_duration timeout)
{
	if (m_IsStopping.load())
	{
		return;
	}
	SV_LOG_GLOBAL(debug) << "Scheduling reconnect in " << timeout.seconds() << " seconds.";
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
		SV_LOG_GLOBAL(warning) << error;
		return;
	}

	SV_LOG_GLOBAL(info) << "Trying to reconnect.";
	m_WebsocketClient = m_WebsocketClientFactory.create(this);
}

void RPCClient::on_request_timeout(const boost::system::error_code& error, uint64_t tx_id)
{
	if (boost::asio::error::operation_aborted == error)
	{
		return;
	}

	if (error)
	{
		SV_LOG_GLOBAL(warning) << error;
		return;
	}

	m_PendingRequestsTimer.erase(tx_id);

	auto it = m_PendingRequests.find(tx_id);
	if (it != m_PendingRequests.end())
	{
		auto cb = it->second;
		m_PendingRequests.erase(it);

		cb.error(build_error(SvPenv::ErrorCode::timeout));
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
	//We catch the exception and remove it as clients may have disconnected in the mean time
	try
	{
		while (!m_PendingRequests.empty())
		{
			auto it = m_PendingRequests.begin();
			auto tx_id = it->first;
			cancel_request_timeout(tx_id);
			auto& task = it->second;
			task.error(build_error(SvPenv::ErrorCode::serviceUnavailable, "Connection lost. Please retry."));
			m_PendingRequests.erase(it);
		}
	}
	catch (const std::exception&)
	{
	}
}

void RPCClient::cancel_all_pending_streams()
{
	//We catch the exception and remove it as clients may have disconnected in the mean time
	try
	{
		while (!m_PendingStreams.empty())
		{
			auto it = m_PendingStreams.begin();
			auto& observer = it->second;
			observer.error(build_error(SvPenv::ErrorCode::serviceUnavailable, "Connection lost. Please retry."));
			m_PendingStreams.erase(it);
		}
	}
	catch (const std::exception&)
	{
	}
}

void RPCClient::on_response(SvPenv::Envelope&& Response)
{
	//We catch the exception and remove it as clients may have disconnected in the mean time
	try
	{
		auto txId = Response.transactionid();
		cancel_request_timeout(txId);
		auto it = m_PendingRequests.find(txId);
		if (it != m_PendingRequests.end())
		{
			auto cb = it->second;
			m_PendingRequests.erase(it);
			cb.finish(std::move(Response));
		}
	}
	catch (const std::exception&)
	{
	}
}

void RPCClient::on_error_response(SvPenv::Envelope&& Response)
{
	//We catch the exception and remove it as clients may have disconnected in the mean time
	try
	{
		auto txId = Response.transactionid();
		cancel_request_timeout(txId);
		auto it = m_PendingRequests.find(txId);
		if (it != m_PendingRequests.end())
		{
			auto cb = it->second;
			m_PendingRequests.erase(it);
			cb.error(Response.error());
		}
	}
	catch (const std::exception&)
	{
	}
}

void RPCClient::on_stream_response(SvPenv::Envelope&& Response)
{
	auto txid = Response.transactionid();
	auto seqNr = Response.sequencenumber();
	auto it = m_PendingStreams.find(txid);
	if (it != m_PendingStreams.end())
	{
		//We catch the exception and remove it as clients may have disconnected in the mean time
		try
		{
			it->second.onNext(std::move(Response)).then(m_IoContex, [this, txid, seqNr](auto f)
			{
				ack_stream_response(txid, seqNr);
			});
		}
		catch (const std::exception&)
		{
		}
	}
}

void RPCClient::on_stream_error_response(SvPenv::Envelope&& Response)
{
	auto it = m_PendingStreams.find(Response.transactionid());
	if (it != m_PendingStreams.end())
	{
		//We catch the exception and remove it as clients may have disconnected in the mean time
		try
		{
			auto cb = it->second;
			m_PendingStreams.erase(it);
			cb.error(Response.error());
		}
		catch (const std::exception&)
		{
		}
	}
}

void RPCClient::on_stream_finish(SvPenv::Envelope&& Response)
{
	auto it = m_PendingStreams.find(Response.transactionid());
	if (it != m_PendingStreams.end())
	{
		//We catch the exception and remove it as clients may have disconnected in the mean time
		try
		{
			auto cb = it->second;
			m_PendingStreams.erase(it);
			cb.finish();
		}
		catch (const std::exception&)
		{
		}
	}
}

void RPCClient::cancel_stream(uint64_t txId)
{
	SvPenv::Envelope Request;
	Request.set_transactionid(txId);
	Request.set_type(SvPenv::MessageType::streamCancel);
	send_envelope(std::move(Request));
}

void RPCClient::ack_stream_response(uint64_t txId, uint64_t seqNr)
{
	SvPenv::Envelope Request;
	Request.set_transactionid(txId);
	Request.set_sequencenumber(seqNr);
	Request.set_type(SvPenv::MessageType::streamAck);
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
