//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSet.cpp
//* .File Name       : $Workfile:   SVToolSet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   07 Jan 2015 17:52:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVToolSet.h"

#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "AnalyzerOperators/Analyzer.h"
#include "Operators/SVConditional.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "Tools/SVTool.h"
#include "SVUtilityLibrary/SVClock.h"
#include "Tools/SVColorTool.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/ObjectNames.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS(SVToolSet, SvPb::ToolSetClassId);

SVToolSet::SVToolSet(SVObjectClass* POwner, int StringResourceID)
	:SVTaskObjectListClass(POwner, StringResourceID)
{
	init();
}

void SVToolSet::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolSetObjectType;

	// Identify our input type needs

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_isObjectValid, SvPb::TaskObjectClassIsObjectValidEId, IDS_OBJECTNAME_ISVALID, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Enabled, SvPb::ToolSetEnabledEId, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedImage(&m_MainImageObject, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1);

	RegisterEmbeddedObject(&m_Passed, SvPb::PassedEId, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Failed, SvPb::FailedEId, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Warned, SvPb::WarnedEId, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExplicitFailed, SvPb::ExplicitFailedEId, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone);


	RegisterEmbeddedObject(&m_PassedCount, SvPb::PassedCountEId, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_FailedCount, SvPb::FailedCountEId, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_WarnedCount, SvPb::WarnedCountEId, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_EnabledCount, SvPb::EnabledCountEId, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ProcessedCount, SvPb::ProcessedCountEId, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_ToolTime, SvPb::ToolTimeEId, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_MinToolsetTime, SvPb::MinToolSetTimeEId, IDS_OBJECTNAME_MINTOOLSETTIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_MaxToolsetTime, SvPb::MaxToolSetTimeEId, IDS_OBJECTNAME_MAXTOOLSETTIME, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_RegressionTestMode, SvPb::RegressionTestModeEId, IDS_OBJECTNAME_REGRESSIONTESTMODE, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_DrawFlag, SvPb::ConditionalToolSetDrawFlagEId, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ResetCountsObject, SvPb::ResetInspectionCountsEId, IDS_OBJECTNAME_RESET_COUNTS, false, SvOi::SVResetItemIP);
	RegisterEmbeddedObject(&m_TriggerCount, SvPb::TriggerCountEId, IDS_OBJECTNAME_TRIGGER_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_PPQIndexAtCompletion, SvPb::PPQIndexEId, IDS_PPQ_INDEX_AT_COMPLETION, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerDelta], SvPb::TriggerDeltaEId, IDS_TRIGGER_DELTA, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerToCompletion], SvPb::TriggerToCompletionEId, IDS_TRIGGER_TO_COMPLETION_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerToStart], SvPb::TriggerToStartEId, IDS_TRIGGER_TO_START_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerToAcquisitionStart], SvPb::TriggerToAcquisitionStartEId, IDS_TRIGGER_TO_ACQUISITION_START_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::AcquisitionTime], SvPb::AcquisitionTimeEId, IDS_ACQUISITION_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerTimeStamp], SvPb::TriggerTimeEId, IDS_TRIGGER_TIMESTAMP, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Width, SvPb::ExtentWidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_Height, SvPb::ExtentHeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_EnableAuxiliaryExtents, SvPb::EnableAuxiliaryExtentsEId, IDS_OBJECTNAME_AUXILIARYEXTENTS, false, SvOi::SVResetItemNone);
	///m_ObjectID is of type double as math tool results are also double which would cause problems with m_InspectedObectID which should only be of one type
	RegisterEmbeddedObject(&m_ObjectID, SvPb::ObjectIDEId, SvDef::c_ObjectID, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ObjectType, SvPb::ObjectTypeEId, SvDef::c_ObjectType, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_TriggerIndex, SvPb::TriggerIndexEId, SvDef::c_TriggerIndex, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_TriggerPerObjectID, SvPb::TriggerPerObjectIDEId, SvDef::c_TriggerPerObjectID, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_InspectedObjectID, SvPb::InspectedObjectIDEId, SvDef::c_InspectedObjectID, false, SvOi::SVResetItemIP);
	
	RegisterEmbeddedObject(&m_ToolComment, SvPb::ToolCommentTypeEId, IDS_OBJECTNAME_TOOLSET_COMMENT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_InspectionName, SvPb::InspectionNameEId, SvDef::c_InspectionName, false, SvOi::SVResetItemIP);

	RegisterEmbeddedObject(&m_MissingImageCountTS, SvPb::MissingImageCountEId, IDS_OBJECTNAME_MISSING_IMAGE_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_NotCompleteCountTS, SvPb::NotCompleteCountEId, IDS_OBJECTNAME_NOT_COMPLETE_COUNT, false, SvOi::SVResetItemNone);

	//Link inspected object ID with incoming object ID as default
	m_InspectedObjectID.setIndirectValue(SVObjectReference {&m_ObjectID});
	//Display them as integers
	m_ObjectID.SetOutputFormat(SvVol::OutputFormat_int);
	m_ObjectType.SetOutputFormat(SvVol::OutputFormat_int);
	m_TriggerIndex.SetOutputFormat(SvVol::OutputFormat_int);
	m_TriggerPerObjectID.SetOutputFormat(SvVol::OutputFormat_int);


	// Set Embedded defaults
	m_isObjectValid.SetDefaultValue(BOOL(false), true);
	m_isObjectValid.setSaveValueFlag(false);
	m_Enabled.SetDefaultValue(BOOL(true));
	m_Passed.SetDefaultValue(BOOL(false));			// Default for Passed is FALSE !!!
	m_Passed.setSaveValueFlag(false);
	m_Failed.SetDefaultValue(BOOL(true));			// Default for Failed is TRUE !!!
	m_Failed.setSaveValueFlag(false);
	m_Warned.SetDefaultValue(BOOL(true));			// Default for Warned is TRUE !!!
	m_Warned.setSaveValueFlag(false);
	m_ExplicitFailed.SetDefaultValue(BOOL(false));	// Default for Explicit Failed is FALSE !!!
	m_ExplicitFailed.setSaveValueFlag(false);

	m_PassedCount.SetDefaultValue(0L);
	m_PassedCount.setSaveValueFlag(false);
	m_FailedCount.SetDefaultValue(0L);
	m_FailedCount.setSaveValueFlag(false);
	m_WarnedCount.SetDefaultValue(0L);
	m_WarnedCount.setSaveValueFlag(false);

	m_EnabledCount.SetDefaultValue(0L);
	m_EnabledCount.setSaveValueFlag(false);
	m_ProcessedCount.SetDefaultValue(0L);
	m_ProcessedCount.setSaveValueFlag(false);

	m_ToolTime.SetDefaultValue(0L);
	m_ToolTime.setSaveValueFlag(false);
	m_ToolTime.SetName("Tool Set Time");

	m_bResetMinMaxToolsetTime = true;

	m_MinToolsetTime.SetDefaultValue(0LL);
	m_MinToolsetTime.setSaveValueFlag(false);
	m_MaxToolsetTime.SetDefaultValue(0LL);
	m_MaxToolsetTime.setSaveValueFlag(false);

	m_RegressionTestMode.SetDefaultValue(BOOL(false));
	m_RegressionTestMode.setSaveValueFlag(false);

	m_DrawFlag.SetEnumTypes(IDS_TOOLSETDRAW_ENUMOBJECT_LIST);
	m_DrawFlag.SetDefaultValue(0L); // 0 Should be show 'All Tools'
	m_TriggerCount.SetDefaultValue(0L);
	m_TriggerCount.setSaveValueFlag(false);
	m_PPQIndexAtCompletion.SetDefaultValue(0L);
	m_PPQIndexAtCompletion.setSaveValueFlag(false);
	for (int i = 0; i < ToolSetTimes::MaxCount; ++i)
	{
		m_Times[i].SetDefaultValue(0LL);
		m_Times[i].setSaveValueFlag(false);
	}
	m_Width.SetDefaultValue(0.0);
	m_Height.SetDefaultValue(0.0);

	m_EnableAuxiliaryExtents.SetDefaultValue(BOOL(false));

	m_ObjectID.setDefaultValue(0.0);
	m_ObjectID.setSaveValueFlag(false);
	m_ObjectType.setDefaultValue(0);
	m_ObjectType.setSaveValueFlag(false);
	m_TriggerIndex.setDefaultValue(0);
	m_TriggerIndex.setSaveValueFlag(false);
	m_TriggerPerObjectID.setDefaultValue(0);
	m_TriggerPerObjectID.setSaveValueFlag(false);
	/// Set the default value so that the linked variant type is set to double!
	m_InspectedObjectID.setDefaultValue(0.0);
	

	// Set local defaults
	m_StartTime = 0.0;
	m_EndTime = 0.0;
	m_AverageTime = 0.0;
	m_ProcessTime = 0.0;
	m_SetNumber = 0;
	m_isCreated = false;

	SvOp::SVConditional* pConditional = new SvOp::SVConditional(this);
	AddFriend(pConditional);

	// Identify our input type needs
	m_inputConditionBool.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::ConditionalResultEId);
	registerInputObject(&m_inputConditionBool, _T("ToolSetConditionalValue"), SvPb::ToolSetConditionalInputEId);
	m_inputConditionBool.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_ResultList.SetToolSet(this);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVToolSet
