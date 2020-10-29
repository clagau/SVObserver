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

		SvDef::SVObjectTypeInfoStruct info(SvPb::ParameterTaskObjectType, SvPb::ParameterInputObjectType);
		auto* pInputs = dynamic_cast<SvOp::ParameterTask*>(getFirstObject(info));
		if (nullptr == pInputs)
		{
			pInputs = new SvOp::InputParameterTask;
			Add(pInputs);
		}

		info.m_SubType = SvPb::ParameterResultObjectType;
		auto* pResults = dynamic_cast<SvOp::ParameterTask*>(getFirstObject(info));
		if (nullptr == pResults)
		{
			pResults = new SvOp::ResultParameterTask;
			Add(pResults);
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



	void GroupTool::Initialize()
	{
		m_canResizeToParent = false;
		m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
		m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::GroupToolObjectType;
		removeEmbeddedExtents();
	}
} //namespace SvTo
