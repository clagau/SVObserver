//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClient.h
/// All Rights Reserved
//******************************************************************************
/// The WebsocketClient class can be used to connect to any Websocket server.
///
/// Use the connect() function to connecto the server (constructor is not
/// connecting anywhere). The future resolves when handshake was successful.
///
/// Events (like incoming text or binary messages) are reported via the
/// MessageHandler interface. Outgoing messages can be sent using the
/// sendTextMessage and sendBinaryMessage methods.
///
/// The provided boost::asio::io_service must be single-threaded.
//******************************************************************************

#pragma once

#include <memory>
#include <thread>
#include <vector>

#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/future.hpp>

#include "SvHttpLibrary/Handshake.h"
#include "SvHttpLibrary/WebSocketParser.h"

namespace SvHttp
{
class WebsocketClient
{
public:
	class EventHandler
	{
	public:
		virtual ~EventHandler() {}
	protected:
		friend class WebsocketClient;
		virtual void onConnect() = 0;
		virtual void onDisconnect() = 0;
		virtual void onTextMessage(const std::vector<char>&) = 0;
		virtual void onBinaryMessage(const std::vector<char>&) = 0;
	};

private:
	WebsocketClient(EventHandler*);
	WebsocketClient() = delete;

public:
	virtual ~WebsocketClient();
	static std::shared_ptr<WebsocketClient> create(EventHandler*);

	void connect(std::string host, uint16_t port);
	void disconnect();

	void sendTextMessage(const std::vector<char>&);
	void sendBinaryMessage(const std::vector<char>&);

private:
	void handle_connect(const boost::system::error_code& ec);
	void handle_connection_error(const boost::system::error_code& ec);
	void handle_connection_success();
	void close_connection();

	void send_handshake();
	void handle_handshake_sent(const boost::system::error_code& error, size_t bytes_sent);
	void read_handshake_response();
	void handle_handshake_response(const boost::system::error_code& error, size_t bytes_read);

	void handle_request_sent(const boost::system::error_code& error,
		size_t bytes_sent,
		std::shared_ptr<std::vector<char>> buf);

	void read_buffer();
	void handle_read_buffer(const boost::system::error_code& error, size_t bytes_read);
	bool process_frame(std::vector<char>& frames, std::vector<char>& payload);

	void send_pong();

private:
	boost::asio::io_service m_IoService;
	std::unique_ptr<boost::asio::io_service::work> m_IoWork;
	std::thread m_IoThread;
	boost::asio::ip::tcp::socket m_Socket;
	Handshake m_Handshake;
	EventHandler* m_pEventHandler;
	bool m_IsDisconnectEventSent {false};
	bool m_IsShuttingDown {false};
	WebSocketParser m_WebsocketParser;
	std::vector<char> m_Buf;
	std::vector<char> m_Frames;
	std::vector<char> m_Payload;
};

} // namespace SvHttp
