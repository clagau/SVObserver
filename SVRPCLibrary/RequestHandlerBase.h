//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RequestHandlerBae.h
/// All Rights Reserved
//******************************************************************************
/// Implement the RequestHandlerBase interface to receive incoming Envelopes of
/// type request or stream. Use the RequestHandler interface for a more comfort.
///
/// Internally, both are the same, but we need the Handler and Observer
/// interfaces for handling the different ways the results are returned.
//******************************************************************************

#pragma once

#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/Task.h"
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{
class RequestHandlerBase
{
public:
	virtual ~RequestHandlerBase() {}
	virtual void onRequest(Envelope&& request, Task<Envelope> handler) = 0;
	virtual void onStream(Envelope&& request, Observer<Envelope> observer) = 0;
};
}
