//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCServer.h
/// All Rights Reserved
//******************************************************************************
/// The RPCServer implements our protobuf-based RPC protocol using a Websocket
/// server. It implements the  WebsocketServerConnection's MessageHandler
/// interface for receiving all incoming websocket messages.
///
/// It assumes all incoming messages are binary protobuf envelopes. It
/// deserializes and dispatches them using the provided RequestHandler as well
/// as serializing the responses and sending them to correct according websocket
/// conenction.
//******************************************************************************

#pragma once

#include <memory>
#include <vector>

#include "SVHttpLibrary/WebsocketServerConnection.h"
#include "SVRPCLibrary/RequestHandlerBase.h"
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{
class RPCServer : public SVHTTP::WebsocketServerConnection::EventHandler
{
public:
	RPCServer(RequestHandlerBase* request_handler);

protected:
	virtual void onConnect(int id, SVHTTP::WebsocketServerConnection&) override;
	virtual void onTextMessage(int id, const std::vector<char>&) override;
	virtual void onBinaryMessage(int id, const std::vector<char>&) override;
	virtual void onDisconnect(int id) override;

private:
	void on_request(int id, Envelope&&);
	void on_stream(int id, Envelope&&);

	std::future<void> send_response(int id, uint64_t txId, Envelope&& response);
	std::future<void> send_error_response(int id, uint64_t txId, const Error& err);
	std::future<void> send_stream_response(int id, uint64_t txId, Envelope&& response);
	std::future<void> send_stream_error_response(int id, uint64_t txId, const Error& err);
	std::future<void> send_stream_finish(int id, uint64_t txId);
	std::future<void> send_envelope(int id, const Envelope& envelope);

private:
	RequestHandlerBase* m_request_handler;
	std::map<int, SVHTTP::WebsocketServerConnection*> m_connections;
};
}
