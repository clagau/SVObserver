//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#include "url.hpp"

#include "SVLogLibrary/Logging.h"
#include "SVMessage/SVMessage.h"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextGenerator.h"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServerConnection.h"
#include "HttpServerSettings.h"


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
	m_WsSocket.write_buffer_bytes(rSettings.WriteBufferSize);
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

void HttpServerConnection::close()
{
	std::weak_ptr<HttpServerConnection> pWeakThis{ shared_from_this() };
	auto closeImplFunctor = [pWeakThis]()
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->close_impl();
		}
	};

	m_rIoContext.dispatch(closeImplFunctor);
}

void HttpServerConnection::close_impl()
{
	if (!m_IsUpgraded)
	{
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
	}
	else
	{
		if (m_WsSocket.is_open())
		{
			boost::system::error_code ec;
			m_WsSocket.next_layer().close(ec);
			if (ec)
			{
				std::stringstream ss;
				ss << ec;
				SvDef::StringVector msgList;
				msgList.push_back(ss.str());
				SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Http_ConnectionCloseError, msgList);
				SvStl::MessageManager Exception(SvStl::MsgType::Notify);
				Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_Http_ConnectionCloseError, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
}

SvSyl::SVFuture<void> HttpServerConnection::sendTextMessage(const std::vector<char>& buf)
{
	return ws_send_message_impl(buf, false);
}

SvSyl::SVFuture<void> HttpServerConnection::sendBinaryMessage(const std::vector<char>& buf)
{
	return ws_send_message_impl(buf, true);
}

void HttpServerConnection::http_do_read()
{
	m_Request = {};
	std::weak_ptr<HttpServerConnection> pWeakThis{ shared_from_this() };
	auto readFunctor = [pWeakThis](const boost::system::error_code& rError, size_t bytes)
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->http_on_read(rError, bytes);
		}
	};
	boost::beast::http::async_read(m_Socket, m_Buf, m_Request, readFunctor);
}

