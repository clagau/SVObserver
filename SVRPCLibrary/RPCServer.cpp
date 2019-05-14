//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCServer.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCServer is a thin wrapper around the WebsocketServer. It implements
/// HttpServerConnection's MessageHandler interface for receiving all
/// incoming websocket messages. It assumes they are binary envelopes only.
//******************************************************************************

#include "stdafx.h"

#include "RPCServer.h"

#include "SVLogLibrary/Logging.h"
#include "SVRPCLibrary/ErrorUtil.h"

namespace SvRpc
{
RPCServer::RPCServer(RequestHandlerBase* pRequestHandler) : m_pRequestHandler(pRequestHandler) {}

void RPCServer::onConnect(int id, SvHttp::HttpServerConnection& rConnection)
{
	SV_LOG_GLOBAL(info) << "[http] Client connected id: " << id;
	m_Connections[id] = &rConnection;
	m_ServerStreamContexts[id] = {};
}

bool RPCServer::onHandshake(int id, const std::string& token)
{
	return m_pRequestHandler->onHandshake(token);
}

void RPCServer::onTextMessage(int id, std::vector<char>&&)
{
	throw std::runtime_error("only binary messages expected!");
}

void RPCServer::onBinaryMessage(int id, std::vector<char>&& buf)
{
	if (buf.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
	{
		SV_LOG_GLOBAL(error) << "Message too large " << buf.size() << ". Must not be larger than "
			<< std::numeric_limits<int>::max() << ".";
		return;
	}

	SvPenv::Envelope Request;
	Request.ParseFromArray(buf.data(), static_cast<int>(buf.size()));

	auto type = Request.type();
	switch (type)
	{
		case SvPenv::MessageType::request:
			on_request(id, std::move(Request));
			break;

		case SvPenv::MessageType::streamRequest:
			on_stream(id, std::move(Request));
			break;

		case SvPenv::MessageType::streamCancel:
			on_stream_cancel(id, std::move(Request));
			break;

		case SvPenv::MessageType::streamAck:
			on_stream_ack(id, std::move(Request));
			break;

		default:
			SV_LOG_GLOBAL(error) << "Invalid message type " << type;
			break;
	}
}

void RPCServer::onDisconnect(int id)
{
	SV_LOG_GLOBAL(info) << "RPCServer::onDisconnect(" << id << ")";
	m_Connections.erase(id);
	cancel_stream_contexts(id);
	m_ServerStreamContexts.erase(id);
}

void RPCServer::on_request(int id, SvPenv::Envelope&& Request)
{
	auto txId = Request.transactionid();
	m_pRequestHandler->onRequest(
		std::move(Request),
		Task<SvPenv::Envelope>([this, id, txId](SvPenv::Envelope&& Response) { send_response(id, txId, std::move(Response)); },
		[this, id, txId](const SvPenv::Error& err) { send_error_response(id, txId, err); }));
}

void RPCServer::on_stream(int id, SvPenv::Envelope&& Request)
{
	auto txId = Request.transactionid();
	auto ctx = std::make_shared<ServerStreamContext>();
	m_ServerStreamContexts[id].insert({txId, ctx});
	m_pRequestHandler->onStream(std::move(Request),
		Observer<SvPenv::Envelope>(
		[this, ctx, id, txId](SvPenv::Envelope&& Response) -> SvSyl::SVFuture<void>
	{
		if (ctx->isThrottlingEnabled())
		{
			auto t = ctx->new_throttled_response();
			if (!std::get<0>(t))
			{
				// TODO better status code needed
				auto err = build_error(SvPenv::internalError, "Client overloaded");
				return SvSyl::SVFuture<void>::make_exceptional(errorToExceptionPtr(err));
			}
			auto seqNr = std::get<1>(t);
			send_stream_response(id, txId, seqNr, std::move(Response));
			return std::move(std::get<2>(t));
		}
		else
		{
			uint64_t seqNr = 0;
			return send_stream_response(id, txId, seqNr, std::move(Response));
		}
	},
		[this, ctx, id, txId]()
	{
			send_stream_finish(id, txId);
			remove_stream_context(id, txId);
	},
		[this, ctx, id, txId](const SvPenv::Error& err)
	{
			send_stream_error_response(id, txId, err);
			remove_stream_context(id, txId);
	}),
		ctx);
}

void RPCServer::on_stream_cancel(int id, SvPenv::Envelope&& Request)
{
	auto it = m_ServerStreamContexts.find(id);
	if (it != m_ServerStreamContexts.end())
	{
		auto txId = Request.transactionid();
		auto ctxIt = it->second.find(txId);
		if (ctxIt != it->second.end())
		{
			if (auto ctx = ctxIt->second.lock())
			{
				ctx->cancel();
			}
		}
	}
}

void RPCServer::on_stream_ack(int id, SvPenv::Envelope&& Request)
{
	auto it = m_ServerStreamContexts.find(id);
	if (it != m_ServerStreamContexts.end())
	{
		auto txId = Request.transactionid();
		auto ctxIt = it->second.find(txId);
		if (ctxIt != it->second.end())
		{
			if (auto ctx = ctxIt->second.lock())
			{
				ctx->on_ack(Request.sequencenumber());
			}
		}
	}
}

void RPCServer::cancel_stream_contexts(int id)
{
	auto it = m_ServerStreamContexts.find(id);
	if (it != m_ServerStreamContexts.end())
	{
		for (auto& entry : it->second)
		{
			if (auto ctx = entry.second.lock())
			{
				ctx->cancel();
			}
		}
	}
}

void RPCServer::remove_stream_context(int id, uint64_t txId)
{
	auto it = m_ServerStreamContexts.find(id);
	if (it != m_ServerStreamContexts.end())
	{
		it->second.erase(txId);
	}
}

SvSyl::SVFuture<void> RPCServer::send_response(int id, uint64_t txId, SvPenv::Envelope&& Response)
{
	Response.set_transactionid(txId);
	Response.set_type(SvPenv::MessageType::response);
	return send_envelope(id, Response);
}

SvSyl::SVFuture<void> RPCServer::send_error_response(int id, uint64_t txId, const SvPenv::Error& rError)
{
	SvPenv::Envelope response;
	*response.mutable_error() = rError;
	response.set_transactionid(txId);
	response.set_type(SvPenv::MessageType::errorResponse);
	return send_envelope(id, response);
}

SvSyl::SVFuture<void> RPCServer::send_stream_response(int id, uint64_t txId, uint64_t seqNr, SvPenv::Envelope&& Response)
{
	Response.set_transactionid(txId);
	Response.set_sequencenumber(seqNr);
	Response.set_type(SvPenv::MessageType::streamResponse);
	return send_envelope(id, Response);
}

SvSyl::SVFuture<void> RPCServer::send_stream_error_response(int id, uint64_t txId, const SvPenv::Error& rError)
{
	SvPenv::Envelope Response;
	*Response.mutable_error() = rError;
	Response.set_transactionid(txId);
	Response.set_type(SvPenv::MessageType::streamErrorResponse);
	return send_envelope(id, Response);
}

SvSyl::SVFuture<void> RPCServer::send_stream_finish(int id, uint64_t txId)
{
	SvPenv::Envelope Response;
	Response.set_transactionid(txId);
	Response.set_type(SvPenv::MessageType::streamFinish);
	return send_envelope(id, Response);
}

SvSyl::SVFuture<void> RPCServer::send_envelope(int id, const SvPenv::Envelope& rEnvelope)
{
	auto it = m_Connections.find(id);
	if (it == m_Connections.end())
	{
		SV_LOG_GLOBAL(warning) << "Can not send envelope to connection " << id << ". not found. PayloadType: " << rEnvelope.payloadtype();
		throw ConnectionLostException("Connection lost");
	}

	auto resSize = rEnvelope.ByteSize();
	std::vector<char> buf;
	buf.resize(resSize);
	rEnvelope.SerializeToArray(buf.data(), resSize);

	return it->second->sendBinaryMessage(buf);
}

} // namespace SvRpc
