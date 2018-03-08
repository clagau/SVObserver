//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file OneOfUtil.h
/// All Rights Reserved
//******************************************************************************
/// Helper for finding the getting/setting an entry in a oneof list based on
/// its type by using protobuf's reflection functionality.
///
/// When having a proto as follows:
///     message ApplicationMessages
///     {
///       oneof message
///       {
///         HelloWorldReq HelloWorldReq = 1;
///         HelloWorldRes HelloWorldRes = 2;
///         HelloMoonReq HelloMoonReq = 3;
///         HelloMoonRes HelloMoonRes = 4;
///       }
///     }
///
/// You can use this helper to get/set e.g. the oneof entry `HelloWorldReq` by:
///
///     void foobar(ApplicationMessages& msg)
///     {
///       OneOfUtil<ApplicationMessages, HelloWorldReq> util;
///       const HelloWorldReq& req = util.get(msg);
///       std::cout << req.DebugString() << std::endl;
///       ...
///       HelloWorldReq req2;
///       ...
///       util.set(msg, std::move(req2));
///     }
//******************************************************************************

#pragma once

#include "SVRPCLibrary/EnvelopeUtil.h"
#include "SVProtoBuf/envelope.h"

namespace SVRPC
{
template <typename TPayload, typename TMessage> class OneOfUtil
{
public:
	OneOfUtil() : m_FieldDescriptor(getFieldDescriptor()) {}

	bool unwrap(TMessage& rMessage, TPayload&& payload)
	{
		auto reflection = payload.GetReflection();
		auto msg = reflection->MutableMessage(&payload, &m_FieldDescriptor);
		rMessage = std::move(*static_cast<TMessage*>(msg));
		return true;
	}

	bool unwrap(TMessage& rMessage, Envelope&& envelope)
	{
		TPayload payload;
		if (!unwrap_payload(envelope, payload))
		{
			return false;
		}
		return unwrap(rMessage, std::move(payload));
	}

	void wrap(TPayload& rPayload, TMessage&& message)
	{
		auto reflection = rPayload.GetReflection();
		auto dest_message = reflection->MutableMessage(&rPayload, &m_FieldDescriptor);
		*static_cast<TMessage*>(dest_message) = std::move(message);
	}

	void wrap(Envelope& rEnvelope, TMessage&& message)
	{
		TPayload payload;
		wrap(payload, std::move(message));
		wrap_payload(rEnvelope, payload);
	}

private:
	static const google::protobuf::FieldDescriptor& getFieldDescriptor()
	{
		const auto msg_desc = TMessage::descriptor();
		const auto payload_desc = TPayload::descriptor();
		for (int i = 0; i < payload_desc->oneof_decl_count(); ++i)
		{
			const auto oneof = payload_desc->oneof_decl(i);
			for (int f = 0; f < oneof->field_count(); ++f)
			{
				const auto fdesc = oneof->field(f);
				if (google::protobuf::FieldDescriptor::TYPE_MESSAGE == fdesc->type())
				{
					if (fdesc->message_type() == msg_desc)
					{
						return *fdesc;
					}
				}
			}
		}
		throw std::runtime_error("Provided message is not a oneof field of the given payload");
	}

private:
	const ::google::protobuf::FieldDescriptor& m_FieldDescriptor;
};

} // namespace SVRPC