// -----------------------------------------------------------------------------
// .Description : Standard destructor of class SVToolSet
////////////////////////////////////////////////////////////////////////////////
SVToolSet::~SVToolSet()
{
	Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
bool SVToolSet::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVTaskObjectListClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_isObjectValid.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
	m_Enabled.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_Passed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Failed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Warned.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ExplicitFailed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_PassedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_FailedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_WarnedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_EnabledCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ProcessedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_RegressionTestMode.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_DrawFlag.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ToolTime.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ResetCountsObject.SetObjectAttributesAllowed(SvPb::remotelySetable | SvPb::setableOnline | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
	m_TriggerCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_PPQIndexAtCompletion.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	for (int i = 0; i < ToolSetTimes::MaxCount; ++i)
	{
		m_Times[i].SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	}
	m_MinToolsetTime.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_MaxToolsetTime.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Width.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Height.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ObjectID.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ObjectType.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_TriggerIndex.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_TriggerPerObjectID.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_InspectedObjectID.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_EnableAuxiliaryExtents.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_MainImageObject.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	// Tool Comment attributes...
	m_ToolComment.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ToolComment.SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);	// We do not want this to show up in the results picker.

	//This is only required to be able to read old configurations with auxiliary extents set in the old format
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if (nullptr != pInspection)
	{
		m_EnableAuxiliaryExtents.SetValue(BOOL(pInspection->getInitialAuxiliaryExtents()));
	}


	SVObjectClass* p_Parent = GetParent();
	if (nullptr != p_Parent)
	{
		std::string inspectionName = p_Parent->GetName();
		m_InspectionName.SetValue(inspectionName);
	}

	m_isCreated = bOk;

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// Destroy Operator
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// .Title       : Destroy All Array Member operator of class SVToolSet
// -----------------------------------------------------------------------------
// .Description : Destroys all array members
////////////////////////////////////////////////////////////////////////////////
void SVToolSet::Destroy()
{
	// Delete our friends
	DestroyFriends();

	m_isCreated = false;

	DeleteAll();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Check the Enabled State
//              : SVToolSet
// -----------------------------------------------------------------------------
// .Description : Checks the Enabled state of the SVToolSet Object
//              : SVToolClass* argument
////////////////////////////////////////////////////////////////////////////////
bool SVToolSet::IsEnabled() const
{
	BOOL bEnabled(true);
	m_Enabled.GetValue(bEnabled);
	return bEnabled ? true : false;
}

bool SVToolSet::WasEnabled() const
{
	bool bEnabled = IsEnabled();

	if (bEnabled)
	{
		bEnabled = getConditionalResult();
	}

	return bEnabled;
}

void SVToolSet::updateToolPosition()
{
	for (long i(0), position(1); i < GetSize(); i++)
	{
		//! For object values use one based index
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetAt(i));
		if (nullptr != pTool)
		{
			position = pTool->setToolPosition(position);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Called by CreateObject
////////////////////////////////////////////////////////////////////////////////
void SVToolSet::SetDefaultInputs()
{
	// Connects all not valid ( Invalid ) input objects to default...

	// Try to get all inputs and outputs...
	connectAllInputs();
	m_ResultList.Refresh(this);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Result List member function of class
//              : SVToolSet
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVResultList object
////////////////////////////////////////////////////////////////////////////////
SVResultList* SVToolSet::GetResultList()
{
	return &m_ResultList;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Image member function of class
//              : SVToolSet
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVImageClass object
////////////////////////////////////////////////////////////////////////////////
SvIe::SVImageClass* SVToolSet::getCurrentImage()
{
	return &m_MainImageObject;
}

SvOi::ISVImage* SVToolSet::getCurrentImageInterface()
{
	return &m_MainImageObject;
}

bool SVToolSet::getConditionalResult(bool bRunMode /*= false*/) const
{
	BOOL Value(false);

	SvVol::SVBoolValueObjectClass* pBoolObject = m_inputConditionBool.getInput<SvVol::SVBoolValueObjectClass>(bRunMode);
	if (nullptr != pBoolObject)
	{
		pBoolObject->GetValue(Value);
	}
	return Value ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetDrawFlagObject
// -----------------------------------------------------------------------------
// .Description : Returns pointer to enumeration value object which is used as 
//				: conditional tool set draw flag.
////////////////////////////////////////////////////////////////////////////////
SvVol::SVEnumerateValueObjectClass* SVToolSet::GetDrawFlagObject()
{
	return &m_DrawFlag;
}


long SVToolSet::getTriggerCount() const
{
	long count = 0;
	m_TriggerCount.GetValue(count);
	return count;
}

SvPb::OverlayDesc SVToolSet::getOverlayStruct(const SvOi::ISVImage& rImage) const
{
	SvPb::OverlayDesc overlayDesc;
	auto* pImage = dynamic_cast<const SvIe::SVImageClass*>(&rImage);
	if (nullptr != pImage)
	{
		for (auto* pTask : m_TaskObjectVector)
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(pTask);
			if (nullptr != pTool && pTool->isInputImage(pImage->getObjectId()))
			{
				pTool->addOverlays(pImage, overlayDesc);
			}
		}
	}
	return overlayDesc;
}

#pragma region virtual method (IToolSet)
bool SVToolSet::IsToolPreviousToSelected(uint32_t toolID) const
{
	bool Result(false);

	std::vector<uint32_t> ToolIds;
	GetToolIds(std::back_inserter(ToolIds));

	for (auto l_Iter = ToolIds.begin(); !Result && l_Iter != ToolIds.end(); ++l_Iter)
	{
		Result = (toolID == (*l_Iter));
	}

	return Result;
}

SvOi::IObjectClass* SVToolSet::getBand0Image() const
{
	SvOi::IObjectClass* pResult(nullptr);

	for (SVTaskObjectPtrVector::const_iterator Iter(m_TaskObjectVector.begin()); Iter != m_TaskObjectVector.end(); ++Iter)
	{
		SvTo::SVColorToolClass* pColorTool = dynamic_cast<SvTo::SVColorToolClass*>(*Iter);
		if (nullptr != pColorTool)
		{
			pResult = dynamic_cast<SvOi::IObjectClass*> (pColorTool->getBand0Image());
			return pResult;
		}
	}
	return pResult;
}
#pragma endregion virtual method (IToolSet)

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun member function of class SVToolSet
// -----------------------------------------------------------------------------
// .Description : runs this toolset
////////////////////////////////////////////////////////////////////////////////
bool SVToolSet::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	m_TriggerCount.SetValue(rRunStatus.m_lTriggerCount > 0L ? rRunStatus.m_lTriggerCount : 0L);

	bool Result = __super::onRun(rRunStatus, pErrorMessages);
	if (Result)
	{
		// Friends were running, validation was successfully
		// Check conditional execution
		if (!getConditionalResult(true))
		{
			rRunStatus.SetDisabledByCondition();
		}
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : To execute the Run process for all Tools in this Tool Set.
//				: This Function returns TRUE, if the Tool Set were running.
//				: Otherwise it returns FALSE, that means: if the Tool Set should
//				: not run, because the Tool Set Condition failed!
////////////////////////////////////////////////////////////////////////////////
bool SVToolSet::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;
	clearRunErrorMessages();

	if (m_ResetCounts)
	{
		// Reset Counters...
		m_PassedCount.SetValue(0);
		m_FailedCount.SetValue(0);
		m_WarnedCount.SetValue(0);
		m_EnabledCount.SetValue(0);
		m_ProcessedCount.SetValue(0);

		m_bResetMinMaxToolsetTime = true;
		m_ResetCounts = false;

		for (SVTaskObjectClass* tool : m_TaskObjectVector)
		{ 
			if (tool != nullptr)
			{
				if (tool->m_ObjectTypeInfo.m_ObjectType == SvPb::SVObjectTypeEnum::SVToolObjectType)
				{
					SvTo::SVToolClass* svtool = static_cast<SvTo::SVToolClass*>(tool);
					svtool->resetCounters();
				}
			}
		}
	}

	double l_Timer = SvUl::GetTimeStamp();
	m_ToolTime.Start();

	// If Conditional is disabled equation.Run() returns always TRUE.
	// Otherwise the return value depends on the Conditional equation result!
	if (IsEnabled())
	{
		++m_SetNumber;

		if (rRunStatus.m_UpdateCounters)
		{
			// Set Processed Count...
			long lCount = 0;
			m_ProcessedCount.GetValue(lCount);
			m_ProcessedCount.SetValue(++lCount);
		}

		// Run yourself...
		bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

		// if disabled or disabled by condition
		// leave in previous state
		if (!rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition())
		{
			if (rRunStatus.m_UpdateCounters)
			{
				// Set Enabled Count...
				long lCount = 0;
				m_EnabledCount.GetValue(lCount);
				m_EnabledCount.SetValue(++lCount);
			}

			SvIe::RunStatus toolRunStatus;
			toolRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
			// Run your children...
			for (int i = 0; i < GetSize(); i++)
			{
				if (GetAt(i))
				{
					toolRunStatus.ResetRunStateAndToolSetTimes();
					toolRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

					bRetVal = GetAt(i)->Run(toolRunStatus, &m_RunErrorMessages) && bRetVal;

					// Update the Run Status
					rRunStatus.updateState(toolRunStatus, false);
				}
			}
			rRunStatus.m_triggerRecord = std::move(toolRunStatus.m_triggerRecord);
		}
		
		// set our state according to the runStatus
		// rRunStatus.SetValid();

		setPostRunStatus(l_Timer, rRunStatus);
	}

	// if disabled or disabled by condition
	// leave in previous state
	if (!rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition())
	{
		// set our state according to the runStatus
		m_Passed.SetValue(BOOL(rRunStatus.IsPassed()));
		m_Failed.SetValue(BOOL(rRunStatus.IsFailed()));
		m_ExplicitFailed.SetValue(BOOL(rRunStatus.IsFailed()));
		m_Warned.SetValue(BOOL(rRunStatus.IsWarned()));

		if (rRunStatus.m_UpdateCounters)
		{
			// Set Counts...
			long lCount = 0;
			if (rRunStatus.IsPassed())
			{
				m_PassedCount.GetValue(lCount);
				m_PassedCount.SetValue(++lCount);
			}
			lCount = 0;
			if (rRunStatus.IsFailed())
			{
				m_FailedCount.GetValue(lCount);
				m_FailedCount.SetValue(++lCount);
			}
			lCount = 0;
			if (rRunStatus.IsWarned())
			{
				m_WarnedCount.GetValue(lCount);
				m_WarnedCount.SetValue(++lCount);
			}
		}
	}

	setStatus(rRunStatus);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	if (bRetVal && isErrorMessageEmpty())
	{
		m_isObjectValid.SetValue(BOOL(true));
	}
	else
	{
		m_isObjectValid.SetValue(BOOL(false));
	}

	return bRetVal;
}// end Run

bool SVToolSet::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages)
{

	bool result = __super::resetAllObjects(pErrorMessages);
	m_isObjectValid.SetValue(BOOL(result));

	return result;
}

bool SVToolSet::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	BOOL bResetCounter(false);
	if (S_OK == m_ResetCountsObject.GetValue(bResetCounter) && bResetCounter)
	{
		m_ResetCounts = true;
		m_ResetCountsObject.SetValue(false);
	}

	m_inputConditionBool.validateInput();

	m_RegressionTestMode.SetDefaultValue(SVSVIMStateClass::CheckState(SV_STATE_REGRESSION));

	SVImageExtentClass ImageExtent(m_MainImageObject.GetImageExtents());
	double Width(0.0);
	double Height(0.0);
	if (S_OK == ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyWidth, Width) &&
		S_OK == ImageExtent.GetExtentProperty(SvPb::SVExtentPropertyHeight, Height))
	{
		m_Width.SetDefaultValue(Width);
		m_Height.SetDefaultValue(Height);
	}

	updateToolPosition();

	return Result;
}

HRESULT SVToolSet::ResetCounts()
{
	HRESULT Result = S_OK;

	try
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

		//add request to inspection process
		if (nullptr != pInspection && pInspection->AddInputRequest(SVObjectReference{ &m_ResetCountsObject }, _T("true")))
		{
			//add request to inspection process
			if (!pInspection->AddInputRequestMarker())
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_FAIL;
		}
	}
	catch (...)
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVToolSet::onCollectOverlays(SvIe::SVImageClass*, SVExtentMultiLineStructVector& )
{
	// override TaskObjectList implementation
	return S_FALSE;	// no overlays for toolset
}

bool SVToolSet::createAllObjectsFromChild(SVObjectClass& rChildObject)
{
	//MZA: 5. Nov 2014: the method call SetDefaultInputs is missing in the other method
	//do we need this method call SetDefaultInputs here?
	// Set defaults, to ensure that no invalid input info exists...
	SetDefaultInputs();

	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();

	return rChildObject.createAllObjects(createStruct);
}

void SVToolSet::connectChildObject(SVTaskObjectClass& rChildObject)
{
	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();

	rChildObject.ConnectObject(createStruct);
}

bool SVToolSet::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if (m_inputConditionBool.IsConnected() && m_inputConditionBool.GetInputObjectInfo().getObject())
	{
		return true;
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConditionalValue_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
}

void SVToolSet::setPostRunStatus(double timer, SvIe::RunStatus &rRunStatus)
{
	double l_Elapsed = (SvUl::GetTimeStamp() - timer);
	m_EndTime = SvUl::ConvertTo(SvUl::Seconds, l_Elapsed);
	m_AverageTime = (m_AverageTime + m_EndTime) / 2.0;

	if (0 < m_SetNumber)
	{
		rRunStatus.m_ToolSetEndTime = m_EndTime;	// put units in seconds
		rRunStatus.m_ToolSetAvgTime = m_AverageTime;
	}
	else
	{
		rRunStatus.m_ToolSetEndTime = -1.0;
		rRunStatus.m_ToolSetAvgTime = -1.0;
	}

	if (rRunStatus.m_UpdateCounters)
	{
		m_ToolTime.Stop();

		__int64 l_i64Time = -1;
		m_ToolTime.GetValue(l_i64Time);

		__int64 l_i64MinTime = -1;
		m_MinToolsetTime.GetValue(l_i64MinTime);
		if (m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime)
		{
			m_MinToolsetTime.SetValue(l_i64Time);
		}
		else
		{
			m_MinToolsetTime.SetValue(l_i64MinTime);
		}

		__int64 l_i64MaxTime = -1;
		m_MaxToolsetTime.GetValue(l_i64MaxTime);

		if (m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time)
		{
			m_MaxToolsetTime.SetValue(l_i64Time);
		}
		else
		{
			m_MaxToolsetTime.SetValue(l_i64MaxTime);
		}

		m_bResetMinMaxToolsetTime = false;
	}
}
