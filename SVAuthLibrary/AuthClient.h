//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file AuthClient.h
/// All Rights Reserved
//******************************************************************************
/// Central class for handling authentication and authorization requests.
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost/asio/io_context.hpp>

#include "SVProtoBuf/SVAuth.h"
#include "SVHttpLibrary/HttpClient.h"
#include "SVHttpLibrary/url.hpp"
#pragma endregion Includes

namespace SvAuth
{

class AuthClient
{
public:
	AuthClient(boost::asio::io_context&, const std::string& host, uint16_t port);

	bool login(const std::string& user, const std::string& pass, std::string& access_token);
	bool auth(const std::string& access_token, std::string& auth_token);

private:
	Url build_url(const std::string& path);

private:
	SvHttp::HttpClient m_HttpClient;
	const std::string m_Host;
	uint16_t m_Port;
};

} // namespace SvAuth
