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

#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/chrono/duration.hpp>

#include "SVHttpLibrary/WebsocketClientFactory.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/Task.h"
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{
class RPCClient : public SVHTTP::WebsocketClient::EventHandler
{
public:
	RPCClient(std::string host, uint16_t port);
	~RPCClient();

	void Connect(std::string host, uint16_t port);
	void Disconnect();
	void stop();
	bool isConnected();
	bool  waitForConnect(int time_in_ms );

	void request(Envelope&& request, Task<Envelope>);
	void request(Envelope&& request, Task<Envelope>, boost::posix_time::time_duration);
	void stream(Envelope&& request, Observer<Envelope>);

protected:
	void onConnect() override;
	void onDisconnect() override;
	void onTextMessage(const std::vector<char>&) override;
	void onBinaryMessage(const std::vector<char>&) override;

private:
	void request_impl(Envelope&& request, Task<Envelope>, uint64_t);

	void schedule_reconnect(boost::posix_time::time_duration);
	void on_reconnect(const boost::system::error_code&);

	void schedule_timeout(uint64_t, boost::posix_time::time_duration);
	void on_request_timeout(const boost::system::error_code&, uint64_t);
	void cancel_request_timeout(uint64_t);
	void cancel_all_pending();
	void cancel_all_pending_requests();
	void cancel_all_pending_streams();

	void on_response(Envelope&&);
	void on_error_response(Envelope&&);
	void on_stream_response(Envelope&&);
	void on_stream_error_response(Envelope&&);
	void on_stream_finish(Envelope&&);

private:
	boost::asio::io_service m_io_service;
	std::unique_ptr<boost::asio::io_service::work> m_io_work;
	std::thread m_io_thread;
	std::atomic_bool m_stopped {false};
	SVHTTP::WebsocketClientFactory  m_websocket_client_factory;
	std::unique_ptr<SVHTTP::WebsocketClient> m_websocket_client;
	std::mutex m_connect_mutex;
	std::condition_variable m_connect_cv;
	std::atomic_bool m_connected {false};
	boost::asio::deadline_timer m_reconnect_timer;
	uint64_t m_next_transaction_id = 0;
	std::map<uint64_t, Task<Envelope>> m_pending_requests;
	// deadline_timer is neither movable nor copyable :/ therefore the shared_ptr workaround
	std::map<uint64_t, std::shared_ptr<boost::asio::deadline_timer>> m_pending_requests_timer;
	std::map<uint64_t, Observer<Envelope>> m_pending_streams;
	std::atomic_bool m_TryToReconnect {true};
};
}
