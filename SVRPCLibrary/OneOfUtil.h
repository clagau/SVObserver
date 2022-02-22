//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#pragma region Includes
#include "EnvelopeUtil.h"
#include "SVProtoBuf/Envelope.h"
#pragma endregion Includes

namespace SvRpc
{
template <typename TPayload, typename TMessage>
class OneOfUtil
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

	bool unwrap(TMessage& rMessage, SvPenv::Envelope&& rEnvelope)
	{
		TPayload Payload;
		if (!unwrap_payload(rEnvelope, Payload))
		{
			return false;
		}
		return unwrap(rMessage, std::move(Payload));
	}

	void wrap(TPayload& rPayload, TMessage&& Message)
	{
		auto reflection = rPayload.GetReflection();
		auto dest_message = reflection->MutableMessage(&rPayload, &m_FieldDescriptor);
		*static_cast<TMessage*>(dest_message) = std::move(Message);
	}

	void wrap(SvPenv::Envelope& rEnvelope, TMessage&& Message)
	{
		TPayload Payload;
		wrap(Payload, std::move(Message));
		wrap_payload(rEnvelope, Payload);
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

} // namespace SvRpc
