//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file EnvelopeUtil.h
/// All Rights Reserved
//******************************************************************************
/// Utils for wrapping und unwrapping an Envelope (see envelope.proto).
/// The Payload inside the Envelope is stored as `google.protobuf.Any` and
/// will be extracted into the provided type `TPayload`.
//******************************************************************************

#pragma once

#pragma warning ( push, 2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)
namespace SVRPC
{
template <typename TPayload> void wrap_payload(Envelope& envelope, const TPayload& payload)
{
	envelope.set_payload_type(payload.message_case());
	envelope.mutable_payload()->PackFrom(payload);
}
template <typename TPayload> bool unwrap_payload(const Envelope& envelope, TPayload& payload)
{
	if (!envelope.has_payload())
	{
		return false;
	}

	if (!envelope.payload().UnpackTo(&payload))
	{
		return false;
	}

	return true;
}
}
