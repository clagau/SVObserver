//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file CommandInternalHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "CommandInternalHelper.h"
#include "ObjectSelectorFilter.h"
#include "Definitions\ObjectDefines.h"
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes


namespace SvCmd
{
namespace AllowedFunctionHelper
{
bool hasAttributes(const SvOi::IObjectClass* pObject)
{
	return (0 != ((SvDef::selectableAttributes | SvPb::archivableImage | SvPb::taskObject) & pObject->ObjectAttributesAllowed())) ? true : false;
}

// cppcheck-suppress unusedFunction; used in line 117
bool hasExtension(const SvOi::IObjectClass* pObject, bool&)
{
	bool result = false;
	if (nullptr != pObject)
	{
		result = pObject->allowExtensionCopy();
	}
	return result;
}

class IsValidObject
{
public:
	explicit IsValidObject(bool showHidden = false) : m_showHidden(showHidden) {};

	bool operator()(const SvOi::IObjectClass* pObject, bool&) const
	{
		bool Result {false};
		if (nullptr != pObject)
		{
			Result = m_showHidden || hasAttributes(pObject);
		}
		return Result;
	}

private:
	bool m_showHidden;
};

class IsObjectFromPriorTool
{
public:
	explicit IsObjectFromPriorTool(uint32_t taskObjectID)
		: m_TaskObjectID(taskObjectID)
	{
		const SvOi::ITool* pTool = dynamic_cast<const SvOi::ITool*> (SvOi::getObject(m_TaskObjectID));
		if (nullptr != pTool)
		{
			m_ToolPos = pTool->getToolPosition();
		}
	}

	bool operator()(const SvOi::IObjectClass* pObject, bool& bStop) const
	{
		bStop = !isObjectAboveTask(pObject);
		return !bStop && hasAttributes(pObject);
	}

private:
	bool isObjectAboveTask(const SvOi::IObjectClass* pObject) const
	{
		bool bRetVal = true;
		const SvOi::ITool* pOwnerTool = dynamic_cast<const SvOi::ITool*> (pObject->GetAncestorInterface(SvPb::SVToolObjectType));

		if (nullptr != pOwnerTool)
		{
			//Stop when tool position greater or the same m_ToolPos must be valid
			if (0 <= m_ToolPos && m_ToolPos <= pOwnerTool->getToolPosition())
			{
				bRetVal = false;
			}
		}
		return bRetVal;
	}

private:
	uint32_t m_TaskObjectID;
	int m_ToolPos {-1};
};


}

IsAllowedFunc getAllowedFunc(const SvPb::GetAvailableObjectsRequest& rMessage)
{
	switch (rMessage.allowedMode_case())
	{
		case SvPb::GetAvailableObjectsRequest::kIsBeforeToolMethod:
		{
			return AllowedFunctionHelper::IsObjectFromPriorTool(rMessage.isbeforetoolmethod().toolid());
		}
		case SvPb::GetAvailableObjectsRequest::kDefaultPlusHidden:
		{
			return AllowedFunctionHelper::IsValidObject(true);
		}
		case SvPb::GetAvailableObjectsRequest::kHasExtension:
		{
			return AllowedFunctionHelper::hasExtension;
		}
		default:
		{
			return AllowedFunctionHelper::IsValidObject();
		}
	}
}

IsObjectInfoAllowed getObjectSelectorFilterFunc(const SvPb::GetObjectSelectorItemsRequest& rRequest)
{
	switch (rRequest.filter_case())
	{
		case SvPb::GetObjectSelectorItemsRequest::kExcludeSameLineage:
		{
			std::vector<std::string> excludeVector;
			for (auto id : rRequest.excludesamelineage().excludeids())
			{
				SvOi::IObjectClass* pObject = SvOi::getObject(id);
				if (nullptr != pObject)
				{
					excludeVector.emplace_back(std::move(pObject->GetCompleteName()));
				}
			}
			return ExcludeSameLineageSelectorFilter(std::move(excludeVector));
		}

		case SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed:
			return AttributesAllowedFilter();

		case SvPb::GetObjectSelectorItemsRequest::kAttributesSet:
			return AttributesSetFilter();

		default:
			return [](const SvOi::IObjectClass*, unsigned int, int) { return true; };
	}	
}

} //namespace SvCmd