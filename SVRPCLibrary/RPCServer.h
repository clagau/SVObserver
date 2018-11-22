//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCServer.h
/// All Rights Reserved
//******************************************************************************
/// The RPCServer implements our protobuf-based RPC protocol using a Websocket
/// server. It implements the  HttpServerConnection's MessageHandler
/// interface for receiving all incoming websocket messages.
///
/// It assumes all incoming messages are binary protobuf envelopes. It
/// deserializes and dispatches them using the provided RequestHandler as well
/// as serializing the responses and sending them to correct according websocket
/// conenction.
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <vector>

#include "SvHttpLibrary/HttpServerConnection.h"
#include "RequestHandlerBase.h"
#include "SVProtoBuf/envelope.h"

namespace SvRpc
{
struct ConnectionLostException : public std::runtime_error
{
	explicit ConnectionLostException(const std::string& msg) : std::runtime_error(msg)
	{
	}
	explicit ConnectionLostException(const char* msg) : std::runtime_error(msg)
	{
	}
};

class RPCServer : public SvHttp::EventHandler
{
public:
	RPCServer(RequestHandlerBase* request_handler);

protected:
	virtual bool onHandshake(int id, const std::string&) override;
	virtual void onConnect(int id, SvHttp::HttpServerConnection&) override;
	virtual void onTextMessage(int id, const std::vector<char>&) override;
	virtual void onBinaryMessage(int id, const std::vector<char>&) override;
	virtual void onDisconnect(int id) override;

private:
	void on_request(int id, SvPenv::Envelope&&);
	void on_stream(int id, SvPenv::Envelope&&);
	void on_stream_cancel(int id, SvPenv::Envelope&&);

	void cancel_stream_contexts(int id);
	void remove_stream_context(int id, uint64_t txId);

	std::future<void> send_response(int id, uint64_t txId, SvPenv::Envelope&& Response);
	std::future<void> send_error_response(int id, uint64_t txId, const SvPenv::Error& rError);
	std::future<void> send_stream_response(int id, uint64_t txId, SvPenv::Envelope&& Response);
	std::future<void> send_stream_error_response(int id, uint64_t txId, const SvPenv::Error& rError);
	std::future<void> send_stream_finish(int id, uint64_t txId);
	std::future<void> send_envelope(int id, const SvPenv::Envelope& rEnvelope);

private:
	RequestHandlerBase* m_pRequestHandler;
	std::map<int, SvHttp::HttpServerConnection*> m_Connections;
	std::map<int, std::map<uint64_t, std::weak_ptr<ServerStreamContext>>> m_ServerStreamContexts;
};

} // namespace SvRpc