void HttpServerConnection::http_on_read(const boost::system::error_code& error, size_t )
{
	if (error)
	{
		http_on_error(error);
		http_do_close();
		return;
	}

	m_ReceivedAt = std::chrono::system_clock::now();

	auto target = m_Request.target();
	Url url;

	try
	{
		// The Url constructor does not trigger parsing.
		// Only calling one of the getter will trigger Url::lazy_parse().
		url = Url(std::string(target));
		url.path();
	}
	catch (const Url::parse_error& pe)
	{
		SV_LOG_GLOBAL(debug) << "Received request for invalid path \"" << target << "\": " << pe.what();
		return http_do_write(http_build_bad_request("Illegal URL"));
	}

	const auto& url_path = url.path();
	if (url_path.empty() || url_path[0] != '/' || url_path.find("..") != std::string::npos)
	{
		return http_do_write(http_build_bad_request("Illegal path"));
	}

	// Check for WebSocket Upgrade
	if (boost::beast::websocket::is_upgrade(m_Request))
	{
		std::string protocol;
		const auto token = ws_get_access_token(protocol);
		const auto handler = m_rSettings.pEventHandler;
		if (handler && !handler->onHandshake(m_ConnectionId, token))
		{
			return http_do_write(http_build_not_authorized("Missing or invalid token"));
		}

		// From here on, http_do_write is not called any more.
		// So let's trigger the access log explicitly.
		// TODO: shall we delay it until handshake was done?
		ws_access_log();

		return ws_do_upgrade(protocol);
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
	auto path = std::filesystem::path(m_rSettings.DataDir).append(url_path.substr(1));
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
		ec.clear();
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

void HttpServerConnection::http_on_write(const boost::system::error_code& error, size_t , bool close)
{
	// release memory we needed to keep during write
	m_EmptyResponse = {};
	m_FileResponse = {};
	m_StringResponse = {};

	if (error)
	{
		http_on_error(error);
		http_do_close();
		return;
	}

	if (close)
	{
		SV_LOG_GLOBAL(debug) << "No keep-alive connection. Closing...";
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

	m_IsHttpErrorOccurred = true;

	if (error == boost::asio::error::eof ||
		error == boost::asio::error::connection_aborted ||
		error == boost::asio::error::connection_reset ||
		error == boost::beast::http::error::end_of_stream ||
		error.default_error_condition() == boost::system::errc::connection_aborted ||
		error.default_error_condition() == boost::system::errc::connection_reset ||
		error.value() == WSAEBADF) // bad file descriptor, when connection already closed
	{
		SV_LOG_GLOBAL(info) << "[http] Client disconnected";
	}
	else
	{
		std::stringstream ss;
		ss << error;
		SvDef::StringVector msgList;
		msgList.push_back(ss.str());
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Http_ConnectionError, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_Http_ConnectionError, msgList, SvStl::SourceFileParams(StdMessageParams));
	}

	if (!m_IsDisconnectErrorHandled)
	{
		if (m_rSettings.pEventHandler)
		{
			m_IsDisconnectErrorHandled = true;
			m_rSettings.pEventHandler->onDisconnect(m_ConnectionId);
		}
	}
}

void HttpServerConnection::http_do_close()
{
	SV_LOG_GLOBAL(debug) << "Closing connection";
	if (!m_Socket.is_open())
	{
		return;
	}

	boost::system::error_code ec;
	// There was an error on the connection, so no need to do a graceful shutdown
	if (m_IsHttpErrorOccurred)
	{
		m_Socket.close(ec);
	}
	else
	{
		m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
	}
	if (ec)
	{
		std::stringstream ss;
		ss << ec;
		SvDef::StringVector msgList;
		msgList.push_back(ss.str());
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Http_ConnectionCloseError, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_Http_ConnectionCloseError, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
}

template<class Body>
void HttpServerConnection::http_do_write_impl(boost::beast::http::response<Body>& Response)
{
	http_access_log(Response);
	bool close = Response.need_eof();
	std::weak_ptr<HttpServerConnection> pWeakThis{ shared_from_this() };
	auto writeFunctor = [pWeakThis, close](const boost::system::error_code& rError, size_t bytes)
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->http_on_write(rError, bytes, close);
		}
	};
	boost::beast::http::async_write(m_Socket, Response, writeFunctor);
}

void HttpServerConnection::http_do_write(boost::beast::http::response<boost::beast::http::empty_body>&& Response)
{
	m_EmptyResponse = std::move(Response);
	http_do_write_impl(m_EmptyResponse);
}

void HttpServerConnection::http_do_write(boost::beast::http::response<boost::beast::http::string_body>&& Response)
{
	m_StringResponse = std::move(Response);
	http_do_write_impl(m_StringResponse);
}

void HttpServerConnection::http_do_write(boost::beast::http::response<boost::beast::http::file_body>&& Response)
{
	m_FileResponse = std::move(Response);
	http_do_write_impl(m_FileResponse);
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

	SV_LOG_GLOBAL(debug)
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
	for (const auto& field : m_Request)
	{
		Request.Headers.emplace(field.name(), field.value());
	}

	HttpResponse Response;
	if (m_rSettings.HttpRequestHandler(Request, Response))
	{
		boost::beast::http::response<boost::beast::http::string_body> res {
			Response.Status,
			m_Request.version()
		};
		res.set(boost::beast::http::field::server, m_rSettings.ServerVersionString);
		res.set(boost::beast::http::field::content_type, Response.ContentType);
		for (const auto& it : Response.Headers)
		{
			res.set(it.first, it.second);
		}
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
HttpServerConnection::http_build_file_head(const std::filesystem::path& path, boost::beast::http::file_body::value_type body)
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
HttpServerConnection::http_build_file_get(const std::filesystem::path& path, boost::beast::http::file_body::value_type body)
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
std::string HttpServerConnection::ws_get_access_token(std::string& rProtocol)
{
	rProtocol.clear();

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

	// Pass access_token as a protocol via Sec-WebSocket-Protocol header
	// In Browser: new WebSocket(url, ['access_token', '${access_token}'])
	const auto& ws_protocol = m_Request[boost::beast::http::field::sec_websocket_protocol];
	if (!ws_protocol.empty())
	{
		const auto access_token_prefix = std::string {"access_token, "};
		if (ws_protocol.starts_with(access_token_prefix))
		{
			rProtocol = "access_token";
			return ws_protocol.substr(access_token_prefix.size()).to_string();
		}
	}

	// TODO: fall back to query parameter `access_token`.

	return "";
}

void HttpServerConnection::ws_do_upgrade(const std::string& rProtocol)
{
	m_IsUpgraded = true;
	std::weak_ptr<HttpServerConnection> pWeakThis{ shared_from_this() };
	auto decorateFunctor = [pWeakThis, rProtocol](boost::beast::websocket::response_type& m)
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->ws_on_decorate(m, rProtocol);
		}
	};
	auto handshakeFunctor = [pWeakThis](const boost::system::error_code& rError)
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->ws_on_handshake(rError);
		}
	};
	m_WsSocket.set_option(boost::beast::websocket::stream_base::decorator(decorateFunctor));
	m_WsSocket.async_accept(m_Request, handshakeFunctor);
}

