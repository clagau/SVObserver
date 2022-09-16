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
#include "Definitions\GlobalConst.h"
#include "Definitions\SVObjectTypeInfoStruct.h"
#include "Definitions\ObjectDefines.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IRootObject.h"
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

IsAllowedFunc getAllowedFunc(const SvPb::TreeSearchParameters& rMessage)
{
	switch (rMessage.allowedMode_case())
	{
		case SvPb::TreeSearchParameters::kIsBeforeToolMethod:
		{
			return AllowedFunctionHelper::IsObjectFromPriorTool(rMessage.isbeforetoolmethod().toolid());
		}
		case SvPb::TreeSearchParameters::kDefaultPlusHidden:
		{
			return AllowedFunctionHelper::IsValidObject(true);
		}
		case SvPb::TreeSearchParameters::kHasExtension:
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

SvPb::InspectionCmdResponse getAvailableObjects(uint32_t startingObjectId, const SvDef::SVObjectTypeInfoStruct& typeInfo, bool addHidden, bool stopIfClosed, SvPb::SVObjectTypeEnum desiredFirstObjectTypeForName, bool excludeFirstObjectNameInName, IsAllowedFunc isAllowedFunc)
{
	SvPb::InspectionCmdResponse cmdResponse;
	std::vector<SvOi::IObjectClass*> list;
	SvOi::fillObjectList(std::back_inserter(list), typeInfo, startingObjectId, addHidden, stopIfClosed);

	if (isAllowedFunc)// required, even if it does nothing...
	{
		bool bStop = false;
		for (const auto* pObject : list)
		{
			if (nullptr != pObject)
			{

				if (isAllowedFunc(pObject, bStop))
				{
					std::string name;
					switch (typeInfo.m_ObjectType)
					{
						case SvPb::SVImageObjectType:
						{
							const SvOi::ISVImage* pImage = dynamic_cast<const SvOi::ISVImage*>(pObject);
							if (pImage)
							{
								name = pImage->getDisplayedName();
							}
							else
							{
								name = pObject->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
							}
						}
						break;
						default:
						{
							if (SvPb::SVNotSetObjectType == desiredFirstObjectTypeForName)
							{
								name = pObject->GetName();
							}
							else
							{
								if (excludeFirstObjectNameInName)
								{
									name = pObject->GetObjectNameBeforeObjectType(desiredFirstObjectTypeForName);
								}
								else
								{
									name = pObject->GetObjectNameToObjectType(desiredFirstObjectTypeForName);
								}
							}
						}
						break;
					}
					SvPb::GetAvailableObjectsResponse* pResponse = cmdResponse.mutable_getavailableobjectsresponse();
					if (!name.empty())
					{
						auto* pEntry = pResponse->add_list();
						pEntry->set_objectname(name.c_str());
						pEntry->set_objectid(pObject->getObjectId());
					}
				}
			}
			if (bStop)
			{
				return cmdResponse;
			}
		}
	}
	else
	{
		cmdResponse.set_hresult(E_NOINTERFACE);
	}
	return cmdResponse;
}

std::pair<uint32_t, bool> correctStartId(uint32_t preSearchStartId, uint32_t startingObjectId)
{
	SvOi::IObjectClass* pImportantObject = SvOi::getObject(preSearchStartId);
	bool isStopAtBorder {nullptr != pImportantObject};
	if (isStopAtBorder)
	{
		auto tmpId = pImportantObject->getFirstClosedParent(startingObjectId);
		if (SvDef::InvalidObjectId != tmpId)
		{
			startingObjectId = tmpId;
		}
	}
	return {startingObjectId, isStopAtBorder};
}
} //namespace SvCmd

void SvOi::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IInspectionProcess* pInspection, SvOi::IObjectClass* pStartObject, SvPb::LinkedValueTypeEnum valueType, bool wholeArray /*=false*/, std::vector<SvOi::IObjectClass*> excludeSameLineageList /*= {}*/)
{
	if (nullptr == pInspection)
	{
		assert(false);
		return;
	}

	SvPb::ObjectAttributes attribute {SvPb::ObjectAttributes::noAttributes};
	SvPb::ObjectSelectorType type {SvPb::allValueObjects};
	bool onlyToolset {true};
	switch (valueType)
	{
		case SvPb::LinkedValueTypeEnum::TypeDecimal:
		{
			type = SvPb::allNumberValueObjects;
			attribute = SvPb::selectableForEquation;
			onlyToolset = false;
			break;
		}
		case SvPb::LinkedValueTypeEnum::TypeText:
		{
			type = SvPb::allValueObjects;
			attribute = SvPb::ObjectAttributes::viewable;
			onlyToolset = false;
			break;
		}
		case SvPb::LinkedValueTypeEnum::TypeTable:
		{
			type = SvPb::tableObjects;
			attribute = SvPb::ObjectAttributes::taskObject;
			break;
		}
		case SvPb::LinkedValueTypeEnum::TypeGrayImage:
		{
			type = SvPb::grayImageObjects;
			attribute = SvPb::ObjectAttributes::archivableImage;
			break;
		}
		case SvPb::LinkedValueTypeEnum::TypeColorImage:
		{
			type = SvPb::colorImageObjects;
			attribute = SvPb::ObjectAttributes::archivableImage;
			break;
		}
		case SvPb::LinkedValueTypeEnum::TypeImage:
		{
			type = SvPb::allImageObjects;
			attribute = SvPb::ObjectAttributes::archivableImage;
			break;
		}
		case SvPb::LinkedValueTypeEnum::TypeStates:
		{
			type = SvPb::toolObjects;
			attribute = SvPb::ObjectAttributes::taskObject;
			break;
		}
		default:
			break;
	}

	if (false == onlyToolset)
	{
		if (nullptr == pStartObject)
		{
			SvOi::fillRootChildSelectorList(treeInserter, _T(""), attribute, type);
			pInspection->fillPPQSelectorList(treeInserter, attribute, type);
			pInspection->fillCameraSelectorList(treeInserter, attribute, type);
		}
		else
		{
			SvOi::fillRootChildSelectorList(treeInserter, SvDef::FqnEnvironment, attribute, type);
		}
	}

	if (nullptr == pStartObject)
	{
		SvOi::ITaskObject* pTask = pInspection->GetToolSetInterface();
		pStartObject = dynamic_cast<SvOi::IObjectClass*>(pTask);
		if (nullptr == pStartObject)
		{
			return;
		}
	}

	SvOi::IsObjectAllowedFunc pFunctor = SvCmd::AttributesAllowedFilter();
	if (false == excludeSameLineageList.empty())
	{
		std::vector<std::string> excludeVector;
		for (auto* pExcludeObject : excludeSameLineageList)
		{
			if (nullptr != pExcludeObject)
			{
				excludeVector.emplace_back(std::move(pExcludeObject->GetCompleteName()));
			}
		}
		pFunctor = SvCmd::ExcludeSameLineageSelectorFilter(std::move(excludeVector));
	}
	pStartObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, SvPb::SVToolSetObjectType, type, true, true);
}