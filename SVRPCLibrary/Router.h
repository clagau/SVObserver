//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Router.h
/// All Rights Reserved
//******************************************************************************
/// Helper class that automatically forwards all messages not handled by the
/// given RequestHandler to the given RPC Client
//******************************************************************************

#pragma once

#pragma region Includes
#include "RequestHandler.h"
#pragma endregion Includes

namespace SvRpc
{
class RPCClient;

class Router
{
public:
	Router(RPCClient& rClient, RequestHandler& rRequestHandler);
};

} // namespace SvRpc
