//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpServerConnection.cpp
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#include "stdafx.h"

#include <filesystem>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/log/trivial.hpp>

#include "url.hpp"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServerConnection.h"

namespace SvHttp
{
HttpServerConnection::HttpServerConnection(const HttpServerSettings& rSettings,
	boost::asio::io_context& rIoContext,
	boost::asio::ip::tcp::socket Socket,
	int ConnectionId)
	: m_ConnectionId(ConnectionId)
	, m_rSettings(rSettings)
	, m_rIoContext(rIoContext)
	, m_WsSocket(std::move(Socket))
	, m_Socket(m_WsSocket.next_layer())
{
	m_WsSocket.read_message_max(rSettings.MaxMessageSize);
	m_WsSocket.write_buffer_size(rSettings.WriteBufferSize);
}

std::shared_ptr<HttpServerConnection> HttpServerConnection::create(
	const HttpServerSettings& rSettings,
	boost::asio::io_context& rIoContext,
	boost::asio::ip::tcp::socket Socket,
	int ConnectionId)
{
	return std::shared_ptr<HttpServerConnection>(new HttpServerConnection(rSettings, rIoContext, std::move(Socket), ConnectionId));
}

bool HttpServerConnection::isOpen() const
{
	return m_IsUpgraded ? m_WsSocket.is_open() : m_Socket.is_open();
}

void HttpServerConnection::start()
{
	http_do_read();
}

std::future<void> HttpServerConnection::sendTextMessage(const std::vector<char>& buf)
{
	return ws_send_message_impl(buf, false);
}

std::future<void> HttpServerConnection::sendBinaryMessage(const std::vector<char>& buf)
{
	return ws_send_message_impl(buf, true);
}

void HttpServerConnection::http_do_read()
{
	m_Request.clear();
	boost::beast::http::async_read(
		m_Socket,
		m_Buf,
		m_Request,
		std::bind(&HttpServerConnection::http_on_read,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2));
}

void HttpServerConnection::http_on_read(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		http_on_error(error);
		http_do_close();
		return;
	}

	m_ReceivedAt = std::chrono::system_clock::now();

	const auto url = Url(std::string(m_Request.target()));

	const auto& url_path = url.path();
	if (url_path.empty() || url_path[0] != '/' || url_path.find("..") != std::string::npos)
	{
		return http_do_write(http_build_bad_request("Illegal path"));
	}

	// Check for WebSocket Upgrade
	if (boost::beast::websocket::is_upgrade(m_Request))
	{
		const auto token = ws_get_access_token();
		const auto handler = m_rSettings.pEventHandler;
		if (handler && !handler->onHandshake(m_ConnectionId, token))
		{
			return http_do_write(http_build_not_authorized("Missing or invalid token"));
		}

		// From here on, http_do_write is not called any more.
		// So let's trigger the access log explicitly.
		// TODO: shall we delay it until handshake was done?
		ws_access_log();

		return ws_do_upgrade();
	}

	// call custom http request handler
	if (http_custom_request_handler(url))
	{
		return;
	}

	if (!m_rSettings.bEnableFileServing)
	{
		return http_do_write(http_build_not_found(m_Request.target()));
	}

	// Build the path to the requested file
	auto path = std::experimental::filesystem::path(m_rSettings.DataDir).append(url_path);
	if (m_Request.target().back() == '/')
	{
		path.append(m_rSettings.DefaultIndexHtmlFile);
	}

	// Make sure we can handle the method
	if (m_Request.method() != boost::beast::http::verb::get &&
		m_Request.method() != boost::beast::http::verb::head)
	{
		return http_do_write(http_build_bad_request("Unknown HTTP-method"));
	}

	// Attempt to open the file
	boost::beast::error_code ec;
	boost::beast::http::file_body::value_type body;
	body.open(path.string().c_str(), boost::beast::file_mode::scan, ec);
	if (ec == boost::system::errc::no_such_file_or_directory)
	{
		path = path.parent_path() / m_rSettings.DefaultErrorHtmlFile;
	}

