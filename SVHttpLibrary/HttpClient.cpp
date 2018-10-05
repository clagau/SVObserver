//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpClient.cpp
/// All Rights Reserved
//******************************************************************************
// Simple http client.
//******************************************************************************

#include "stdafx.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "SVLogLibrary/Logging.h"
#include "SVRPCLibrary/ErrorUtil.h"

#include "HttpClient.h"

using namespace boost::beast;

namespace SvHttp
{

namespace {

// Based on https://www.boost.org/doc/libs/1_66_0/libs/beast/example/http/client/async/http_client_async.cpp
class RestRequest : public std::enable_shared_from_this<RestRequest>
{
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::socket socket_;
	boost::beast::flat_buffer buffer_; // (Must persist between reads)
	http::request<http::string_body> req_;
	http::response<http::string_body> res_;
	std::promise<HttpResponse> promise_;

public:
	// Resolver and socket require an io_context
	explicit RestRequest(boost::asio::io_context& ioc)
		: resolver_(ioc)
		, socket_(ioc)
	{
	}

	// Start the asynchronous operation
	std::future<HttpResponse> request(HttpRequest req)
	{
		req_.version(11);
		req_.method(req.Method);
		req_.target(req.Url.path());
		req_.set(boost::beast::http::field::host, req.Url.host());
		req_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		for (const auto& it : req.Headers)
		{
			req_.set(it.first, it.second);
		}
		if (!req.Body.empty())
		{
			req_.body() = req.Body.to_string();
			req_.prepare_payload();
		}
		
		// Look up the domain name
		resolver_.async_resolve(
			req.Url.host(),
			req.Url.port(),
			std::bind(
			&RestRequest::on_resolve,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));

		return promise_.get_future();
	}

	void on_resolve(
		boost::system::error_code ec,
		boost::asio::ip::tcp::resolver::results_type results)
	{
		if (ec)
			return fail(ec, "resolve");

		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(
			socket_,
			results.begin(),
			results.end(),
			std::bind(
			&RestRequest::on_connect,
			shared_from_this(),
			std::placeholders::_1));
	}

	void on_connect(boost::system::error_code ec)
	{
		if (ec)
			return fail(ec, "connect");

		// Send the HTTP request to the remote host
		boost::beast::http::async_write(socket_, req_,
			std::bind(
			&RestRequest::on_write,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
	}

	void on_write(
		boost::system::error_code ec,
		std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "write");

		// Receive the HTTP response
		boost::beast::http::async_read(socket_, buffer_, res_,
			std::bind(
			&RestRequest::on_read,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
	}

	void on_read(
		boost::system::error_code ec,
		std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "read");

		HttpResponse res;
		res.Status = http::int_to_status(res_.result_int());
		res.Body = res_.body();
		for (const auto& field : req_)
		{
			res.Headers.emplace(field.name(), field.value());
		}

		// Gracefully close the socket
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes so don't bother reporting it.
		if (ec && ec != boost::system::errc::not_connected)
			return fail(ec, "shutdown");

		// If we get here then the connection is closed gracefully

		promise_.set_value(res);
	}

	void fail(boost::system::error_code ec, char const* what)
	{
		std::stringstream ss;
		ss << what << ": " << ec.message();
		const auto str = ss.str();
		auto err = SvRpc::build_error(SvPenv::ErrorCode::internalError, str.c_str());
		auto ptr = SvRpc::errorToExceptionPtr(err);
		promise_.set_exception(ptr);
	}
};

} // anonymous namespace

HttpClient::HttpClient(boost::asio::io_context& rIoContext)
	: m_rIoContext(rIoContext)
{
}

HttpResponse HttpClient::request_sync(HttpRequest req)
{
	try
	{
		auto session = std::make_shared<RestRequest>(m_rIoContext);
		auto future = session->request(std::move(req));
		return future.get();
	}
	catch (const std::exception& ex)
	{
		SV_LOG_GLOBAL(error) << ex.what();
		HttpResponse res;
		res.Status = http::status::internal_server_error;
		return res;
	}
}

} // namespace SvHttp
