//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file Helper.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "RunReApi.pb.h"
#pragma warning( pop )

namespace RRApi
{
	class Helper
	{
	public:
		static bool wrap(MessageEnvelope& envelope, MessageType type, int messageId, const ::google::protobuf::Message& msg)
		{
			envelope.set_id(messageId);
			envelope.set_type(type);
			return msg.SerializeToString(envelope.mutable_payload());
		}

		static bool unwrap(const MessageEnvelope& envelope, ::google::protobuf::Message& msg)
		{
			return msg.ParseFromString(envelope.payload());
		}

	};

}