	body.open(path.string().c_str(), boost::beast::file_mode::scan, ec);
	if (ec == boost::system::errc::no_such_file_or_directory)
	{
		return http_do_write(http_build_not_found(m_Request.target()));
	}

	// Handle an unknown error
	if (ec)
	{
		return http_do_write(http_build_server_error(ec.message()));
	}

	// handle head-request
	if (m_Request.method() == boost::beast::http::verb::head)
	{
		return http_do_write(http_build_file_head(path, std::move(body)));
	}

	return http_do_write(http_build_file_get(path, std::move(body)));
}

void HttpServerConnection::http_on_write(const boost::system::error_code& error, size_t bytes_read, bool close)
{
	m_pResponse = {}; // release memory we needed to keep during write

	if (error)
	{
		http_on_error(error);
		http_do_close();
		return;
	}

	if (close)
	{
		BOOST_LOG_TRIVIAL(debug) << "No keep-alive connection. Closing...";
		return http_do_close();
	}

	if (m_Socket.is_open())
	{
		http_do_read();
	}
}

void HttpServerConnection::http_on_error(const boost::system::error_code& error)
{
	if (!error)
	{
		return;
	}

	if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset ||
		error == boost::asio::error::connection_aborted ||
		error.value() == WSAEBADF) // bad file descriptor, when connection already closed
	{
		if (!m_IsDisconnectErrorHandled)
		{
			BOOST_LOG_TRIVIAL(info) << "[http] Client disconnected";
			if (m_rSettings.pEventHandler)
			{
				m_rSettings.pEventHandler->onDisconnect(m_ConnectionId);
			}
		}
		m_IsDisconnectErrorHandled = true;
		return;
	}

	BOOST_LOG_TRIVIAL(warning) << "client connection error: " << error;
}

void HttpServerConnection::http_do_close()
{
	BOOST_LOG_TRIVIAL(debug) << "Closing connection";
	if (!m_Socket.is_open())
	{
		return;
	}

	boost::system::error_code ec;
	m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
	if (ec)
	{
		BOOST_LOG_TRIVIAL(warning) << "Error while closing http connection: " << ec;
	}
}

template<class Body>
void HttpServerConnection::http_do_write(boost::beast::http::response<Body> Response)
{
	http_access_log(Response);

	auto pResponse = std::make_shared<boost::beast::http::response<Body>>(std::move(Response));
	m_pResponse = pResponse;
	
	boost::beast::http::async_write(
		m_Socket,
		*pResponse,
		std::bind(
		&HttpServerConnection::http_on_write,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		pResponse->need_eof()
	)
	);
}

// Formats given time_point to required format, e.g. 27/Jul/2018:16:31:21 +0100
static std::string format_timestamp(const std::chrono::system_clock::time_point& time_point)
{
	const auto timestamp = std::chrono::system_clock::to_time_t(time_point);
	tm tm;
	gmtime_s(&tm, &timestamp);

	char buf[64];
	const auto locale = _create_locale(LC_ALL, "C");
	const auto len = _strftime_l(buf, 64, "%d/%b/%Y:%H:%M:%S %z", &tm, locale);
	_free_locale(locale);

	return len ? std::string(buf, buf + len) : "-";
}

