//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : JsonDataHelper
//* .File Name       : $Workfile:   JsonDataHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Jan 2015 10:10:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVRemoteControlConstants.h"
#pragma endregion Includes

class JsonDataHelper
{
public:
	static bool IsNotification(const Json::Value& p_Values)
	{
		Json::Value l_value = p_Values[SVRC::cmd::notfctn];
		return !l_value.isNull();
	}

	static bool IsResponse(const Json::Value& p_Values)
	{
		Json::Value l_value = p_Values[SVRC::cmd::response];
		return (l_value.isNull()) ? false : true;
	}

	static bool DoesCommandMatchResponse(const Json::Value& p_Command, const Json::Value& p_Response)
	{
		std::string l_Command;
		std::string l_Response;

		if (p_Command.isObject())
		{
			Json::Value l_JsonValue = p_Command[SVRC::cmd::command];
			if( l_JsonValue.isString() )
			{
				l_Command = l_JsonValue.asString();
			}
		}

		if (p_Response.isObject())
		{
			Json::Value l_JsonValue = p_Response[SVRC::cmd::response];
			if (l_JsonValue.isString())
			{
				l_Response = l_JsonValue.asString();
			}
		}
		return (!(l_Command.empty()) && (l_Command == l_Response));
	}

	static bool DoesCommandMatchResponseWithID(const Json::Value& p_Command, const Json::Value& p_Response)
	{
		std::string l_Command;
		unsigned long l_CommandId = 0;
		std::string l_Response;
		unsigned long l_ResponseId = 0;

		if (p_Command.isObject())
		{
			Json::Value l_JsonValue = p_Command[SVRC::cmd::name];
			if (l_JsonValue.isString())
			{
				l_Command = l_JsonValue.asString();
			}

			Json::Value l_JsonValueId = p_Command[SVRC::cmd::id];
			if (l_JsonValueId.isInt())
			{
				l_CommandId = l_JsonValueId.asInt();
			}
			else if (l_JsonValueId.isUInt())
			{
				l_CommandId = l_JsonValueId.asUInt();
			}
		}
		if (p_Response.isObject())
		{
			Json::Value l_JsonValue = p_Response[SVRC::cmd::name];
			if (l_JsonValue.isString())
			{
				l_Response = l_JsonValue.asString();
			}

			Json::Value l_JsonValueId = p_Response[SVRC::cmd::id];
			if (l_JsonValueId.isInt())
			{
				l_ResponseId = l_JsonValueId.asInt();
			}
			else if (l_JsonValueId.isUInt())
			{
				l_ResponseId = l_JsonValueId.asUInt();
			}
		}
		bool l_Status = (!l_Command.empty() && !l_Response.empty() && l_Command == l_Response) && 
						(0 != l_CommandId && 0 != l_ResponseId  && l_CommandId == l_ResponseId);
		return l_Status;
	}
};
