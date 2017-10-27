//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputBrokerJsonBuilder
//* .File Name       : $Workfile:   SVOutputBrokerJsonBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:39:36  $
//******************************************************************************
#pragma once
#pragma region Includes
#include <string>
#pragma endregion Includes

namespace SeidenaderVision
{
	class SVOutputBrokerJsonBuilder
	{
	public:
		static std::string BuildStartCommand(const std::wstring& p_ListName);
		static std::string BuildStopCommand(const std::wstring& p_ListName);
		static std::string BuildQueryListNamesCommand();
		static std::string BuildQueryListItemsCommand(const std::wstring& p_ListName);
	};
}
