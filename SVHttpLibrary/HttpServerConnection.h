//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file HttpServerConnection.h
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#pragma once

#include <chrono>
#include <memory>
#include <queue>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>

#include "MediaType.h"
#include "url.hpp"
#include "SVSystemLibrary/SVFuture.h"

namespace SvHttp
{
struct HttpServerSettings;


class HttpServerConnection : public std::enable_shared_from_this<HttpServerConnection>
{
private:
	HttpServerConnection(const HttpServerSettings& rSettings,
		boost::asio::io_context& rIoContext,
		boost::asio::ip::tcp::socket Socket,
		int ConnectionId);

public:
	static std::shared_ptr<HttpServerConnection> create(
		const HttpServerSettings& rSettings,
		boost::asio::io_context& rIoContext,
		boost::asio::ip::tcp::socket Socket, 
		int ConnectionId);

	bool isOpen() const;

	void start();
	void close();

	SvSyl::SVFuture<void> sendTextMessage(const std::vector<char>&);
	SvSyl::SVFuture<void> sendBinaryMessage(const std::vector<char>&);

	std::string getIpAddress() const;

private:
	void close_impl();

private:
	void http_do_read();
	void http_on_read(const boost::system::error_code& error, size_t bytes_read);
	void http_on_write(const boost::system::error_code& error, size_t bytes_read, bool close);
	void http_on_error(const boost::system::error_code& error);
	void http_do_close();

	void http_do_write(boost::beast::http::response<boost::beast::http::empty_body>&& Response);
	void http_do_write(boost::beast::http::response<boost::beast::http::string_body>&& Response);
	void http_do_write(boost::beast::http::response<boost::beast::http::file_body>&& Response);
	template<class Body> void http_do_write_impl(boost::beast::http::response<Body>& Response);
	template<class Body> void http_access_log(const boost::beast::http::response<Body>& rResponse);

	bool http_custom_request_handler(const Url&);

	boost::beast::http::response<boost::beast::http::string_body> http_build_not_found(boost::beast::string_view target);
	boost::beast::http::response<boost::beast::http::string_body> http_build_not_authorized(boost::beast::string_view reason);
	boost::beast::http::response<boost::beast::http::string_body> http_build_bad_request(boost::beast::string_view why);
	boost::beast::http::response<boost::beast::http::string_body> http_build_server_error(boost::beast::string_view what);
	boost::beast::http::response<boost::beast::http::empty_body> http_build_file_head(const std::filesystem::path&, boost::beast::http::file_body::value_type body);
	boost::beast::http::response<boost::beast::http::file_body> http_build_file_get(const std::filesystem::path&, boost::beast::http::file_body::value_type body);

private:
	std::string ws_get_access_token(std::string& rProtocol);
	void ws_do_upgrade(const std::string& rProtocol);
	void ws_on_decorate(boost::beast::websocket::response_type& m, const std::string& rProtocol);
	void ws_on_handshake(const boost::system::error_code& error);
	void ws_access_log();

	void ws_do_read();
	void ws_on_read(const boost::system::error_code& error, size_t bytes_read);

	SvSyl::SVFuture<void> ws_send_message_impl(const std::vector<char>&, bool is_binary);
	void ws_send_next_frame();
	void ws_on_frame_sent(const boost::system::error_code& error, size_t bytes_sent);

	void ws_on_error(const boost::system::error_code& error, const char* source);

private:
	bool base64decode(std::string&, const boost::beast::string_view&);

private:
	int m_ConnectionId;
	const HttpServerSettings& m_rSettings;
	boost::asio::io_context& m_rIoContext;
	bool m_IsUpgraded {false};
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_WsSocket;
	boost::asio::ip::tcp::socket& m_Socket;

	// http
	MediaType m_MediaType;
	boost::beast::flat_buffer m_Buf;
	std::chrono::system_clock::time_point m_ReceivedAt;
	boost::beast::http::request<boost::beast::http::string_body> m_Request;
	boost::beast::http::response<boost::beast::http::empty_body> m_EmptyResponse;
	boost::beast::http::response<boost::beast::http::file_body> m_FileResponse;
	boost::beast::http::response<boost::beast::http::string_body> m_StringResponse;

	// ws
	std::vector<char> m_WsBuf;
	std::vector<char> m_Frames;
	std::vector<char> m_Payload;
	bool m_IsSendingFrame {false};
	struct PendingFrame
	{
		bool IsBinary;
		std::vector<char> Frame;
		std::shared_ptr<SvSyl::SVPromise<void>> Promise;
	};
	std::queue<PendingFrame> m_FrameQueue;
	bool m_IsWebsocketHandshakeDone {false};
	bool m_IsDisconnectErrorHandled {false};
	bool m_IsHttpErrorOccurred {false};
};

} // namespace SvHttp
