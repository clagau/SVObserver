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
class RPCClient : public SvHttp::WebsocketClient::EventHandler
{
public:
	RPCClient(std::string host, uint16_t port);
	~RPCClient();

	void stop();
	bool isConnected();
	bool waitForConnect(int time_in_ms);

	void request(SvPenv::Envelope&& Request, Task<SvPenv::Envelope>);
	void request(SvPenv::Envelope&& Request, Task<SvPenv::Envelope>, boost::posix_time::time_duration);
	ClientStreamContext stream(SvPenv::Envelope&& Request, Observer<SvPenv::Envelope>);

protected:
	void onConnect() override;
	void onDisconnect() override;
	void onTextMessage(const std::vector<char>&) override;
	void onBinaryMessage(const std::vector<char>&) override;

private:
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
	boost::asio::io_service m_IoService;
	std::unique_ptr<boost::asio::io_service::work> m_IoWork;
	std::thread m_IoThread;
	std::atomic_bool m_IsStopped {false};
	SvHttp::WebsocketClientFactory m_WebsocketClientFactory;
	std::unique_ptr<SvHttp::WebsocketClient> m_WebsocketClient;
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
};

} // namespace SvRpc
