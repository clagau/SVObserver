//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#include "RequestHandlerBase.h"
#include "SvHttpLibrary/HttpServerSettings.h"

namespace SvHttp
{
class HttpServerConnection;
}

namespace SvPenv
{
class Envelope;
class Error;
}

namespace SvRpc
{
class RPCServer : public SvHttp::EventHandler
{
public:
	explicit RPCServer(RequestHandlerBase* request_handler);

	void disconnect_all_connections();

protected:
	virtual bool onHandshake(int id, const std::string&) override;
	virtual void onConnect(int id, std::weak_ptr<SvHttp::HttpServerConnection>) override;
	virtual void onTextMessage(int id, std::vector<char>&&) override;
	virtual void onBinaryMessage(int id, std::vector<char>&&) override;
	virtual void onDisconnect(int id) override;

private:
	void on_request(int id, SvPenv::Envelope&&);
	void on_stream(int id, SvPenv::Envelope&&);
	void on_stream_cancel(int id, SvPenv::Envelope&&);
	void on_stream_ack(int id, SvPenv::Envelope&&);

	void cancel_stream_contexts(int id);
	void remove_stream_context(int id, uint64_t txId);

	SvSyl::SVFuture<void> send_response(int id, uint64_t txId, SvPenv::Envelope&& Response);
	SvSyl::SVFuture<void> send_error_response(int id, uint64_t txId, const SvPenv::Error& rError);
	SvSyl::SVFuture<void> send_stream_response(int id, uint64_t txId, uint64_t seqNr, SvPenv::Envelope&& Response);
	SvSyl::SVFuture<void> send_stream_error_response(int id, uint64_t txId, const SvPenv::Error& rError);
	SvSyl::SVFuture<void> send_stream_finish(int id, uint64_t txId);
	SvSyl::SVFuture<void> send_envelope(int id, const SvPenv::Envelope& rEnvelope);

private:
	std::vector<int> connections_ids() const;

	RequestHandlerBase* m_pRequestHandler;
	std::map<int, std::weak_ptr<SvHttp::HttpServerConnection>> m_Connections;
	std::map<int, SvAuth::SessionContext> m_SessionContexts;
	std::map<int, std::map<uint64_t, std::weak_ptr<ServerStreamContext>>> m_ServerStreamContexts;
};

} // namespace SvRpc
