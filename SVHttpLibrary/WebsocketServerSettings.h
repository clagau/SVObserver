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

namespace SVHTTP
{
struct WebsocketServerSettings
{
	/// Host the tcp server binds to. Use "127.0.0.1" to list locally only
	/// and use "0.0.0.0" on all public interfaces (including localhost).
	/// Or use a specific IP of one of your network interfaces to bind to
	/// a particular interface only.
	std::string host {"0.0.0.0"};

	/// Port the tcp server binds to. Make sure you start the service with
	/// an administrative account when binding to ports <1024.
	uint16_t port {8080};

	/// Size of the read buffer in bytes. Tweak to get an optimal trade-off
	/// between memory usage and throughput. A value between 2k and 64k is
	/// recommended.
	uint32_t read_buffer_size {65536};

	/// The interval inseconds of how often to send a ping request to the
	/// client.
	uint32_t ping_interval_sec {2};

	/// The number of ping messages without a pong response after which the
	/// client will be automatically disconnected.
	uint32_t ping_timeout_count {3};

	/// The server will do regular clean up of closed connections. Choose
	/// a small values like 1 second if you have many short-lived
	/// connections. Use a higher value like something between 5 or 10 to
	/// have a lower load.
	uint32_t connection_cleanup_interval_sec {1};

	/// Allows to automatically split large websocket payloads throughout
	/// multiple websocket frames, a.k.a a fragmented message. Usually,
	/// the OS is taking care of handling large TCP packets in a performant
	/// manner. Enable this to check whether it improves throughput.
	bool split_large_payload {false};
};
}
