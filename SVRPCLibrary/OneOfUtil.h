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
#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{
template <typename TPayload, typename TMessage> class OneOfUtil
{
public:
	OneOfUtil() : m_fdesc(getFieldDescriptor()) {}

	bool unwrap(TMessage& message, TPayload&& payload)
	{
		auto reflection = payload.GetReflection();
		auto msg = reflection->MutableMessage(&payload, &m_fdesc);
		message = std::move(*static_cast<TMessage*>(msg));
		return true;
	}

	bool unwrap(TMessage& message, Envelope&& envelope)
	{
		TPayload payload;
		if (!unwrap_payload(envelope, payload))
		{
			return false;
		}
		return unwrap(message, std::move(payload));
	}

	void wrap(TPayload& payload, TMessage&& message)
	{
		auto reflection = payload.GetReflection();
		auto dest_message = reflection->MutableMessage(&payload, &m_fdesc);
		*static_cast<TMessage*>(dest_message) = std::move(message);
	}

	void wrap(Envelope& envelope, TMessage&& message)
	{
		TPayload payload;
		wrap(payload, std::move(message));
		wrap_payload(envelope, payload);
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
				if (fdesc->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
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
	const ::google::protobuf::FieldDescriptor& m_fdesc;
};

} // namespace SVRPC
