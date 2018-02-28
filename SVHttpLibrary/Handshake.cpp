//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Handshake.cpp
/// All Rights Reserved
//******************************************************************************
/// The Handshake class is responsible for handling the initial Websocket
/// handshake. See Handshake.h for details.
//******************************************************************************

#include "stdafx.h"

#include <Wincrypt.h>

#include <algorithm>

#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/Handshake.h"

namespace SVHTTP
{
Handshake::ParserState Handshake::parseClientRequest(char* buf, size_t len)
{
	if (!m_initialized)
	{
		init_parser(/*parse_request=*/true);
		m_initialized = true;
	}
	auto nparsed = http_parser_execute(&m_parser, &m_settings, buf, len);
	if (nparsed != len)
	{
		BOOST_LOG_TRIVIAL(warning) << "Error while parsing incoming http request! " << m_parser.http_errno;
		return PS_ERROR;
	}

	if (!m_complete)
	{
		return PS_INCOMPLETE;
	}

	// TODO: check all headers
	if (m_parser.upgrade != 1 || m_parser.method != HTTP_GET)
	{
		return PS_INVALID;
	}

	return PS_SUCCESS;
}

Handshake::ParserState Handshake::parseServerResponse(char* buf, size_t len)
{
	if (!m_initialized)
	{
		init_parser(/*parse_request=*/false);
		m_initialized = true;
	}
	auto nparsed = http_parser_execute(&m_parser, &m_settings, buf, len);
	if (nparsed != len)
	{
		BOOST_LOG_TRIVIAL(warning) << "Error while parsing incoming http request! " << m_parser.http_errno;
		return PS_ERROR;
	}

	if (!m_complete)
	{
		return PS_INCOMPLETE;
	}

	// TODO: check all headers
	if (m_parser.upgrade != 1 || m_parser.status_code != 101)
	{
		return PS_INVALID;
	}

	return PS_SUCCESS;
}

/// Generates an arbitrary, but valid Websocket Handshake request.
std::string Handshake::generateClienRequest()
{
	auto handshake = std::string();
	handshake += "GET / HTTP/1.1\r\n";
	handshake += "Host: rrs.svo.seidenader.intern\r\n";
	handshake += "Upgrade: websocket\r\n";
	handshake += "Connection: Upgrade\r\n";
	handshake += "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n";
	handshake += "Origin: http://example.com\r\n";
	handshake += "Sec-WebSocket-Protocol: seidenader-svo\r\n";
	handshake += "Sec-WebSocket-Version: 13\r\n";
	handshake += "\r\n";
	return handshake;
}

static std::vector<char> sha1(std::vector<char> str)
{
	static const auto SHA1LEN = 20;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE rgbHash[SHA1LEN];
	CHAR rgbDigits[] = "0123456789abcdef";
	DWORD cbHash = 0;

	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		BOOST_LOG_TRIVIAL(warning) << "CryptAcquireContext failed: " << GetLastError();
		throw std::runtime_error("CryptAcquireContext failed");
	}

	if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
	{
		BOOST_LOG_TRIVIAL(warning) << "CryptAcquireContext failed: " << GetLastError();
		CryptReleaseContext(hProv, 0);
		throw std::runtime_error("CryptAcquireContext failed");
	}

	if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(str.data()), static_cast<DWORD>(str.size()), 0))
	{
		BOOST_LOG_TRIVIAL(warning) << "CryptHashData failed: " << GetLastError();
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		throw std::runtime_error("CryptHashData failed");
	}

	cbHash = SHA1LEN;
	if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		BOOST_LOG_TRIVIAL(warning) << "CryptGetHashParam failed: " << GetLastError();
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		throw std::runtime_error("CryptGetHashParam failed");
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return std::vector<char>{rgbHash, rgbHash + SHA1LEN};
}

static std::string str_to_lower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

static std::vector<char> encode64(const std::vector<char>& buf)
{
	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	auto* bytes_to_encode = reinterpret_cast<unsigned char const*>(buf.data());
	auto in_len = buf.size();

	std::vector<char> ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret.push_back(base64_chars[char_array_4[i]]);
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret.push_back(base64_chars[char_array_4[j]]);

		while ((i++ < 3))
			ret.push_back('=');
	}

	return ret;
}

/// The Websocket RFC requires a special accept header containing the hash of
/// the client key provided by the client.
static std::vector<char> generate_accept_key(const std::string& clientKey)
{
	// TODO: we need more magic!
	static const auto MAGIC_GUID = std::string("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	std::vector<char> buf;
	buf.insert(buf.end(), clientKey.begin(), clientKey.end());
	buf.insert(buf.end(), MAGIC_GUID.begin(), MAGIC_GUID.end());
	return encode64(sha1(std::move(buf)));
}

std::string Handshake::generateServerResponse()
{
	if (!m_complete)
	{
		throw std::runtime_error("Cannot generate response for non-successful handshake request");
	}

	auto handshake = std::string();
	handshake += "HTTP/1.1 101 Switching Protocols\r\n";
	handshake += "Upgrade: websocket\r\n";
	handshake += "Connection: Upgrade\r\n";

	auto keyIt = find_header("Sec-WebSocket-Key");
	if (keyIt != m_headers.end())
	{
		auto accept = generate_accept_key(keyIt->second);
		handshake += "Sec-WebSocket-Accept: ";
		handshake += std::string {accept.begin(), accept.end()};
		handshake += "\r\n";
	}

	if (find_header("Sec-WebSocket-Protocol") != m_headers.end())
	{
		// TODO: validate list of requested protocols
		handshake += "Sec-WebSocket-Protocol: seidenader-svo\r\n";
	}

	handshake += "\r\n";

	return handshake;
}

void Handshake::init_parser(bool parse_request)
{
	http_parser_settings_init(&m_settings);
	m_settings.on_url = Handshake::on_url_cb;
	m_settings.on_header_field = Handshake::on_header_field_cb;
	m_settings.on_header_value = Handshake::on_header_value_cb;
	m_settings.on_message_complete = Handshake::on_message_complete_cb;
	auto type = parse_request ? HTTP_REQUEST : HTTP_RESPONSE;
	http_parser_init(&m_parser, type);
	m_parser.data = this;
}

int Handshake::on_url_cb(http_parser* parser, const char* at, size_t length)
{
	return ((Handshake*)parser->data)->on_url(parser, at, length);
}

int Handshake::on_url(http_parser* parser, const char* at, size_t length)
{
	m_url.append(at, length);
	return 0;
}

int Handshake::on_header_field_cb(http_parser* parser, const char* at, size_t length)
{
	return ((Handshake*)parser->data)->on_header_field(parser, at, length);
}

int Handshake::on_header_field(http_parser* parser, const char* at, size_t length)
{
	m_curr_header = str_to_lower(std::string(at, length));
	return 0;
}

int Handshake::on_header_value_cb(http_parser* parser, const char* at, size_t length)
{
	return ((Handshake*)parser->data)->on_header_value(parser, at, length);
}

int Handshake::on_header_value(http_parser* parser, const char* at, size_t length)
{
	m_headers[m_curr_header] = std::string(at, length);
	m_curr_header.clear();
	return 0;
}

int Handshake::on_message_complete_cb(http_parser* parser)
{
	return ((Handshake*)parser->data)->on_message_complete(parser);
}

int Handshake::on_message_complete(http_parser* parser)
{
	m_complete = true;
	return 0;
}

std::map<std::string, std::string>::iterator Handshake::find_header(const std::string& key)
{
	return m_headers.find(str_to_lower(key));
}
}
