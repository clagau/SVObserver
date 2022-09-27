//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	Tool,
	SubTool,
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
			case NavElementType::Tool:
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
	bool m_canHaveChildren {false}; //< Can it have children (e.g. GroupTool or LoopTool)
	bool m_Collapsed {false};
	bool m_Valid {true};
	int m_indent {cUndefinedIndentationValue};
};

typedef std::shared_ptr<NavigatorElement> PtrNavigatorElement;
