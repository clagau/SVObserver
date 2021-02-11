//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ServerRequestHandler.h
/// All Rights Reserved
//******************************************************************************
/// Glue code that maps incoming requests to calls to the SharedMemoryAccess.
/// Most of the logic is just unwrapping the custom request from the envelope,
/// calling the according function from the interface and wrapping the response
/// into an envelope again.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVRPCLibrary/RequestHandler.h"
#pragma endregion Includes

namespace SvAuth
{
class AuthManager;
}

namespace SvOgw
{

class SharedMemoryAccess;

class ServerRequestHandler : public SvRpc::RequestHandler
{
public:
	ServerRequestHandler(SharedMemoryAccess* service, SvAuth::AuthManager* am);
};
}// namespace SvOgw