/// Log requests using the common log format for http requests
/// See https://en.wikipedia.org/wiki/Common_Log_Format
template<class Body>
void HttpServerConnection::http_access_log(const boost::beast::http::response<Body>& rResponse)
{
	const auto version = rResponse.version();
	const auto major = version / 10;
	const auto minor = version % 10;

	const auto get_request_header = [this](const boost::beast::http::field& field)
	{
		const auto str = m_Request[field].to_string();
		if (str.empty())
		{
			return std::string("-");
		}
		return std::string("\"") + str + "\"";
	};

	BOOST_LOG_TRIVIAL(debug)
		// remote host, usually the client's ip
		<< m_Socket.remote_endpoint().address().to_string() << " "
		// client identity, ignored
		<< "- "
		// user name, ignored because not supported yet
		<< "- "
		// time when request received in required format incl. brackets
		<< "[" << format_timestamp(m_ReceivedAt) << "] "
		// full client request line, e.g. "GET /index.html HTTP/1.1"
		<< "\"" << m_Request.method() << " " << m_Request.target() << " HTTP/" << major << "." << minor << "\" "
		// response status code
		<< rResponse.result_int() << " "
		// response size in bytes
		<< rResponse.payload_size().value_or(0) << " "
		// client's referer header
		<< get_request_header(boost::beast::http::field::referer) << " "
		// client's user-agent header
		<< get_request_header(boost::beast::http::field::user_agent);
}

bool HttpServerConnection::http_custom_request_handler(const Url& rUrl)
{
	if (!m_rSettings.HttpRequestHandler)
	{
		return false;
	}

	HttpRequest Request;
	Request.Method = m_Request.method();
	Request.Url = rUrl;
	Request.Body = m_Request.body();

	HttpResponse Response;
	if (m_rSettings.HttpRequestHandler(Request, Response))
	{
		boost::beast::http::response<boost::beast::http::string_body> res {
			Response.Status,
			m_Request.version()
		};
		res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
		res.set(boost::beast::http::field::content_type, Response.ContentType);
		res.keep_alive(m_Request.keep_alive());
		res.body() = Response.Body;
		res.prepare_payload();
		http_do_write(std::move(res));
		return true;
	}

	return false;
}

boost::beast::http::response<boost::beast::http::string_body>
HttpServerConnection::http_build_not_found(boost::beast::string_view target)
{
	boost::beast::http::response<boost::beast::http::string_body> res {
		boost::beast::http::status::not_found,
		m_Request.version()
	};
	res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
	res.set(boost::beast::http::field::content_type, "text/html");
	res.keep_alive(m_Request.keep_alive());
	res.body() = "The resource '" + target.to_string() + "' was not found.";
	res.prepare_payload();
	return res;
}

boost::beast::http::response<boost::beast::http::string_body>
HttpServerConnection::http_build_not_authorized(boost::beast::string_view reason)
{
	boost::beast::http::response<boost::beast::http::string_body> res {
		boost::beast::http::status::unauthorized,
		m_Request.version()
	};
	res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
	res.set(boost::beast::http::field::content_type, "text/html");
	res.keep_alive(m_Request.keep_alive());
	res.body() = "Not authorized: " + reason.to_string();
	res.prepare_payload();
	return res;
}

boost::beast::http::response<boost::beast::http::string_body>
HttpServerConnection::http_build_bad_request(boost::beast::string_view why)
{
	boost::beast::http::response<boost::beast::http::string_body> res {
		boost::beast::http::status::bad_request,
		m_Request.version()
	};
	res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
	res.set(boost::beast::http::field::content_type, "text/html");
	res.keep_alive(m_Request.keep_alive());
	res.body() = why.to_string();
	res.prepare_payload();
	return res;
}

boost::beast::http::response<boost::beast::http::string_body>
HttpServerConnection::http_build_server_error(boost::beast::string_view what)
{
	boost::beast::http::response<boost::beast::http::string_body> res {
		boost::beast::http::status::internal_server_error,
		m_Request.version()
	};
	res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
	res.set(boost::beast::http::field::content_type, "text/html");
	res.keep_alive(m_Request.keep_alive());
	res.body() = "An error occurred: '" + what.to_string() + "'";
	res.prepare_payload();
	return res;
}

boost::beast::http::response<boost::beast::http::empty_body>
HttpServerConnection::http_build_file_head(const std::experimental::filesystem::path& path, boost::beast::http::file_body::value_type body)
{
	boost::beast::http::response<boost::beast::http::empty_body> res {
		boost::beast::http::status::ok,
		m_Request.version()
	};
	res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
	res.set(boost::beast::http::field::content_type, m_MediaType.determine(path));
	res.content_length(body.size());
	res.keep_alive(m_Request.keep_alive());
	return res;
}

