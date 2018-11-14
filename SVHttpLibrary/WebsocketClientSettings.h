//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClientSettings.h
/// All Rights Reserved
//******************************************************************************
/// Use these settings to congiure the Websocket client.
///
/// See inline comments for details about each setting.
//******************************************************************************

#pragma once

#include <cstdint>
#include <string>
#include "DefaultSettings.h"

namespace SvHttp
{
struct WebsocketClientSettings
{
	/// Host of the WebSocket server to connect to.
	std::string Host {"127.0.0.1"};

	/// Port of the WebSocket server to connect to.
	uint16_t Port {SvHttp::Default_Port};

	/// Path of the WebSocket server to connect to.
	std::string Path {"/"};

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

	/// The interval in seconds of how often to send a ping request to the
	/// server.
	uint32_t PingIntervalSec {2};

	/// The number of ping messages without a pong response after which the
	/// client will automatically disconnect.
	uint32_t PingTimeoutCount {3};
};

} // namespace SvHttp
