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
#include "SVProtoBuf/Envelope.h"

namespace SvRpc
{
template <typename TPayload> 
void wrap_payload(SvPenv::Envelope& rEnvelope, const TPayload& rPayload)
{
	rEnvelope.set_payloadtype(rPayload.message_case());
	rEnvelope.mutable_payload()->PackFrom(rPayload);
}

template <typename TPayload>
bool unwrap_payload(const SvPenv::Envelope& rEnvelope, TPayload& rPayload)
{
	if (!rEnvelope.has_payload())
	{
		return false;
	}

	if (!rEnvelope.payload().UnpackTo(&rPayload))
	{
		return false;
	}

	return true;
}

} // namespace SvRpc
