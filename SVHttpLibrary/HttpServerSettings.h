//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpServerSettings.h
/// All Rights Reserved
//******************************************************************************
/// Use these settings to congiure the Http server.
///
/// See inline comments for details about each setting.
//******************************************************************************

#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include "DefaultSettings.h"

namespace SvHttp
{
class HttpRequest;
class HttpResponse;
class HttpServerConnection;

class EventHandler
{
protected:
	friend class HttpServerConnection;

	/// When new connection is established, the bearer token of the handshake
	/// request will be extracted and passed to this handler. When the
	/// handler returns true, the handshake is authenticated, when false is
	/// returned, the handshake is aborted with http status code 403.
	virtual bool onHandshake(int id, const std::string& access_token) = 0;

	/// Will be called after successful WebSocket handshake and connection
	/// is fully established. The Connection handle can be used to send
	/// messages to the client.
	virtual void onConnect(int id, HttpServerConnection&) = 0;

	/// Will be called on incoming text message.
	virtual void onTextMessage(int id, std::vector<char>&&) = 0;

	/// Will be called on incoming binary message.
	virtual void onBinaryMessage(int id, std::vector<char>&&) = 0;

	/// Will be called when connection was closed. Can be triggered by
	/// either server-side or client-side.
	virtual void onDisconnect(int id) = 0;
};

struct HttpServerSettings
{
	/// Host the tcp server binds to. Use "127.0.0.1" to list locally only
	/// and use "0.0.0.0" on all public interfaces (including localhost).
	/// Or use a specific IP of one of your network interfaces to bind to
	/// a particular interface only.
	std::string Host {"127.0.0.1"};

	/// Port the tcp server binds to. Make sure you start the service with
	/// an administrative account when binding to ports <1024.
	uint16_t Port {SvHttp::Default_Port};

	/// Size of the read buffer in bytes. Tweak to get an optimal trade-off
	/// between memory usage and throughput. A value between 2k and 64k is
	/// recommended.
	uint32_t ReadBufferSize {65536};

	/// Size of the write buffer in bytes. Tweak to get an optimal trade-off
	/// between memory usage and throughput. A value between 2k and 64k is
	/// recommended.
	uint32_t WriteBufferSize {65536};

	/// Maximum size of a single Websocket message. Use some sane default that
	/// will cover all of your use cases and still prevents the server/client
	/// from running out of memory.
	uint32_t MaxMessageSize {256 * 1024 * 1024};

	/// The server will do regular clean up of closed connections. Choose
	/// a small values like 1 second if you have many short-lived
	/// connections. Use a higher value like something between 5 or 10 to
	/// have a lower load.
	uint32_t ConnectionCleanupIntervalSec {1};

	/// Use this to implement custom custom http endpoints. The caller must
	/// take care of sending the response via the provided HttpResponse object.
	/// Return true if request handled, false if the HttpServer should continue
	/// handling the request. Sending data to the client and returning false will
	/// result in undefined behaviour.
	std::function<bool(const HttpRequest& req, HttpResponse& res)> HttpRequestHandler;

	/// Event Handler for all WebSocket events.
	EventHandler* pEventHandler {nullptr};

	/// Enable serving of static files, i.e. act as a WebServer.
	bool bEnableFileServing {false};

	/// Data dir that is used for serving.
	std::filesystem::path DataDir = {"."};

	/// Default index html file
	std::string DefaultIndexHtmlFile = {"index.html"};

	/// Default 404 html file
	std::string DefaultErrorHtmlFile = {"404.html"};

	/// Version string returned by the http server inside the `Server` header.
	std::string ServerVersionString = {"SVGateway 1.0"};
};

} // namespace SvHttp
