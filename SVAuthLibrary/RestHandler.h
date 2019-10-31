//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RestHandler.h
/// All Rights Reserved
//******************************************************************************
/// Central class for handling authentication and authorization requests.
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvHttp
{
class HttpRequest;
class HttpResponse;
}

namespace SvAuth
{
class AuthManager;

class RestHandler
{
public:
	explicit RestHandler(AuthManager&);

	bool onRestRequest(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res);

private:
	AuthManager& m_rAuthManager;
};

} // namespace SvAuth
