//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file NavigatorElement.h
/// All Rights Reserved 
//*****************************************************************************
//NavigatorElement holds information for one element in the navigator
//******************************************************************************
#pragma once
#include "Definitions\ObjectDefines.h"

enum class NavElementType
{
	StartGrouping,
	EndGrouping,
	LoopTool,
	GroupTool,
	Tool,
	SubTool,
	SubLoopTool,
	EndDelimiterToolSet,
	EndDelimiterTool,
	Empty
};

constexpr int cUndefinedIndentationValue = -1;

struct NavigatorElement
{
	explicit NavigatorElement(LPCSTR displayName) :m_DisplayName(displayName)
	{
	};

	bool mayHaveComment()
	{
		switch (m_Type)
		{
			case NavElementType::StartGrouping:
			case NavElementType::EndGrouping:
			case NavElementType::LoopTool:
			case NavElementType::Tool:
			case NavElementType::GroupTool:
			case NavElementType::SubTool:
				return true;
		}

		return false;
	}

	std::string m_DisplayName;
	std::string m_Comment; //< for non tool element
	uint32_t m_navigatorObjectId{ SvDef::InvalidObjectId };
	uint32_t m_OwnerId {SvDef::InvalidObjectId};
	NavElementType m_Type {NavElementType::Tool};
	bool m_Collapsed {false};
	bool m_Valid {true};
	int m_indent {cUndefinedIndentationValue};
};

typedef std::shared_ptr<NavigatorElement> PtrNavigatorElement;
using NavigatorIndexAndElement = std::pair<int, PtrNavigatorElement>;
using NavigatorIndexAndElementVector = std::vector<NavigatorIndexAndElement>;