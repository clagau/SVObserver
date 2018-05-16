//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RPCServer.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCServer is a thin wrapper around the WebsocketServer. It implements
/// WebsocketServerConnection's MessageHandler interface for receiving all
/// incoming websocket messages. It assumes they are binary envelopes only.
//******************************************************************************

#include "stdafx.h"

#include "RPCServer.h"

#include <boost/log/trivial.hpp>

namespace SvRpc
{
RPCServer::RPCServer(RequestHandlerBase* pRequestHandler) : m_pRequestHandler(pRequestHandler) {}

void RPCServer::onConnect(int id, SvHttp::WebsocketServerConnection& rConnection)
{
	m_Connections[id] = &rConnection;
	m_ServerStreamContexts[id] = {};
}

void RPCServer::onTextMessage(int id, const std::vector<char>&)
{
	throw std::runtime_error("only binary messages expected!");
}

void RPCServer::onBinaryMessage(int id, const std::vector<char>& buf)
{
	if (buf.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
	{
		BOOST_LOG_TRIVIAL(error) << "Message too large " << buf.size() << ". Must not be larger than "
			<< std::numeric_limits<int>::max() << ".";
		return;
	}

	SvPenv::Envelope Request;
	Request.ParseFromArray(buf.data(), static_cast<int>(buf.size()));

	auto type = Request.type();
	switch (type)
	{
		case SvPenv::MessageType::Request:
			on_request(id, std::move(Request));
			break;

		case SvPenv::MessageType::StreamRequest:
			on_stream(id, std::move(Request));
			break;

		case SvPenv::MessageType::StreamCancel:
			on_stream_cancel(id, std::move(Request));
			break;

		default:
			BOOST_LOG_TRIVIAL(error) << "Invalid message type " << type;
			break;
	}
}

void RPCServer::onDisconnect(int id)
{
	m_Connections.erase(id);
	m_ServerStreamContexts.erase(id);
}

void RPCServer::on_request(int id, SvPenv::Envelope&& Request)
{
	auto txId = Request.transaction_id();
	m_pRequestHandler->onRequest(
		std::move(Request),
		Task<SvPenv::Envelope>([this, id, txId](SvPenv::Envelope&& Response) { send_response(id, txId, std::move(Response)); },
		[this, id, txId](const SvPenv::Error& err) { send_error_response(id, txId, err); }));
}

void RPCServer::on_stream(int id, SvPenv::Envelope&& Request)
{
	auto txId = Request.transaction_id();
	auto ctx = std::make_shared<ServerStreamContext>();
	m_ServerStreamContexts[id].insert({txId, ctx});
	m_pRequestHandler->onStream(std::move(Request),
		Observer<SvPenv::Envelope>(
		[this, ctx, id, txId](SvPenv::Envelope&& Response) -> std::future<void>
	{
		//@TODO [gra][8.10][16.05.2018] Temporary resolution to avoid SVOGateaway crashes
		std::future<void> Result;
		try
		{
			Result = send_stream_response(id, txId, std::move(Response));
		}
		catch (const std::exception&)
		{
		}
		return Result;
	},
		[this, ctx, id, txId]()
	{
		//@TODO [gra][8.10][16.05.2018] Temporary resolution to avoid SVOGateaway crashes
		try
		{
			send_stream_finish(id, txId);
			remove_stream_context(id, txId);
		}
		catch (const std::exception&)
		{
		}
	},
		[this, ctx, id, txId](const SvPenv::Error& err)
	{
		//@TODO [gra][8.10][16.05.2018] Temporary resolution to avoid SVOGateaway crashes
		try
		{
			send_stream_error_response(id, txId, err);
			remove_stream_context(id, txId);
		}
		catch (const std::exception&)
		{
		}
	}),
		ctx);
}

void RPCServer::on_stream_cancel(int id, SvPenv::Envelope&& Request)
{
	auto it = m_ServerStreamContexts.find(id);
	if (it != m_ServerStreamContexts.end())
	{
		auto txId = Request.transaction_id();
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

void RPCServer::remove_stream_context(int id, uint64_t txId)
{
	auto it = m_ServerStreamContexts.find(id);
	if (it != m_ServerStreamContexts.end())
	{
		it->second.erase(txId);
	}
}

std::future<void> RPCServer::send_response(int id, uint64_t txId, SvPenv::Envelope&& Response)
{
	Response.set_transaction_id(txId);
	Response.set_type(SvPenv::MessageType::Response);
	return send_envelope(id, Response);
}

std::future<void> RPCServer::send_error_response(int id, uint64_t txId, const SvPenv::Error& rError)
{
	SvPenv::Envelope response;
	*response.mutable_error() = rError;
	response.set_transaction_id(txId);
	response.set_type(SvPenv::MessageType::ErrorResponse);
	return send_envelope(id, response);
}

std::future<void> RPCServer::send_stream_response(int id, uint64_t txId, SvPenv::Envelope&& Response)
{
	Response.set_transaction_id(txId);
	Response.set_type(SvPenv::MessageType::StreamResponse);
	return send_envelope(id, Response);
}

std::future<void> RPCServer::send_stream_error_response(int id, uint64_t txId, const SvPenv::Error& rError)
{
	SvPenv::Envelope Response;
	*Response.mutable_error() = rError;
	Response.set_transaction_id(txId);
	Response.set_type(SvPenv::MessageType::StreamErrorResponse);
	return send_envelope(id, Response);
}

std::future<void> RPCServer::send_stream_finish(int id, uint64_t txId)
{
	SvPenv::Envelope Response;
	Response.set_transaction_id(txId);
	Response.set_type(SvPenv::MessageType::StreamFinish);
	return send_envelope(id, Response);
}

std::future<void> RPCServer::send_envelope(int id, const SvPenv::Envelope& rEnvelope)
{
	auto it = m_Connections.find(id);
	if (it == m_Connections.end())
	{
		BOOST_LOG_TRIVIAL(error) << "Can not send envelope to connection " << id << ". not found.";
		throw ConnectionLostException("Connection lost");
	}

	auto resSize = rEnvelope.ByteSize();
	std::vector<char> buf;
	buf.resize(resSize);
	rEnvelope.SerializeToArray(buf.data(), resSize);

	return it->second->sendBinaryMessage(buf);
}

} // namespace SvRpc
