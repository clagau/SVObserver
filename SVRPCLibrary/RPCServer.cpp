//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file RPCServer.cpp
/// All Rights Reserved
//******************************************************************************
/// The RPCServer is a thin wrapper around the WebsocketServer. It implements
/// HttpServerConnection's MessageHandler interface for receiving all
/// incoming websocket messages. It assumes they are binary envelopes only.
//******************************************************************************

#include "stdafx.h"

#include "RPCServer.h"

#include "SVHttpLibrary/HttpServerConnection.h"
#include "SVLogLibrary/Logging.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/SVAuth.h"
#include "SVStatusLibrary/ErrorUtil.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextGenerator.h"

namespace SvRpc
{
RPCServer::RPCServer(RequestHandlerBase* pRequestHandler) : m_pRequestHandler(pRequestHandler) {}

void RPCServer::disconnect_all_connections()
{
	auto ids = connections_ids();
	for (const auto id : ids)
	{
		onDisconnect(id);
	}
}

void RPCServer::onConnect(int id, std::weak_ptr<SvHttp::HttpServerConnection> pConnection)
{
	SV_LOG_GLOBAL(info) << "[http] Client connected id: " << id;
	m_Connections[id] = pConnection;
	m_ServerStreamContexts[id] = {};
	auto pThis = pConnection.lock();
	if (nullptr != pThis)
	{
		m_SessionContexts[id].set_host(pThis->getIpAddress());
	}

	// NOTE: m_SessionContexts[id] already initialized in onHandshake which is called before onConnect
}

bool RPCServer::onHandshake(int id, const std::string& token)
{
	return m_pRequestHandler->onHandshake(token, m_SessionContexts[id]);
}

void RPCServer::onTextMessage(int, std::vector<char>&&)
{
	SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_RPC_ReceivedTextMessage);
	SvStl::MessageManager Exception(SvStl::MsgType::Notify);
	Exception.setMessage(SVMSG_SVO_1_GENERAL_WARNING, SvStl::Tid_RPC_ReceivedTextMessage, SvStl::SourceFileParams(StdMessageParams));
}

void RPCServer::onBinaryMessage(int id, std::vector<char>&& buf)
{
	if (buf.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{:0d}"), buf.size()));
		msgList.push_back(std::format(_T("{}"), std::numeric_limits<int>::max()));
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_RPC_MessageTooLarge, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVO_1_GENERAL_WARNING, SvStl::Tid_RPC_MessageTooLarge, msgList, SvStl::SourceFileParams(StdMessageParams));
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
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{}"), static_cast<int>(type)));
			SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_RPC_InvalidMessageType, msgList);
			SvStl::MessageManager Exception(SvStl::MsgType::Notify);
			Exception.setMessage(SVMSG_SVO_1_GENERAL_WARNING, SvStl::Tid_RPC_InvalidMessageType, msgList, SvStl::SourceFileParams(StdMessageParams));
			break;
	}
}

void RPCServer::onDisconnect(int id)
{
	SV_LOG_GLOBAL(info) << "RPCServer::onDisconnect(" << id << ")";
	m_Connections.erase(id);
	m_SessionContexts.erase(id);
	cancel_stream_contexts(id);
	m_ServerStreamContexts.erase(id);
}

void RPCServer::on_request(int id, SvPenv::Envelope&& Request)
{
	auto txId = Request.transactionid();
	m_pRequestHandler->onRequest(
		m_SessionContexts[id],
		std::move(Request),
		Task<SvPenv::Envelope>([this, id, txId](SvPenv::Envelope&& Response) { send_response(id, txId, std::move(Response)); },
		[this, id, txId](const SvPenv::Error& err) { send_error_response(id, txId, err); }));
}

void RPCServer::on_stream(int id, SvPenv::Envelope&& Request)
{
	auto txId = Request.transactionid();
	auto ctx = std::make_shared<ServerStreamContext>();
	m_ServerStreamContexts[id].insert({txId, ctx});
	m_pRequestHandler->onStream(m_SessionContexts[id], std::move(Request),
		Observer<SvPenv::Envelope>(
		[this, ctx, id, txId](SvPenv::Envelope&& Response) -> SvSyl::SVFuture<void>
	{
		if (ctx->isCancelled())
		{
			SV_LOG_GLOBAL(info) << "RPCServer::on_stream on cancelled context " << id;
			remove_stream_context(id, txId);
			return SvSyl::SVFuture<void>::make_ready();
		}
		if (ctx->isThrottlingEnabled())
		{
			auto t = ctx->new_throttled_response();
			if (!std::get<0>(t))
			{
				// TODO better status code needed
				auto err = SvStl::build_error(SvPenv::internalError, "Client overloaded");
				return SvSyl::SVFuture<void>::make_exceptional(SvStl::errorToExceptionPtr(err));
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
		SV_LOG_GLOBAL(debug) << "Can not send envelope to connection " << id << ". not found. PayloadType: " << rEnvelope.payloadtype();
		return SvSyl::SVFuture<void>::make_ready();
	}

	SV_LOG_GLOBAL(trace) << "Response with SVRC MessageType:" << rEnvelope.payloadtype();
	auto resSize = rEnvelope.ByteSizeLong();
	std::vector<char> buf;
	buf.resize(resSize);
	rEnvelope.SerializeToArray(buf.data(), static_cast<int> (resSize));
	auto pThis = it->second.lock();

	return (nullptr != pThis) ? pThis->sendBinaryMessage(buf) : SvSyl::SVFuture<void>::make_ready();
}

std::vector<int> RPCServer::connections_ids() const
{
	auto ids = std::vector<int>{};
	for (const auto& conn : m_Connections)
	{
		ids.push_back(conn.first);
	}
	return ids;
}


} // namespace SvRpc
