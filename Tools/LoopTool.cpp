//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file LoopTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Implements the LoopTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "LoopTool.h"
#include "Definitions/TextDefineSvDef.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVConditional.h"
#include "SVStatusLibrary/RunStatus.h"
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

SV_IMPLEMENT_CLASS(LoopTool, SvPb::LoopToolClassId);

LoopTool::LoopTool(SVObjectClass* POwner, int StringResourceID)
	:SVToolClass(POwner, StringResourceID)
{
	Initialize();
}

LoopTool::~LoopTool()
{
}

void LoopTool::Initialize()
{
	m_canResizeToParent = false;
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::LoopToolObjectType;
	removeEmbeddedExtents();
	BuildEmbeddedObjectList();
}

void LoopTool::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_MaxLoops, SvPb::MaxLoopsEId, IDS_OBJECTNAME_LOOPTOOL_MAXLOOPS, false, SvOi::SVResetItemNone);
	m_MaxLoops.SetDefaultValue(DefaultMaxLoops);
	RegisterEmbeddedObject(&m_LoopCounter, SvPb::LoopCounterEId, IDS_OBJECTNAME_LOOPTOOL_LOOPCOUNTER, false, SvOi::SVResetItemNone);
	m_LoopCounter.SetDefaultValue(0);

	RegisterEmbeddedObject(&m_LinkedLoops, SvPb::LinkedLoopsEId, IDS_OBJECTNAME_LOOPTOOL_LINKEDLOOPS, false, SvOi::SVResetItemNone);
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = VT_I4;
	vtTemp.lVal = DefaultLinkedLoops;
	m_LinkedLoops.SetDefaultValue(vtTemp, true);

	std::string ObjectName = SvUl::LoadStdString(IDS_OBJECTNAME_LOOPTOOL_LINKEDLOOPS);
	ObjectName += SvDef::cLinkName;
	RegisterEmbeddedObject(&m_LinkedLoops.getLinkedName(), SvPb::LinkedLoops_LinkedEId, ObjectName.c_str(), false, SvOi::SVResetItemNone);
	m_LinkedLoops.getLinkedName().SetDefaultValue(_T(""), false);



	RegisterEmbeddedObject(&m_LinkedBreak, SvPb::LoopBreakEId, IDS_OBJECTNAME_LOOPTOOL_BREAK, false, SvOi::SVResetItemNone);
	::VariantInit(&vtTemp);
	vtTemp.vt = VT_I4;
	vtTemp.lVal = DefaultLinkedBreak;
	m_LinkedBreak.SetDefaultValue(vtTemp, true);

	ObjectName = SvUl::LoadStdString(IDS_OBJECTNAME_LOOPTOOL_BREAK);
	ObjectName += SvDef::cLinkName;
	RegisterEmbeddedObject(&m_LinkedBreak.getLinkedName(), SvPb::LoopBreak_LinkedEId, ObjectName.c_str(), false, SvOi::SVResetItemNone);
	m_LinkedBreak.getLinkedName().SetDefaultValue(_T(""), false);



}

bool LoopTool::Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool retVal = true;
	clearRunErrorMessages();
	m_ToolTime.Start();
	
	if (IsEnabled())
	{

		if (rRunStatus.m_UpdateCounters)
		{
			// Set Processed Count...
			long lCount = 0;
			m_ProcessedCount.GetValue(lCount);
			m_ProcessedCount.SetValue(++lCount);
		}

		if (m_pToolConditional)
		{
			m_pToolConditional->Run(rRunStatus, &m_RunErrorMessages);
		}

		if (!m_pToolConditional || getConditionalResult())
		{
			
			//write the toolTime
			m_ToolTime.Stop();

			double Value;
			m_MaxLoops.getValue(Value);
			long MaxLoops = static_cast<long> (Value);

			m_LinkedLoops.getValue(Value);
			long Loops = static_cast<long> (Value);
			if (Loops < MaxLoops)
			{
				MaxLoops = Loops;
			}


			for (int count = 1; count <= MaxLoops; count++)
			{
				m_LoopCounter.SetValue(count);
				retVal = RunAllChildren(rRunStatus, &m_RunErrorMessages);
				if (retVal)
				{
					rRunStatus.SetValid();
				}
				else
				{
					rRunStatus.SetInvalid();
					break;
				}
				m_ToolTime.Stop();
				double value;
				m_LinkedBreak.getValue(value);
				if (static_cast<long> (value) > 0)
				{
					break;
				}

			}
		}
		else
		{
			rRunStatus.SetDisabledByCondition();
		}// end else
	}
	else
	{
		retVal = !(rRunStatus.m_UpdateCounters);

		rRunStatus.SetDisabled();
		// To set disabled color for any part of this tool ( embeddeds, children, etc. )
		SetDisabled();
	}

	UpdateStateAndCounter(rRunStatus);

	SvOi::IInspectionProcess* pInspection{GetInspectionInterface()};

	if (pInspection->getEnableAuxiliaryExtent())
	{
		UpdateAuxiliaryExtents();
	}

	m_ToolTime.Stop();

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}
	return retVal;
}// end Run

int LoopTool::InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject)
{
	int result = __super::InsertBefore(objectBeforeID, pTaskObject);
	GetInspectionInterface()->SetDefaultInputs();
	return result;
}

bool LoopTool::propagateSizeAndPosition()
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

bool LoopTool::usePropagateSizeAndPosition() const
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

void LoopTool::addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const
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

std::vector<std::string> LoopTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 3> cToolAdjustNameList
	{
		_T("Loop Condition"),
		_T("Conditional"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}


long  LoopTool::setToolPosition(long ToolPosition)
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

bool LoopTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
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

	return m_isCreated;
}


bool LoopTool::RunAllChildren(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	clearRunErrorMessages();
	bool childUpdateCounters = rRunStatus.m_UpdateCounters;

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	if (!rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition())
	{
		RunStatus ChildRunStatus;
		ChildRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
		ChildRunStatus.m_UpdateCounters = childUpdateCounters;
		// Run your children...
		for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
		{
			SVTaskObjectClass* pTaskObject = m_TaskObjectVector[i];
			if (nullptr != pTaskObject)
			{
				ChildRunStatus.ResetRunStateAndToolSetTimes();

				
				bool bRet = pTaskObject->Run(ChildRunStatus, &m_RunErrorMessages);
				bRetVal = (bRet || ChildRunStatus.IsDisabled())&& bRetVal;

				rRunStatus.updateState(ChildRunStatus, false);
			}
		}
		rRunStatus.m_triggerRecord = std::move(ChildRunStatus.m_triggerRecord);
	}

	setStatus(rRunStatus);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

} //namespace SvTo
