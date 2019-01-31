//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCClient.h
/// All Rights Reserved
//******************************************************************************
/// The RPCClient implements our protobuf-based RPC protocol using the provided
/// Websocket client.
///
/// Attention: single-threaded!
///
/// It assumes total ownership of the websocket connection. All messages must
/// be valid envelopes. It must not be shared with other clients.
///
/// The only thing that is done besides envelope de-/serialization is keeping
/// track of all pending requests. Right now, a per-request timeout is not
/// supported yet, but it should be implemented in this class.
///
/// You have to implement your own messages and wrap them into an Envelop. This
/// class then takes care of sending the request to the server and forwarding
/// all responses (either success or failure) to the provided handler/observer.
///
/// All Envelope fields are automatically set by the RPCClient (existing fields
/// will be overwritten). Just add your data as one or more extensions.
///
/// See envelope.proto for details.
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <atomic>
//Moved to precompiled header: #include <condition_variable>
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/asio/io_service.hpp>
//Moved to precompiled header: #include <boost/chrono/duration.hpp>

#include "SvHttpLibrary/WebsocketClientFactory.h"
#include "ClientStreamContext.h"
#include "Observer.h"
#include "Task.h"
#include "SVProtoBuf/envelope.h"

namespace SvRpc
{
enum class ClientStatus
{
	Disconnected,
	Connected
};

class RPCClient : public SvHttp::WebsocketClient::EventHandler
{
public:
	RPCClient() = delete;
	RPCClient(SvHttp::WebsocketClientSettings&, std::function<void(ClientStatus)> = nullptr);
	virtual ~RPCClient();

	void stop();
	bool isConnected();
	bool waitForConnect(boost::posix_time::time_duration timeout);

	void request(SvPenv::Envelope&& Request, Task<SvPenv::Envelope>);
	void request(SvPenv::Envelope&& Request, Task<SvPenv::Envelope>, boost::posix_time::time_duration);
	ClientStreamContext stream(SvPenv::Envelope&& Request, Observer<SvPenv::Envelope>);

protected:
	void onConnect() override;
	void onDisconnect() override;
	void onTextMessage(std::vector<char>&&) override;
	void onBinaryMessage(std::vector<char>&&) override;

private:
	void on_connect();
	void on_disconnect();
	void on_binary_message(std::shared_ptr<std::vector<char>>);

	void request_impl(SvPenv::Envelope&& request, Task<SvPenv::Envelope>, uint64_t);

	void schedule_reconnect(boost::posix_time::time_duration);
	void on_reconnect(const boost::system::error_code&);

	void schedule_timeout(uint64_t, boost::posix_time::time_duration);
	void on_request_timeout(const boost::system::error_code&, uint64_t);
	void cancel_request_timeout(uint64_t);
	void cancel_all_pending();
	void cancel_all_pending_requests();
	void cancel_all_pending_streams();

	void on_response(SvPenv::Envelope&&);
	void on_error_response(SvPenv::Envelope&&);
	void on_stream_response(SvPenv::Envelope&&);
	void on_stream_error_response(SvPenv::Envelope&&);
	void on_stream_finish(SvPenv::Envelope&&);

	void cancel_stream(uint64_t txId);
	void send_envelope(SvPenv::Envelope&&);

private:
	// TODO: switch to boost::asio::thread_pool when upgrading to boost 1.66.0
	boost::asio::io_context m_IoContex;
	std::unique_ptr<boost::asio::io_context::work> m_IoWork;
	std::thread m_IoThread;
	std::atomic_bool m_IsStopping {false};
	SvHttp::WebsocketClientFactory m_WebsocketClientFactory;
	std::shared_ptr<SvHttp::WebsocketClient> m_WebsocketClient;
	std::mutex m_ConnectMutex;
	std::condition_variable m_ConnectCV;
	std::atomic_bool m_IsConnected {false};
	boost::asio::deadline_timer m_ReconnectTimer;
	uint64_t m_NextTransactionId = 0;
	std::map<uint64_t, Task<SvPenv::Envelope>> m_PendingRequests;
	// deadline_timer is neither movable nor copyable :/ therefore the shared_ptr workaround
	using DeadlineTimerPtr = std::shared_ptr<boost::asio::deadline_timer>;
	std::map<uint64_t, DeadlineTimerPtr> m_PendingRequestsTimer;
	std::map<uint64_t, Observer<SvPenv::Envelope>> m_PendingStreams;
	std::function<void(ClientStatus)> m_pStatusCallback;
};

} // namespace SvRpc