void HttpServerConnection::ws_on_decorate(boost::beast::websocket::response_type& m, const std::string& rProtocol)
{
	m.insert(boost::beast::http::field::server, "SVObserver");
	if (!rProtocol.empty())
	{
		m.insert(boost::beast::http::field::sec_websocket_protocol, rProtocol);
	}
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
		ws_on_error(error, "ws_on_handshake()");
		return;
	}

	m_IsWebsocketHandshakeDone = true;

	if (m_rSettings.pEventHandler)
	{
		m_rSettings.pEventHandler->onConnect(m_ConnectionId, *this);
	}

	ws_do_read();
}

void HttpServerConnection::ws_do_read()
{
	m_WsBuf.resize(m_rSettings.ReadBufferSize);
	std::weak_ptr<HttpServerConnection> pWeakThis{ shared_from_this() };
	auto readFunctor = [pWeakThis](const boost::system::error_code& rError, size_t bytes_read)
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->ws_on_read(rError, bytes_read);
		}
	};
	boost::asio::async_read(m_WsSocket, boost::asio::buffer(m_WsBuf), boost::asio::transfer_at_least(1), readFunctor);
}

void HttpServerConnection::ws_on_read(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		ws_on_error(error, "ws_on_read()");
		close();
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
				m_rSettings.pEventHandler->onBinaryMessage(m_ConnectionId, std::move(m_Payload));
			}
		}
		else
		{
			if (m_rSettings.pEventHandler)
			{
				m_rSettings.pEventHandler->onTextMessage(m_ConnectionId, std::move(m_Payload));
			}
		}
		m_Payload.clear();
	}

	if (m_WsSocket.is_open())
	{
		ws_do_read();
	}
}

SvSyl::SVFuture<void> HttpServerConnection::ws_send_message_impl(const std::vector<char>& buf, bool is_binary)
{
	auto promise = std::make_shared<SvSyl::SVPromise<void>>();
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
	std::weak_ptr<HttpServerConnection> pWeakThis{ shared_from_this() };
	auto frameSentFunctor = [pWeakThis](const boost::system::error_code& rError, size_t bytes)
	{
		auto pThis = pWeakThis.lock();
		if (nullptr != pThis)
		{
			pThis->ws_on_frame_sent(rError, bytes);
		}
	};
	m_WsSocket.async_write(boost::asio::buffer(pendingFrame.Frame), frameSentFunctor);
}

void HttpServerConnection::ws_on_frame_sent(const boost::system::error_code& error, size_t)
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
		ws_on_error(error, "ws_on_frame_sent()");
		close();
		return;
	}

	ws_send_next_frame();
}

void HttpServerConnection::ws_on_error(const boost::system::error_code& error, const char*)
{
	if (!error)
	{
		return;
	}

	// Do not print one of the known/common errors that are emitted when connection is closed
	if (error == boost::asio::error::eof ||
		error == boost::asio::error::connection_reset ||
		error == boost::asio::error::connection_aborted ||
		error == boost::asio::error::operation_aborted ||
		error == boost::beast::websocket::error::closed ||
		error.value() == WSAEBADF) // bad file descriptor, when connection already closed
	{
		SV_LOG_GLOBAL(info) << "[ws] Client disconnected";
	}
	else
	{
		std::stringstream ss;
		ss << error;
		SvDef::StringVector msgList;
		msgList.push_back(ss.str());
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Http_ConnectionError, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVO_1_GENERAL_WARNING, SvStl::Tid_Http_ConnectionError, msgList, SvStl::SourceFileParams(StdMessageParams));
	}

	// When error happens during handshake, we did not even call the onConnect yet.
	// So only trigger onDisconnect when already connected.
	if (m_IsWebsocketHandshakeDone)
	{
		if (!m_IsDisconnectErrorHandled)
		{
			if (m_rSettings.pEventHandler)
			{
				m_IsDisconnectErrorHandled = true;
				m_rSettings.pEventHandler->onDisconnect(m_ConnectionId);
			}
		}
	}
}

bool HttpServerConnection::base64decode(std::string& out, const boost::beast::string_view& in)
{
	return SVEncodeDecodeUtilities::Base64EncodeToStringFromString(out, in.to_string()) == 0;
}

} // namespace SvHttp
