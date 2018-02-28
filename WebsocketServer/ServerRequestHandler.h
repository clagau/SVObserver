//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ServerRequestHandler.h
/// All Rights Reserved
//******************************************************************************
/// Glue code that maps incoming requests to calls to the RRSSharedMemoryAccess.
/// Most of the logic is just unwrapping the custom request from the envelope,
/// calling the according function from the interface and wrapping the response
/// into an envelope again.
//******************************************************************************

#pragma once

#include "SVRPCLibrary/RequestHandler.h"

namespace RRWS
{

class RRSSharedMemoryAccessInterface;

class ServerRequestHandler : public SVRPC::RequestHandler
{
public:
	ServerRequestHandler(RRSSharedMemoryAccessInterface* service);
};
}
