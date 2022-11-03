//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file GroupTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Implements the GroupTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "GroupTool.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVConditional.h"
#include "Operators/ParameterTask.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "InspectionCommands/CommandExternalHelper.h"
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
		:SVToolClass(ToolExtType::None,POwner, StringResourceID)
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

		m_groupStr = GetObjectNameToObjectType(SvPb::SVToolSetObjectType);
		m_inputStr = m_groupStr + "." + SvUl::LoadStdString(IDS_CLASSNAME_INPUT_PARAMETER_TASK);
		m_resultStr = m_groupStr + "." + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_PARAMETER_TASK);

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
		updateValidCheckStrings();
		return result;
	}

	void GroupTool::resetCounters()
	{
		__super::resetCounters();

		for (SVTaskObjectClass* tool : m_TaskObjectVector)
		{
			if (tool != nullptr)
			{
				if (tool->getObjectTypeInfo().m_ObjectType == SvPb::SVObjectTypeEnum::SVToolObjectType)
				{
					SvTo::SVToolClass* svtool = static_cast<SvTo::SVToolClass*>(tool);
					svtool->resetCounters();
				}
			}
		}
		
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

	bool GroupTool::isValidDependency(const std::pair<std::string, std::string>& rEntry) const
	{
		BOOL isClosed = false;
		m_isClosed.GetValue(isClosed);
		return FALSE == isClosed || isValid(rEntry);
	}
	
	std::vector<std::string> GroupTool::getToolAdjustNameList() const
	{
		constexpr std::array<LPCTSTR, 5> cToolAdjustNameList
		{
			_T("Input"),
			_T("Result"),
			_T("Conditional"),
			_T("General"),
			_T("Comment"),
		};
		return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
	}

	void GroupTool::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
	{
		__super::OnObjectRenamed(rRenamedObject, rOldName);
		if (&rRenamedObject == this)
		{
			updateValidCheckStrings();
		}
	}

	int GroupTool::getToolDepth(bool goUpwards) const
	{
		int depth = (false == goUpwards) ? 1 : 0;
		return __super::getToolDepth(goUpwards) + depth;
	}

	void GroupTool::Initialize()
	{
		RegisterEmbeddedObject(&m_isClosed, SvPb::IsClosedEId, IDS_OBJECTNAME_ISCLOSED, false, SvOi::SVResetItemTool, true);
		m_isClosed.SetDefaultValue(BOOL(true), true);			// Default for Failed is TRUE !!!

		m_canResizeToParent = false;
		m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::GroupToolObjectType;
		
	}

	SvDef::StringPairVector GroupTool::getInvalidDependenciesList() const
	{
		SvDef::StringPairVector dependencyList;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getdependencyrequest();
		auto* idSet = pRequest->mutable_idsofobjectsdependedon();

		idSet->Add({getObjectId()});
		pRequest->set_objecttype(SvPb::SVToolSetObjectType);
		pRequest->set_tooldependecytype(SvPb::ToolDependencyEnum::ClientAndSupplier);
		pRequest->set_dependecytype(SvPb::DependecyTypeEnum::Tool);

		HRESULT hr = SvCmd::InspectionCommands(0, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getdependencyresponse())
		{
			dependencyList.clear();
			for (SvPb::DependencyPair dependencyPair : responseCmd.getdependencyresponse().dependencypair())
			{
				SvDef::StringPair supplierClientPair(dependencyPair.supplier().name(), dependencyPair.client().name());
				if (!isValid(supplierClientPair))
				{
					dependencyList.push_back(supplierClientPair);
				}
			}
		}

		return dependencyList;
	}

	bool GroupTool::isValid(const std::pair<std::string, std::string>& rEntry) const
	{
		auto isPartOf = [](const SvDef::StringPair& rPairValue, const std::string& rString) -> bool { return rPairValue.first._Starts_with(rString) || rPairValue.second._Starts_with(rString); };
		auto isBothPartOf = [](const SvDef::StringPair& rPairValue, const std::string& rString) -> bool { return rPairValue.first._Starts_with(rString) && rPairValue.second._Starts_with(rString); };
		auto isPartOfOneInTheList = [](const SvDef::StringPair& rPairValue, const std::vector<std::string>& rNameList) -> bool
		{
			return std::any_of(rNameList.begin(), rNameList.end(), [rPairValue](const std::string& rName) { return rPairValue.first == rName || rPairValue.second == rName; });
		};
		return (isPartOf(rEntry, m_inputStr) || //input connections is always valid
			rEntry.first._Starts_with(m_resultStr) || // group tool results may use only values from tools contained in the group tool, but may be used from outside
			(rEntry.first == m_groupStr || rEntry.second == m_groupStr) || //connection direct with groupTool is valid
			isBothPartOf(rEntry, m_groupStr+".") || //both inside of the groupTool is valid
			isPartOf(rEntry, SvDef::FqnEnvironment) ||
			isPartOfOneInTheList(rEntry, m_embeddedNameList));	//is one of it is an embeddedValue it is valid
	}

	void GroupTool::updateValidCheckStrings()
	{
		m_groupStr = GetObjectNameToObjectType(SvPb::SVToolSetObjectType);
		m_inputStr = m_groupStr + "." + SvUl::LoadStdString(IDS_CLASSNAME_INPUT_PARAMETER_TASK);
		m_resultStr = m_groupStr + "." + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_PARAMETER_TASK);
		m_embeddedNameList.clear();
		std::transform(m_embeddedList.begin(), m_embeddedList.end(), std::back_inserter(m_embeddedNameList), [](const auto* pObject) -> std::string { return pObject->GetObjectNameToObjectType(SvPb::SVToolSetObjectType); });
	}
} //namespace SvTo
