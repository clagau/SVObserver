//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file GroupTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Implements the GroupTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "GroupTool.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVConditional.h"
#include "Operators/ParameterTask.h"
#pragma endregion Includes

//This comment is to avoid that the SVConditional include is marked as not required due to forward declaration from a base class

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

	SV_IMPLEMENT_CLASS(GroupTool, SvPb::GroupToolClassId);

	GroupTool::GroupTool(SVObjectClass* POwner, int StringResourceID)
		:SVToolClass(POwner, StringResourceID)
	{
		Initialize();
	}

	GroupTool::~GroupTool()
	{
		m_embeddedList.clear();
	}

	bool GroupTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		m_isCreated = SVToolClass::CreateObject(rCreateStructure);
		m_isCreated &= (nullptr != GetTool());
		m_isCreated &= (nullptr != GetInspection());

		// These values will not be exposed for this Tool.
		constexpr UINT cAttribute{ SvDef::selectableAttributes | SvPb::audittrail };
		m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_Passed.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_Failed.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_Warned.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_ExplicitFailed.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_PassedCount.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_FailedCount.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_WarnedCount.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
		m_drawToolFlag.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

		SvDef::SVObjectTypeInfoStruct info(SvPb::ParameterTaskObjectType, SvPb::ParameterInputObjectType);
		m_pInputTask = dynamic_cast<SvOp::ParameterTask*>(getFirstObject(info));
		if (nullptr == m_pInputTask)
		{
			m_pInputTask = new SvOp::InputParameterTask;
			Add(m_pInputTask);
		}

		info.m_SubType = SvPb::ParameterResultObjectType;
		m_pResultTask = dynamic_cast<SvOp::ParameterTask*>(getFirstObject(info));
		if (nullptr == m_pResultTask)
		{
			m_pResultTask = new SvOp::ResultParameterTask;
			Add(m_pResultTask);
		}

		return m_isCreated;
	}

	int GroupTool::InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject)
	{
		int result = __super::InsertBefore(objectBeforeID, pTaskObject);
		GetInspectionInterface()->SetDefaultInputs();
		return result;
	}

	bool GroupTool::propagateSizeAndPosition()
	{
		bool result = true;
		for (auto pObj : m_TaskObjectVector)
		{
			SVToolClass* pTool = dynamic_cast<SVToolClass*>(pObj);
			if (nullptr != pTool)
			{
				result = pTool->propagateSizeAndPosition() && result;
			}
		}

		return result;
	}

	bool GroupTool::usePropagateSizeAndPosition() const
	{
		for (auto pObj : m_TaskObjectVector)
		{
			SVToolClass* pTool = dynamic_cast<SVToolClass*>(pObj);
			if (nullptr != pTool)
			{
				if (pTool->usePropagateSizeAndPosition())
				{
					return true;
				}
			}
		}
		return false;
	}

	void GroupTool::addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const
	{
		if (nullptr != pImage)
		{
			for (auto* pTask : m_TaskObjectVector)
			{
				SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(pTask);
				if (nullptr != pTool && pTool->isInputImage(pImage->getObjectId()))
				{
					pTool->addOverlays(pImage, rOverlay);
				}
			}
		}
	}

	long  GroupTool::setToolPosition(long ToolPosition)
	{
		long pos(ToolPosition);
		pos = SVToolClass::setToolPosition(pos);
		for (auto pObj : m_TaskObjectVector)
		{

			SVToolClass* pTool = dynamic_cast<SVToolClass*>(pObj);
			if (pTool)
			{
				pos = pTool->setToolPosition(pos);
			}
		}
		return pos;
	}

	bool GroupTool::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool result = __super::ResetObject(pErrorMessages);
		BOOL isClosed = false;
		m_isClosed.GetValue(isClosed);
		if (isClosed)
		{
			auto dependencyList = getInvalidDependenciesList();
			if (0 < dependencyList.size())
			{
				result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GroupDependencies_Wrong, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
		}
		return result;
	}

	SvPb::InspectionCmdResponse GroupTool::getInvalidDependencies() const
	{
		SvPb::InspectionCmdResponse cmdResponse;
		auto* pResponse = cmdResponse.mutable_getinvaliddependenciesresponse();
		auto dependencyList = getInvalidDependenciesList();
		for (const auto& rPair : dependencyList)
		{
			auto* pPair = pResponse->add_invaliddependencies();
			pPair->set_namefrom(rPair.first);
			pPair->set_nameto(rPair.second);
		}
		return cmdResponse;
	}

	void GroupTool::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed/* = false*/, bool firstObject/* = false*/) const
	{
		BOOL isClosed = false;
		m_isClosed.GetValue(isClosed);
		if (isClosed && stopIfClosed && false == firstObject)
		{
			SVObjectClass::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
			if (m_pInputTask)
			{
				m_pInputTask->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
			}
			if (m_pResultTask)
			{
				m_pResultTask->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
			}
			for (auto* pObject : m_embeddedList)
			{
				if (nullptr != pObject)
				{
					pObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
				}
			}
		}
		else
		{
			__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
		}
	}

	void GroupTool::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool firstObject /*= false*/)
	{
		BOOL isClosed = false;
		m_isClosed.GetValue(isClosed);
		if (isClosed && stopIfClosed && false == firstObject)
		{
			SVObjectClass::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
			if (m_pInputTask)
			{
				m_pInputTask->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
			}
			if (m_pResultTask)
			{
				m_pResultTask->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
			}
			for (auto* pObject : m_embeddedList)
			{
				if (nullptr != pObject)
				{
					pObject->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
				}
			}
		}
		else
		{
			__super::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
		}
	}

	uint32_t GroupTool::getFirstClosedParent(uint32_t stopSearchAtObjectId) const
	{
		BOOL isClosed = false;
		m_isClosed.GetValue(isClosed);
		if (isClosed)
		{
			return getObjectId();
		}
		else
		{
			return __super::getFirstClosedParent(stopSearchAtObjectId);
		}
	}


	void GroupTool::Initialize()
	{
		RegisterEmbeddedObject(&m_isClosed, SvPb::IsClosedEId, IDS_OBJECTNAME_ISCLOSED, false, SvOi::SVResetItemTool);
		m_isClosed.SetDefaultValue(BOOL(true), true);			// Default for Failed is TRUE !!!

		m_canResizeToParent = false;
		m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::GroupToolObjectType;
		removeEmbeddedExtents();
	}

	SvDef::StringPairVector GroupTool::getInvalidDependenciesList() const
	{
		SvDef::StringPairVector dependencyList;
		SvOi::ToolDependencyEnum ToolDependency = /*SvOi::ToolDependencyEnum::Client :*/ SvOi::ToolDependencyEnum::ClientAndSupplier;
		SvOi::getToolDependency(std::back_inserter(dependencyList), { getObjectId() }, SvPb::SVToolObjectType, ToolDependency);
		std::string groupStr = GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
		std::string inputStr = groupStr + "." + SvUl::LoadStdString(IDS_CLASSNAME_INPUT_PARAMETER_TASK);
		std::string resultStr = groupStr + "." + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_PARAMETER_TASK);
		//remove input and result connections
		auto isPartOf = [](const SvDef::StringPair& rPairValue, const std::string& rString) -> bool { return std::string::npos != rPairValue.first.find(rString) || std::string::npos != rPairValue.second.find(rString); };
		dependencyList.erase(std::remove_if(dependencyList.begin(), dependencyList.end(),
			[inputStr, resultStr, isPartOf](const SvDef::StringPair& rEntry) -> bool { return isPartOf(rEntry, inputStr) || isPartOf(rEntry, resultStr); }), dependencyList.end());
		//remove connection direct with groupTool
		dependencyList.erase(std::remove_if(dependencyList.begin(), dependencyList.end(),
			[groupStr](const SvDef::StringPair& rEntry) -> bool { return rEntry.first == groupStr || rEntry.second == groupStr; }), dependencyList.end());
		return dependencyList;
	}
} //namespace SvTo
