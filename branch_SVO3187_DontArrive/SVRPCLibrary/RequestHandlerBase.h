//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file RequestHandlerBae.h
/// All Rights Reserved
//******************************************************************************
/// Implement the RequestHandlerBase interface to receive incoming SvPenv::Envelopes of
/// type request or stream. Use the RequestHandler interface for a more comfort.
///
/// Internally, both are the same, but we need the Handler and Observer
/// interfaces for handling the different ways the results are returned.
//******************************************************************************

#pragma once

#pragma region Includes
#include "Observer.h"
#include "ServerStreamContext.h"
#include "Task.h"
#include "SVProtoBuf/Envelope.h"
#pragma endregion Includes

namespace SvAuth
{
class SessionContext;
}

namespace SvRpc
{
class RequestHandlerBase
{
public:
	virtual ~RequestHandlerBase() {}
	virtual bool onHandshake(const std::string& token, SvAuth::SessionContext&) = 0;
	virtual void onRequest(const SvAuth::SessionContext&, SvPenv::Envelope&& request, Task<SvPenv::Envelope> handler) = 0;
	virtual void onStream(const SvAuth::SessionContext&, SvPenv::Envelope&& request, Observer<SvPenv::Envelope> observer, ServerStreamContext::Ptr ctx) = 0;
};

} // namespace SvRpc
