//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketParser.h
/// All Rights Reserved
//******************************************************************************
/// This class implementis the Websocket protocol specified in RFC 6455.
///
/// See WebsocketClient or WebsocketServerConnection for how to use this class,
/// especially how to handle the FrameType return type.
///
/// Use makeFrame to generate a valid Websocket frame for your payload and use
/// makeFrames to split the payload into multiple frames of ~64k each.
///
/// ATTENTION: parser is stateful. Do not call parseFrame for multiple
///            connections.
//******************************************************************************

#pragma once

#include <string>
#include <vector>

namespace SvHttp
{
class WebSocketParser
{
public:
	enum FrameType
	{
		ERROR_FRAME = 0xFF00,
		INCOMPLETE_FRAME = 0xFE00,

		OPENING_FRAME = 0x3300,
		CLOSING_FRAME = 0x3400,

		CONTINUATION_FRAME = 0x00,

		TEXT_FRAME = 0x81,
		BINARY_FRAME = 0x82,

		PING_FRAME = 0x19,
		PONG_FRAME = 0x1A
	};

	FrameType parseFrame(unsigned char* buf, size_t buf_len, size_t& frame_len, std::vector<char>& payload);
	void makeFrame(std::vector<char>& dst, FrameType frame_type, const char* msg, size_t msg_len);
	void makeFrames(std::vector<std::vector<char>>& dst, FrameType frame_type, const char* msg, size_t msg_len);

private:
	// in case of continuation, we need to keep info
	// whether first frame was binary or text
	bool m_IsBinaryContinuation;
};

} // namespace SvHttp