boost::beast::http::response<boost::beast::http::file_body>
HttpServerConnection::http_build_file_get(const std::experimental::filesystem::path& path, boost::beast::http::file_body::value_type body)
{
	// Cache the size since we need it after the move
	auto const size = body.size();

	boost::beast::http::response<boost::beast::http::file_body> res {
		std::piecewise_construct,
		std::make_tuple(std::move(body)),
		std::make_tuple(boost::beast::http::status::ok, m_Request.version())
	};
	res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
	res.set(boost::beast::http::field::content_type, m_MediaType.determine(path));
	res.content_length(size);
	res.keep_alive(m_Request.keep_alive());
	return res;
}

/// Based on RFC-6750: "The OAuth 2.0 Authorization Framework: Bearer Token Usage"
/// See: https://tools.ietf.org/html/rfc6750#section-2
/// Note: we are skipping "Form-Encoded Body Parameter" because WebSocket handshake
///       requests can not have a body.
/// But we add the WebSocket specific workaround as described here:
///   https://stackoverflow.com/a/4361358
std::string HttpServerConnection::ws_get_access_token()
{
	const auto& auth_header = m_Request[boost::beast::http::field::authorization];
	if (!auth_header.empty())
	{
		const auto bearer_prefix = std::string {"Bearer "};
		if (auth_header.starts_with(bearer_prefix))
		{
			return auth_header.substr(bearer_prefix.size()).to_string();
		}
		
		const auto basic_prefix = std::string {"Basic "};
		if (auth_header.starts_with(basic_prefix))
		{
			std::string username_password;
			if (!base64decode(username_password, auth_header.substr(basic_prefix.size())))
			{
				return "";
			}

			if (username_password.find(':') != std::string::npos)
			{
				return "";
			}

			return username_password;
		}
		
		// unknown authorization type
		return "";
	}

	// Pass access_token as a protocoal via Sec-WebSocket-Protocol header
	// In Browser: new WebSocket(url, ['access_token', '${access_token}'])
	const auto& ws_protocol = m_Request[boost::beast::http::field::sec_websocket_protocol];
	if (!ws_protocol.empty())
	{
		const auto access_token_prefix = std::string {"access_token, "};
		if (ws_protocol.starts_with(access_token_prefix))
		{
			return ws_protocol.substr(access_token_prefix.size()).to_string();
		}
	}

	// TODO: fall back to query parameter `access_token`.

	return "";
}

void HttpServerConnection::ws_do_upgrade()
{
	m_IsUpgraded = true;
	m_WsSocket.async_accept_ex(
		m_Request,
		std::bind(&HttpServerConnection::ws_on_decorate, shared_from_this(), std::placeholders::_1),
		std::bind(&HttpServerConnection::ws_on_handshake, shared_from_this(), std::placeholders::_1)
	);
}

void HttpServerConnection::ws_on_decorate(boost::beast::websocket::response_type& m)
{
	m.insert(boost::beast::http::field::server, "SVObserver");
}

void HttpServerConnection::ws_access_log()
{
	// Create dummy response for logging. Acutal response is sent by boost::beast already.
	boost::beast::http::response<boost::beast::http::empty_body> Response {
		boost::beast::http::status::ok,
		m_Request.version()
	};
	http_access_log(Response);
}

void HttpServerConnection::ws_on_handshake(const boost::system::error_code& error)
{
	if (error)
	{
		ws_on_error(error);
		return;
	}

	if (m_rSettings.pEventHandler)
	{
		m_rSettings.pEventHandler->onConnect(m_ConnectionId, *this);
	}

	ws_do_read();
}

void HttpServerConnection::ws_do_read()
{
	m_WsBuf.resize(m_rSettings.ReadBufferSize);
	boost::asio::async_read(m_WsSocket,
		boost::asio::buffer(m_WsBuf),
		boost::asio::transfer_at_least(1),
		std::bind(&HttpServerConnection::ws_on_read,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2));
}

