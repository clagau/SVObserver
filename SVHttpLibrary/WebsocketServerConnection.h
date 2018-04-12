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

#include "SvHttpLibrary/Handshake.h"
#include "SvHttpLibrary/WebSocketParser.h"
#include "SvHttpLibrary/WebSocketServerSettings.h"

namespace SvHttp
{
class WebsocketServerConnection : public std::enable_shared_from_this<WebsocketServerConnection>
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

private:
	WebsocketServerConnection(const WebsocketServerSettings& rSettings,
		boost::asio::io_service& rIoService,
		int ConnectionId,
		EventHandler* pEventHandler);

public:
	static std::shared_ptr<WebsocketServerConnection> create(
		const WebsocketServerSettings& rSettings,
		boost::asio::io_service& rIoService,
		int ConnectionId,
		EventHandler* pEventHandler);

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
	int m_ConnectionId;
	const WebsocketServerSettings& m_rSettings;
	boost::asio::io_service& m_rIoService;
	boost::asio::ip::tcp::socket m_Socket;
	boost::asio::deadline_timer m_PingTimer;
	uint32_t m_PingTimeoutCount;
	Handshake m_Handshake;
	EventHandler* m_pEventHandler;
	WebSocketParser m_WebsocketParser;
	std::vector<char> m_Buf;
	std::vector<char> m_Frames;
	std::vector<char> m_Payload;
	bool m_IsSendingFrame {false};
	struct PendingFrame
	{
		std::vector<char> Frame;
		std::shared_ptr<std::promise<void>> Promise;
	};
	std::queue<PendingFrame> m_FrameQueue;
	bool m_IsDisconnectErrorHandled {false};
};

} // namespace SvHttp
