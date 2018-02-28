//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Handshake.h
/// All Rights Reserved
//******************************************************************************
/// The Handshake class is responsible for handling the initial Websocket
/// handshake as described in section 1.3 [1] and 1.4 [2] of the Websocket
/// RFC 6455.
///
/// A Websocket connection starts as a normal http request and when both parties
/// agree, is upgraded to a bidirectional TCP connection. Right now, the
/// Handshake class is not handling any authentication or different protocols.
///
/// We use Joyent's http_parser (also used in Node.js) for parsing the http
/// request.
///
/// ATTENTION: the class is stateful. Must not be used to parse multiple
///            requests in parallel.
///
/// [1] https://tools.ietf.org/html/rfc6455#section-1.3
/// [2] https://tools.ietf.org/html/rfc6455#section-1.4
//******************************************************************************

#pragma once

#include <map>
#include <string>

#include "SVHttpLibrary/http_parser.h"

namespace SVHTTP
{
class Handshake
{
public:
	enum ParserState
	{
		PS_SUCCESS,    // successful handshake. proceed with websocket.
		PS_INCOMPLETE, // incomplete handshake. continue reading data.
		PS_INVALID,    // invalid handshake. close the connection.
		PS_ERROR,      // error ocured. close the connection.
	};

	ParserState parseClientRequest(char* buf, size_t len);
	ParserState parseServerResponse(char* buf, size_t len);

	std::string generateClienRequest();
	std::string generateServerResponse();

private:
	void init_parser(bool parse_request);

	// http_parser hooks
	static int on_url_cb(http_parser*, const char* at, size_t length);
	int on_url(http_parser*, const char* at, size_t length);
	static int on_header_field_cb(http_parser*, const char* at, size_t length);
	int on_header_field(http_parser*, const char* at, size_t length);
	static int on_header_value_cb(http_parser*, const char* at, size_t length);
	int on_header_value(http_parser*, const char* at, size_t length);
	static int on_message_complete_cb(http_parser* parser);
	int on_message_complete(http_parser* parser);

	std::map<std::string, std::string>::iterator find_header(const std::string&);

private:
	http_parser_settings m_settings;
	http_parser m_parser;

	bool m_initialized {false};
	bool m_complete {false};
	std::string m_url;
	std::string m_curr_header;
	std::map<std::string, std::string> m_headers;
};
}
