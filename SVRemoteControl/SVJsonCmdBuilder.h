//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCmdBuilder
//* .File Name       : $Workfile:   SVJsonCmdBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Oct 2014 10:03:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "JsonLib/include/json.h"
#pragma endregion Includes

class SVJsonCmdBuilder
{
public:
	SVJsonCmdBuilder(const std::string & cmdName);
	~SVJsonCmdBuilder();
	SVJsonCmdBuilder & Argument(const std::string & name, const Json::Value & value);

	template<typename T>
	Json::Value MkJson(const T & t)
	{
		return Json::Value(t);
	}
	const Json::Value & Value() const
	{
		return m_value;
	}

private:
	Json::Value m_value;
	std::string m_name;
	static long m_cmdNo;
};
