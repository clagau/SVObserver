//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RestHandler.h
/// All Rights Reserved
//******************************************************************************
/// Central class for handling authentication and authorization requests.
//******************************************************************************

#pragma once

#include "SVAuth/AuthManager.h"
#include "SvHttpLibrary/HttpRequest.h"
#include "SvHttpLibrary/HttpResponse.h"

namespace SvAuth
{

class RestHandler
{
public:
	RestHandler(AuthManager&);

	bool onRestRequest(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res);

private:
	AuthManager& m_rAuthManager;
};

} // namespace SvAuth
