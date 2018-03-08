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

#include "SVRPCLibrary/RPCServer.h"

#include <boost/log/trivial.hpp>

namespace SVRPC
{
RPCServer::RPCServer(RequestHandlerBase* pRequestHandler) : m_pRequestHandler(pRequestHandler) {}

void RPCServer::onConnect(int id, SVHTTP::WebsocketServerConnection& rConnection)
{
	m_Connections[id] = &rConnection;
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

	Envelope request;
	request.ParseFromArray(buf.data(), static_cast<int>(buf.size()));

	auto type = request.type();
	switch (type)
	{
		case MessageType::Request:
			on_request(id, std::move(request));
			break;

		case MessageType::StreamRequest:
			on_stream(id, std::move(request));
			break;

		default:
			BOOST_LOG_TRIVIAL(error) << "Invalid message type " << type;
			break;
	}
}

void RPCServer::onDisconnect(int id)
{
	m_Connections.erase(id);
}

void RPCServer::on_request(int id, Envelope&& request)
{
	auto txId = request.transaction_id();
	m_pRequestHandler->onRequest(
		std::move(request),
		Task<Envelope>([this, id, txId](Envelope&& response) { send_response(id, txId, std::move(response)); },
		[this, id, txId](const Error& err) { send_error_response(id, txId, err); }));
}

void RPCServer::on_stream(int id, Envelope&& request)
{
	auto txId = request.transaction_id();
	m_pRequestHandler->onStream(std::move(request),
		Observer<Envelope>(
		[this, id, txId](Envelope&& response) -> std::future<void>
	{
		return send_stream_response(id, txId, std::move(response));
	},
		[this, id, txId]() { send_stream_finish(id, txId); },
		[this, id, txId](const Error& err) { send_stream_error_response(id, txId, err); }));
}

std::future<void> RPCServer::send_response(int id, uint64_t txId, Envelope&& response)
{
	response.set_transaction_id(txId);
	response.set_type(SVRPC::MessageType::Response);
	return send_envelope(id, response);
}

std::future<void> RPCServer::send_error_response(int id, uint64_t txId, const Error& err)
{
	Envelope response;
	*response.mutable_error() = err;
	response.set_transaction_id(txId);
	response.set_type(SVRPC::MessageType::ErrorResponse);
	return send_envelope(id, response);
}

std::future<void> RPCServer::send_stream_response(int id, uint64_t txId, Envelope&& response)
{
	response.set_transaction_id(txId);
	response.set_type(SVRPC::MessageType::StreamResponse);
	return send_envelope(id, response);
}

std::future<void> RPCServer::send_stream_error_response(int id, uint64_t txId, const Error& err)
{
	Envelope response;
	*response.mutable_error() = err;
	response.set_transaction_id(txId);
	response.set_type(SVRPC::MessageType::StreamErrorResponse);
	return send_envelope(id, response);
}

std::future<void> RPCServer::send_stream_finish(int id, uint64_t txId)
{
	Envelope response;
	response.set_transaction_id(txId);
	response.set_type(SVRPC::MessageType::StreamFinish);
	return send_envelope(id, response);
}

std::future<void> RPCServer::send_envelope(int id, const Envelope& envelope)
{
	auto it = m_Connections.find(id);
	if (it == m_Connections.end())
	{
		BOOST_LOG_TRIVIAL(error) << "Can not send envelope to connection " << id << ". not found.";
		std::promise<void> promise;
		try
		{
			throw std::runtime_error("Example");
		}
		catch (...)
		{
			try
			{
				promise.set_exception(std::current_exception());
			}
			catch (...)
			{
			} // set_exception() may throw too
		}
		return promise.get_future();
	}

	auto resSize = envelope.ByteSize();
	std::vector<char> buf;
	buf.resize(resSize);
	envelope.SerializeToArray(buf.data(), resSize);

	return it->second->sendBinaryMessage(buf);
}

} // namespace SVRPC
