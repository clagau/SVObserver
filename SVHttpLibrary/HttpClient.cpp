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

#include "HttpClient.h"
#include "SVLogLibrary/Logging.h"
#include "SVStatusLibrary/ErrorUtil.h"
#include "SVSystemLibrary/SVFuture.h"

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
	SvSyl::SVPromise<HttpResponse> promise_;

public:
	// Resolver and socket require an io_context
	explicit RestRequest(boost::asio::io_context& ioc)
		: resolver_(ioc)
		, socket_(ioc)
	{
	}

	// Start the asynchronous operation
	SvSyl::SVFuture<HttpResponse> request(HttpRequest req)
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
		
		std::string port = req.Url.port();
		if (port.empty())
		{
			const auto& scheme = req.Url.scheme();
			if (scheme == "http")
			{
				port = "80";
			}
			else if (scheme == "https")
			{
				port = "443";
			}
			else
			{
				auto err = SvStl::build_error(SvPenv::ErrorCode::internalError, "Unsupported url scheme");
				auto ptr = SvStl::errorToExceptionPtr(err);
				promise_.set_exception(ptr);
				return promise_.get_future();
			}
		}
		auto pSharedThis = shared_from_this();
		auto resolveFunctor = [pSharedThis] (boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {return pSharedThis->on_resolve(ec, results); };
		// Look up the domain name
		resolver_.async_resolve(req.Url.host(), port, resolveFunctor);

		return promise_.get_future();
	}

	void on_resolve(
		boost::system::error_code ec,
		boost::asio::ip::tcp::resolver::results_type results)
	{
		if (ec)
		{
			return fail(ec, "resolve");
		}

		auto pSharedThis = shared_from_this();
		auto connectFunctor = [pSharedThis] (const boost::system::error_code& rError, boost::asio::ip::tcp::resolver::iterator it) {return pSharedThis->on_connect(rError, it); };
		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(socket_, results.begin(), results.end(), connectFunctor);
	}

	void on_connect(const boost::system::error_code& rError, boost::asio::ip::tcp::resolver::iterator)
	{
		if (rError)
		{
			return fail(rError, "connect");
		}

		auto pSharedThis = shared_from_this();
		auto writeFunctor = [pSharedThis](boost::system::error_code ec, std::size_t bytes_transferred) {return pSharedThis->on_write(ec, bytes_transferred); };
		// Send the HTTP request to the remote host
		boost::beast::http::async_write(socket_, req_, writeFunctor);
	}

	void on_write(
		boost::system::error_code ec,
		std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "write");

		auto pSharedThis = shared_from_this();
		auto readFunctor = [pSharedThis](boost::system::error_code ec, std::size_t bytes_transferred) {return pSharedThis->on_read(ec, bytes_transferred); };
		// Receive the HTTP response
		boost::beast::http::async_read(socket_, buffer_, res_, readFunctor);
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

		promise_.set_value(std::move(res));
	}

	void fail(boost::system::error_code ec, char const* what)
	{
		std::stringstream ss;
		ss << what << ": " << ec.message();
		const auto str = ss.str();
		auto err = SvStl::build_error(SvPenv::ErrorCode::internalError, str.c_str());
		auto ptr = SvStl::errorToExceptionPtr(err);
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
