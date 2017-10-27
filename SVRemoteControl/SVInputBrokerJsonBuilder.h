//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputBrokerJsonBuilder
//* .File Name       : $Workfile:   SVInputBrokerJsonBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:38:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <string>
#include "jsonlib/include/json.h"
#include "SVMatchStruct.h"
#pragma endregion Includes


class SVInputBrokerJsonBuilder
{
public:
	static std::string BuildQueryListNamesCommand();
	static std::string BuildQueryListItemsCommand(const std::wstring& p_ListName);
	static std::string BuildAddDataItemsCommand(const std::wstring& p_ListName, const SVMatchList& p_List);
	static std::string BuildClearDataItemsCommand(const std::wstring& p_ListName);
};

