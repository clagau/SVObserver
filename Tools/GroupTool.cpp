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
#include "InspectionEngine/RunStatus.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
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

		m_pwf.setPwfObjectAttributesAllowed(SvDef::viewableAndUseable, SvOi::SetAttributeType::OverwriteAttribute);
		m_ExplicitFailed.SetObjectAttributesAllowed(SvDef::viewableAndUseable, SvOi::SetAttributeType::OverwriteAttribute);
		m_PassedCount.SetObjectAttributesAllowed(SvDef::viewableAndUseable, SvOi::SetAttributeType::OverwriteAttribute);
		m_FailedCount.SetObjectAttributesAllowed(SvDef::viewableAndUseable, SvOi::SetAttributeType::OverwriteAttribute);
		m_WarnedCount.SetObjectAttributesAllowed(SvDef::viewableAndUseable, SvOi::SetAttributeType::OverwriteAttribute);
		m_drawToolFlag.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

		setParameterTaskPointer();

		if (nullptr == m_pInputTask)
		{
			m_pInputTask = new SvOp::InputParameterTask;
			Add(m_pInputTask);
		}

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

	void GroupTool::getOverlays(const SvIe::SVImageClass& rImage, SVExtentMultiLineStructVector& rMultiLineArray) const
	{
		SvTo::getOverlays(rImage, m_TaskObjectVector, rMultiLineArray);
	}

	void GroupTool::getOverlays(const SvIe::SVImageClass& rImage, SvPb::OverlayDesc& rOverlay) const
	{
		SvTo::getOverlays(rImage, m_TaskObjectVector, rOverlay);
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

		for (auto* pTask : m_TaskObjectVector)
		{
			if (pTask)
			{
				pTask->SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::AddAttribute);
			}
		}

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

	bool GroupTool::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
	{
		bool bRetVal = __super::Run(rRunStatus, pErrorMessages);

		//bretVal is false if one tool is disabled
		if (/*bRetVal && */ false == rRunStatus.IsDisabled() && false == rRunStatus.IsDisabledByCondition())
		{
			bRetVal = m_pResultTask->calcFormulaPost(rRunStatus, pErrorMessages);
			if (!bRetVal)
			{
				// Something was wrong...
				rRunStatus.SetInvalid();
				setStatus(rRunStatus);
			}
		}

		m_ToolTime.Stop();

		return bRetVal;
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
		if (0 != (ObjectAttributesAllowed() & SvPb::viewable))
		{
			if (isClosed() && stopIfClosed && false == firstObject)
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
	}

	void GroupTool::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool firstObject /*= false*/)
	{
		if (isClosed() && stopIfClosed && false == firstObject)
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
		if (isClosed())
		{
			return getObjectId();
		}

		return __super::getFirstClosedParent(stopSearchAtObjectId);
	}

	bool GroupTool::isValidDependency(const std::pair<std::string, std::string>& rEntry) const
	{
		return false == isClosed() || isValid(rEntry);
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

	bool GroupTool::isClosed() const
	{
		BOOL isClosed = false;
		m_isClosed.GetValue(isClosed);
		return isClosed;
	}

	void GroupTool::movedAndDeleteFriends(SVThreadSafeList<SVTaskObjectClass*>& friendList)
	{
		friendList.RemoveAll();
		for (int i = 0; m_friendList.size() > i; ++i)
		{
			friendList.Add(m_friendList[i]);
		}
		m_friendList.RemoveAll();
	}

	void GroupTool::movedAndDeleteTaskObjects(SvIe::SVTaskObjectPtrVector& taskList)
	{
		taskList = std::move(m_TaskObjectVector);
		m_TaskObjectVector.clear();
	}

	void GroupTool::moveEmbeddedObjects(SVObjectPtrVector& rEmbeddedObjects, SvPb::EmbeddedIdEnum excludeIdForMove)
	{
		for (auto* pEmbeddedObject : m_embeddedList)
		{
			if (nullptr != pEmbeddedObject && excludeIdForMove != pEmbeddedObject->GetEmbeddedID())
			{
				auto iter = std::ranges::find_if(rEmbeddedObjects, [pEmbeddedObject](const auto* pEntry) {return nullptr != pEntry && pEntry->GetEmbeddedID() == pEmbeddedObject->GetEmbeddedID(); });
				if (rEmbeddedObjects.end() != iter)
				{
					pEmbeddedObject->moveObject(**iter);
				}
			}
		}
	}

	void GroupTool::moveObjectToThis(GroupTool& rGroupTool, bool includeTaskObjects)
	{
		DestroyFriends();
		rGroupTool.movedAndDeleteFriends(m_friendList);
		m_pToolConditional = nullptr;
		for (int i = 0; m_friendList.size() > i; ++i)
		{
			if (m_friendList[i])
			{
				m_friendList[i]->SetObjectOwner(this);
				if (0 == i)
				{
					//conditional must be the first friend because it will be blocked in runFriends if tool
					m_pToolConditional = dynamic_cast<SvOp::SVConditional*> (m_friendList[i]);
				}
			}
		}

		if (includeTaskObjects)
		{
			DeleteAll();
			rGroupTool.movedAndDeleteTaskObjects(m_TaskObjectVector);
			for (auto* pObject : m_TaskObjectVector)
			{
				if (pObject)
				{
					pObject->SetObjectOwner(this);
				}
			}
		}
		
		rGroupTool.moveEmbeddedObjects(m_embeddedList, includeTaskObjects ? SvPb::NoEmbeddedId : SvPb::ModuleCommentEId);

		std::string oldName = GetName();
		SetName(rGroupTool.GetName());
		if (nullptr != GetInspection())
		{
			GetInspection()->OnObjectRenamed(*this, oldName);
		}
		
		rGroupTool.moveObject(*this);
		if (GetParent())
		{
			SVObjectLevelCreateStruct cs {*GetParent()};
			cs.m_pInspection = GetInspection();
			cs.m_pTool = this;
			setParameterTaskPointer();
			refreshAllObjects(cs);
			updateValidCheckStrings();
			connectInput(m_conditionBoolInput);
			if (m_pInputTask)
			{
				m_pInputTask->refreshToolPointer();
			}
		}
		else
		{
			Log_Assert(false);
		}
	}

	void GroupTool::fixAndReturnAllGroupInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter)
	{
		if (m_pInputTask)
		{
			m_pInputTask->fixAndAddAllObjects(inserter);
		}
		else
		{
			Log_Assert(false);
		}
	}

	void GroupTool::finishAddingAndMovingModule(GroupTool& rOldTool)
	{
		if (m_pInputTask && rOldTool.m_pInputTask)
		{
			m_pInputTask->setObjectValueFromOtherObject(*rOldTool.m_pInputTask);
			m_pInputTask->moveConnectionToThisObject(*rOldTool.m_pInputTask);
		}
		else
		{
			Log_Assert(false);
		}

		if (m_pResultTask && rOldTool.m_pResultTask)
		{
			m_pResultTask->moveConnectionToThisObject(*rOldTool.m_pResultTask);
		}
		else
		{
			Log_Assert(false);
		}

		finishAddTool();
	}

	void GroupTool::Initialize()
	{
		RegisterEmbeddedObject(&m_isClosed, SvPb::IsClosedEId, IDS_OBJECTNAME_ISCLOSED, false, SvOi::SVResetItemToolAndDependent, true);
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

	void GroupTool::setParameterTaskPointer()
	{
		SvDef::SVObjectTypeInfoStruct info(SvPb::ParameterTaskObjectType, SvPb::ParameterInputObjectType);
		m_pInputTask = dynamic_cast<SvOp::ParameterTask*>(getFirstObject(info));

		info.m_SubType = SvPb::ParameterResultObjectType;
		m_pResultTask = dynamic_cast<SvOp::ResultParameterTask*>(getFirstObject(info));
	}
} //namespace SvTo
