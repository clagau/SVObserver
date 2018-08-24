//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file NavigatorElement.h
/// All Rights Reserved 
//*****************************************************************************
//NavigatorElement holds information for one element in the navigator
//******************************************************************************
#pragma once
#include <guiddef.h>

enum class NavElementType
{
	StartGrouping,
	EndGrouping,
	LoopTool,
	Tool,
	SubTool,
	SubLoopTool,
	EndDelimiterToolSet,
	EndDelimiterLoopTool,
	Empty
};
struct NavigatorElement
{
	NavigatorElement(LPCSTR displayName) :m_DisplayName(displayName)
	{
	};

	std::string m_DisplayName;
	std::string m_Comment; //< for non tool element
	GUID m_Guid {GUID_NULL};
	GUID m_OwnerGuid {GUID_NULL};
	NavElementType m_Type {NavElementType::Tool};
	bool m_Collapsed {false};
	bool m_Valid {true};
};

typedef std::shared_ptr<NavigatorElement> PtrNavigatorElement;