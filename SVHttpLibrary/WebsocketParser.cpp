//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketParser.cpp
/// All Rights Reserved
//******************************************************************************
/// This class implementis the Websocket protocol specified in RFC 6455.
///
/// The parser is not iterative, means it requires the whole frame to be
/// available to eventually parse the frame. At first, it seems like a bad
/// idea, but switching to an iterative parser would safe us keeping the memory
/// of the whole frame. But, then we need to keep the payload in memory
/// throughout multiple parse calls anyways. So either way, we need to keep sth
/// in memory as long as the parser returns INCOMPLETE_FRAME.
//******************************************************************************

#include "stdafx.h"

#include "SVHttpLibrary/WebSocketParser.h"

#include <iostream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/log/trivial.hpp>

#define htonll(x) ((1 == htonl(1)) ? (x) : ((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1 == ntohl(1)) ? (x) : ((uint64_t)ntohl((x)&0xFFFFFFFF) << 32) | ntohl((x) >> 32))

namespace SVHTTP
{
WebSocketParser::FrameType
WebSocketParser::parseFrame(unsigned char* buf, size_t buf_len, size_t& frame_len, std::vector<char>& message)
{
	if (buf_len < 2)
	{
		return FrameType::INCOMPLETE_FRAME;
	}

	unsigned char msg_opcode = buf[0] & 0x0F;
	unsigned char msg_fin = (buf[0] >> 7) & 0x01;
	unsigned char msg_masked = (buf[1] >> 7) & 0x01;

	uint64_t payload_len = 0;
	int header_len = 2;
	uint8_t length_field = buf[1] & (~0x80);
	unsigned int mask = 0;

	if (length_field <= 125)
	{
		payload_len = length_field;
	}
	else if (length_field == 126) // msglen is 16bit
	{
		auto p16 = reinterpret_cast<char*>(buf + 2);
		auto n16 = *reinterpret_cast<uint16_t*>(p16);
		payload_len = ntohs(n16);
		header_len += 2;
	}
	else if (length_field == 127) // msglen is 64bit
	{
		auto p64 = reinterpret_cast<char*>(buf + 2);
		auto n64 = *reinterpret_cast<uint64_t*>(p64);
		payload_len = ntohll(n64);
		header_len += 8;
	}

	if (msg_masked)
	{
		mask = *((unsigned int*)(buf + header_len));
		header_len += 4;
	}

	frame_len = header_len + payload_len;
	if (buf_len < frame_len)
	{
		return FrameType::INCOMPLETE_FRAME;
	}

	if (msg_masked)
	{
		unsigned char* c = buf + header_len;
		for (int i = 0; i < payload_len; i++)
		{
			c[i] = c[i] ^ ((char*)(&mask))[i % 4];
		}
	}

	if (payload_len > 0)
	{
		unsigned char* payload = buf + header_len;
		message.insert(message.end(), payload, payload + payload_len);
	}

	if (!msg_fin && (msg_opcode == 0x1 || msg_opcode == 0x2))
	{
		m_is_binary_continuation = (msg_opcode == 0x2);
	}

	switch (msg_opcode)
	{
		case 0x0:
			if (msg_fin)
			{
				return m_is_binary_continuation ? FrameType::BINARY_FRAME : FrameType::TEXT_FRAME;
			}
			else
			{
				return FrameType::CONTINUATION_FRAME;
			}
		case 0x1:
			return (msg_fin) ? FrameType::TEXT_FRAME : FrameType::CONTINUATION_FRAME;
		case 0x2:
			return (msg_fin) ? FrameType::BINARY_FRAME : FrameType::CONTINUATION_FRAME;
		case 0x8:
			return FrameType::CLOSING_FRAME;
		case 0x9:
			return FrameType::PING_FRAME;
		case 0xA:
			return FrameType::PONG_FRAME;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Received unknown WebSocket opcode " << msg_opcode;
			return FrameType::ERROR_FRAME;
	}
}
static void makeFrameImpl(
	std::vector<char>& buffer, WebSocketParser::FrameType frame_type, const char* msg, size_t msg_len, bool fin)
{
	buffer.clear();
	buffer.reserve(msg_len);

	auto header = (static_cast<unsigned char>(frame_type) & 0x0F);
	if (fin)
	{
		header |= (1 << 7);
	}
	buffer.push_back(header);

	uint64_t size = msg_len;
	if (size <= 125)
	{
		buffer.push_back(static_cast<char>(size));
	}
	else if (size <= 65535) // 16 bit length follows
	{
		buffer.push_back(126);
		auto n16 = htons(static_cast<uint16_t>(size));
		auto p16 = reinterpret_cast<char*>(&n16);
		buffer.insert(buffer.end(), p16, p16 + 2);
	}
	else // 64 bit length follows
	{
		buffer.push_back(127);
		auto n64 = htonll(size);
		auto p64 = reinterpret_cast<char*>(&n64);
		buffer.insert(buffer.end(), p64, p64 + 8);
	}

	buffer.insert(buffer.end(), msg, msg + msg_len);
}
void WebSocketParser::makeFrame(std::vector<char>& buffer, FrameType frame_type, const char* msg, size_t msg_len)
{
	makeFrameImpl(buffer, frame_type, msg, msg_len, true);
}
void WebSocketParser::makeFrames(std::vector<std::vector<char>>& dst,
	FrameType frame_type,
	const char* msg,
	size_t msg_len)
{
	size_t maxPayloadBytesPerFrame = 65535 - 1024;
	auto count = (msg_len / maxPayloadBytesPerFrame) + (msg_len % maxPayloadBytesPerFrame != 1);
	dst.reserve(count);
	for (size_t i = 0; i < msg_len; i += maxPayloadBytesPerFrame)
	{
		auto isFirst = i == 0;
		auto type = isFirst ? frame_type : FrameType::CONTINUATION_FRAME;
		auto isLast = (i + maxPayloadBytesPerFrame >= msg_len);
		std::vector<char> buf;
		auto payload = msg + i;
		auto payload_len = std::min(maxPayloadBytesPerFrame, msg_len - i);
		makeFrameImpl(buf, type, payload, payload_len, isLast);
		dst.emplace_back(std::move(buf));
	}
}
}
