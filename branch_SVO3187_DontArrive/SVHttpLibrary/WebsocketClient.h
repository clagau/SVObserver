//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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
/// The provided boost::asioio_context must be single-threaded.
//******************************************************************************

#pragma once

#include <deque>
#include <memory>
#include <thread>
#include <tuple>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

namespace SvHttp
{
struct WebsocketClientSettings;

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
		virtual void onTextMessage(std::vector<char>&&) = 0;
		virtual void onBinaryMessage(std::vector<char>&&) = 0;
	};

private:
	WebsocketClient(WebsocketClientSettings&, EventHandler*);
	WebsocketClient() = delete;

public:
	virtual ~WebsocketClient();
	static std::shared_ptr<WebsocketClient> create(WebsocketClientSettings&, EventHandler*);

	void connect();
	void disconnect();

	void sendTextMessage(const std::vector<char>&);
	void sendBinaryMessage(const std::vector<char>&);

private:
	void handle_resolve(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type);
	void handle_connect(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it);
	void handle_connection_error(const boost::system::error_code& ec);
	void handle_connection_success();
	void close_connection();

	void send_handshake();
	void handle_handshake_response(const boost::system::error_code& error);

	void handle_request_sent(const boost::system::error_code& error,
		size_t bytes_sent,
		std::shared_ptr<std::vector<char>> buf);

	void read_buffer();
	void handle_read_buffer(const boost::system::error_code& error, size_t bytes_read);

	void schedule_ping();
	void on_ping_interval(const boost::system::error_code& error);
	void handle_ping_sent(const boost::system::error_code& error);
	void handle_control_command(boost::beast::websocket::frame_type, boost::beast::string_view);

	void write_message(bool is_binary, std::shared_ptr<std::vector<char>>);
	void write_message_impl(bool is_binary, std::shared_ptr<std::vector<char>>);
	void check_write_queue();

private:
	WebsocketClientSettings& m_rSettings;
	boost::asio::io_context m_IoContext;
	std::unique_ptr<boost::asio::io_context::work> m_IoWork;
	std::thread m_IoThread;
	boost::asio::ip::tcp::resolver m_Resolver;
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_Socket;
	std::function<void(boost::beast::websocket::frame_type, boost::beast::string_view)> m_ControlCommand;
	boost::asio::deadline_timer m_PingTimer;
	uint32_t m_PingTimeoutCount;
	EventHandler* m_pEventHandler;
	bool m_IsDisconnectEventSent {false};
	bool m_IsShuttingDown {false};
	std::vector<char> m_Buf;
	std::vector<char> m_Payload;
	bool m_bIsWriting {false};
	std::deque<std::pair<bool, std::shared_ptr<std::vector<char>>>> m_WriteQueue;
};

} // namespace SvHttp
