//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketServer.h
/// All Rights Reserved
//******************************************************************************
/// Use these settings to congiure the Websocket server.
///
/// See inline comments for details about each setting.
//******************************************************************************

#pragma once

#include <cstdint>
#include <string>

namespace SvHttp
{
struct WebsocketServerSettings
{
	/// Host the tcp server binds to. Use "127.0.0.1" to list locally only
	/// and use "0.0.0.0" on all public interfaces (including localhost).
	/// Or use a specific IP of one of your network interfaces to bind to
	/// a particular interface only.
	std::string Host {"127.0.0.1"};

	/// Port the tcp server binds to. Make sure you start the service with
	/// an administrative account when binding to ports <1024.
	uint16_t Port {8080};

	/// Size of the read buffer in bytes. Tweak to get an optimal trade-off
	/// between memory usage and throughput. A value between 2k and 64k is
	/// recommended.
	uint32_t ReadBufferSize {65536};

	/// Size of the write buffer in bytes. Tweak to get an optimal trade-off
	/// between memory usage and throughput. A value between 2k and 64k is
	/// recommended.
	uint32_t WriteBufferSize {65536};

	/// The interval inseconds of how often to send a ping request to the
	/// client.
	uint32_t PingIntervalSec {5};

	/// The number of ping messages without a pong response after which the
	/// client will be automatically disconnected.
	uint32_t PingTimeoutCount {10};

	/// The server will do regular clean up of closed connections. Choose
	/// a small values like 1 second if you have many short-lived
	/// connections. Use a higher value like something between 5 or 10 to
	/// have a lower load.
	uint32_t ConnectionCleanupIntervalSec {1};
};

} // namespace SvHttp
