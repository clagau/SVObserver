//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketServerConnection.h
/// All Rights Reserved
//******************************************************************************
/// The WebsocketServerConnection is created by the WebSocketParser per
/// incomsing client connection.
///
/// Implement the MessageHandler interface to receive events. The connections
/// are identified by an id (because we use one MessageHandler for all
/// connections).
///
/// Use sendTextMessage and sendBinaryMessage to send messages to the client.
/// You will get a reference to the connection in the onConnect event. Store
/// this together with the provided connection id. The connection reference will
/// become invalid after you received the onDisconnect event.
//******************************************************************************

#pragma once

#include <future>
#include <memory>
#include <queue>

#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "SVHttpLibrary/Handshake.h"
#include "SVHttpLibrary/WebSocketParser.h"
#include "SVHttpLibrary/WebSocketServerSettings.h"

namespace SVHTTP
{
class WebsocketServerConnection : std::enable_shared_from_this<WebsocketServerConnection>
{
public:
	class EventHandler
	{
	protected:
		friend class WebsocketServerConnection;
		virtual void onConnect(int id, WebsocketServerConnection&) = 0;
		virtual void onTextMessage(int id, const std::vector<char>&) = 0;
		virtual void onBinaryMessage(int id, const std::vector<char>&) = 0;
		virtual void onDisconnect(int id) = 0;
	};

	WebsocketServerConnection(const WebsocketServerSettings& settings,
		boost::asio::io_service& io_service,
		int connection_id,
		EventHandler* event_handler);

	bool isOpen() const;
	boost::asio::ip::tcp::socket& socket();

	void start();

	std::future<void> sendTextMessage(const std::vector<char>&);
	std::future<void> sendBinaryMessage(const std::vector<char>&);

private:
	void read_handshake();
	void handle_handshake_read(const boost::system::error_code& error, size_t bytes_sent);
	void send_handshake_response();
	void handle_handshake_sent(const boost::system::error_code& error, size_t bytes_read);

	void read_buffer();
	void handle_read_buffer(const boost::system::error_code& error, size_t bytes_read, std::vector<char>& buf);
	bool process_frame(std::vector<char>& frames, std::vector<char>& payload);

	std::future<void> send_message_impl(const std::vector<char>&, WebSocketParser::FrameType);
	void send_next_frame();
	void handle_frame_sent(const boost::system::error_code& error, size_t bytes_sent);

	void schedule_ping();
	void on_ping_interval(const boost::system::error_code& error);

	void handle_error(const boost::system::error_code& error);

private:
	int m_connection_id;
	const WebsocketServerSettings& m_settings;
	boost::asio::io_service& m_io_service;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::deadline_timer m_ping_timer;
	uint32_t m_ping_timeout_count;
	Handshake m_handshake;
	EventHandler* m_event_handler;
	WebSocketParser m_websocket_parser;
	std::vector<char> m_buf;
	std::vector<char> m_frames;
	std::vector<char> m_payload;
	bool m_sending_frame {false};
	struct PendingFrame
	{
		std::vector<char> frame;
		std::shared_ptr<std::promise<void>> promise;
	};
	std::queue<PendingFrame> m_frame_queue;
	bool m_disconnect_error_handled {false};
};
}