void HttpServerConnection::ws_on_read(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		ws_on_error(error);
		if (m_WsSocket.is_open())
		{
			boost::system::error_code ec;
			m_WsSocket.next_layer().close(ec);
			if (ec)
			{
				BOOST_LOG_TRIVIAL(warning) << "Error while closing websocket connection: " << ec;
			}
		}
		return;
	}

	auto prev_size = m_Payload.size();
	m_Payload.reserve(prev_size + bytes_read + 1);
	m_Payload.insert(m_Payload.end(), m_WsBuf.data(), m_WsBuf.data() + bytes_read);
	m_WsBuf.clear();

	if (m_WsSocket.is_message_done())
	{
		if (m_WsSocket.got_binary())
		{
			if (m_rSettings.pEventHandler)
			{
				m_rSettings.pEventHandler->onBinaryMessage(m_ConnectionId, m_Payload);
			}
		}
		else
		{
			if (m_rSettings.pEventHandler)
			{
				m_rSettings.pEventHandler->onTextMessage(m_ConnectionId, m_Payload);
			}
		}
		m_Payload.clear();
	}

	if (m_WsSocket.is_open())
	{
		ws_do_read();
	}
}

std::future<void> HttpServerConnection::ws_send_message_impl(const std::vector<char>& buf, bool is_binary)
{
	auto promise = std::make_shared<std::promise<void>>();
	m_rIoContext.dispatch([this, buf, is_binary, promise]()
	{
		PendingFrame pendingFrame;
		pendingFrame.IsBinary = is_binary;
		pendingFrame.Frame = std::move(buf);
		pendingFrame.Promise = promise;
		m_FrameQueue.emplace(std::move(pendingFrame));
		ws_send_next_frame();
	});
	return promise->get_future();
}

void HttpServerConnection::ws_send_next_frame()
{
	if (m_FrameQueue.empty() || m_IsSendingFrame)
	{
		return;
	}

	m_IsSendingFrame = true;
	auto& pendingFrame = m_FrameQueue.front();
	m_WsSocket.binary(pendingFrame.IsBinary);
	m_WsSocket.async_write(
		boost::asio::buffer(pendingFrame.Frame),
		std::bind(&HttpServerConnection::ws_on_frame_sent, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void HttpServerConnection::ws_on_frame_sent(const boost::system::error_code& error, size_t bytes_sent)
{
	auto promise = m_FrameQueue.front().Promise;
	if (promise)
	{
		promise->set_value();
	}
	m_FrameQueue.pop();
	m_IsSendingFrame = false;

	if (error)
	{
		ws_on_error(error);
		if (m_WsSocket.is_open())
		{
			boost::system::error_code ec;
			m_WsSocket.next_layer().close(ec);
			if (ec)
			{
				BOOST_LOG_TRIVIAL(warning) << "Error while closing websocket connection: " << ec;
			}
		}
		return;
	}

	ws_send_next_frame();
}

void HttpServerConnection::ws_on_error(const boost::system::error_code& error)
{
	if (!error)
	{
		return;
	}

	if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset ||
		error == boost::asio::error::connection_aborted ||
		error.value() == WSAEBADF) // bad file descriptor, when connection already closed
	{
		if (!m_IsDisconnectErrorHandled)
		{
			BOOST_LOG_TRIVIAL(info) << "[ws] Client disconnected";
			if (m_rSettings.pEventHandler)
			{
				m_rSettings.pEventHandler->onDisconnect(m_ConnectionId);
			}
		}
		m_IsDisconnectErrorHandled = true;
		return;
	}

	BOOST_LOG_TRIVIAL(warning) << "client connection error: " << error;
}

bool HttpServerConnection::base64decode(std::string& out, const boost::beast::string_view& in)
{
	return SVEncodeDecodeUtilities::Base64EncodeToStringFromString(out, in.to_string()) == 0;
}

} // namespace SvHttp
