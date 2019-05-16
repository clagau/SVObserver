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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "AnalyzerOperators/SVAnalyzer.h"
#include "Operators/SVConditional.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "Tools/SVTool.h"
#include "SVTimerLibrary/SVClock.h"
#include "Tools/SVColorTool.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS(SVToolSetClass, SVToolSetClassGuid);

SVToolSetClass::SVToolSetClass(SVObjectClass* POwner, int StringResourceID)
	:SVTaskObjectListClass(POwner, StringResourceID)
{
	init();
}

void SVToolSetClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolSetObjectType;

	// Identify our input type needs

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_isObjectValid, SVTaskObjectClassIsObjectValidGuid, IDS_OBJECTNAME_ISVALID, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Enabled, SVToolSetEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_MainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1);

	RegisterEmbeddedObject(&m_Passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExplicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone);


	RegisterEmbeddedObject(&m_PassedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_FailedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_WarnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_EnabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ProcessedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_MinToolsetTime, SVMinToolSetTimeGuid, IDS_OBJECTNAME_MINTOOLSETTIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_MaxToolsetTime, SVMaxToolSetTimeGuid, IDS_OBJECTNAME_MAXTOOLSETTIME, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_RegressionTestMode, SVRegressionTestModeGuid, IDS_OBJECTNAME_REGRESSIONTESTMODE, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_DrawFlag, SVConditionalToolSetDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ResetCounts, SVResetInspectionCountsGuid, IDS_OBJECTNAME_RESET_COUNTS, false, SvOi::SVResetItemIP);
	RegisterEmbeddedObject(&m_TriggerCount, SVTriggerCountGuid, IDS_OBJECTNAME_TRIGGER_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_PPQIndexAtCompletion, SVPPQIndexGuid, IDS_PPQ_INDEX_AT_COMPLETION, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerDelta], SVTriggerDeltaGuid, IDS_TRIGGER_DELTA, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerToCompletion], SVTriggerToCompletionGuid, IDS_TRIGGER_TO_COMPLETION_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerToStart], SVTriggerToStartGuid, IDS_TRIGGER_TO_START_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::TriggerToAcquisitionStart], SVTriggerToAcquisitionStartGuid, IDS_TRIGGER_TO_ACQUISITION_START_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Times[ToolSetTimes::AcquisitionTime], SVAcquisitionTimeGuid, IDS_ACQUISITION_TIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Width, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_Height, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_EnableAuxiliaryExtents, EnableAuxiliaryExtentsObjectGuid, IDS_OBJECTNAME_AUXILIARYEXTENTS, false, SvOi::SVResetItemNone);

	m_EnableAuxiliaryExtents.SetObjectAttributesAllowed(SvPb::printable | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

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

	m_PassedCount.SetDefaultValue(0);
	m_PassedCount.setSaveValueFlag(false);
	m_FailedCount.SetDefaultValue(0);
	m_FailedCount.setSaveValueFlag(false);
	m_WarnedCount.SetDefaultValue(0);
	m_WarnedCount.setSaveValueFlag(false);

	m_EnabledCount.SetDefaultValue(0);
	m_EnabledCount.setSaveValueFlag(false);
	m_ProcessedCount.SetDefaultValue(0);
	m_ProcessedCount.setSaveValueFlag(false);

	m_ToolTime.SetDefaultValue(0);
	m_ToolTime.setSaveValueFlag(false);
	m_ToolTime.SetName("Tool Set Time");

	m_bResetMinMaxToolsetTime = true;

	m_MinToolsetTime.SetDefaultValue(0);
	m_MinToolsetTime.setSaveValueFlag(false);
	m_MaxToolsetTime.SetDefaultValue(0);
	m_MaxToolsetTime.setSaveValueFlag(false);

	m_RegressionTestMode.SetDefaultValue(BOOL(false));
	m_RegressionTestMode.setSaveValueFlag(false);

	m_DrawFlag.SetEnumTypes(IDS_TOOLSETDRAW_ENUMOBJECT_LIST);
	m_DrawFlag.SetDefaultValue(0l); // 0 Should be show 'All Tools'
	m_TriggerCount.SetDefaultValue(0);
	m_TriggerCount.setSaveValueFlag(false);
	m_PPQIndexAtCompletion.SetDefaultValue(0);
	m_PPQIndexAtCompletion.setSaveValueFlag(false);
	for (int i = 0; i < ToolSetTimes::MaxCount; ++i)
	{
		m_Times[i].SetDefaultValue(0LL);
		m_Times[i].setSaveValueFlag(false);
	}
	m_Width.SetDefaultValue(0.0);
	m_Height.SetDefaultValue(0.0);

	m_EnableAuxiliaryExtents.SetDefaultValue(BOOL(false));

	// Set local defaults
	m_StartTime = 0.0;
	m_EndTime = 0.0;
	m_AverageTime = 0.0;
	m_ProcessTime = 0.0;
	m_SetNumber = 0;
	m_isCreated = false;

	SvOp::SVConditionalClass* pConditional = new SvOp::SVConditionalClass(this);
	AddFriend(pConditional->GetUniqueObjectID());

	// Identify our input type needs
	m_inputConditionBoolObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SVConditionalResultObjectGuid);
	m_inputConditionBoolObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_inputConditionBoolObjectInfo, _T("ToolSetConditionalValue"));

	// Set default inputs and outputs
	// Note:: Call the Derived Class (this) here
	addDefaultInputObjects();
	m_ResultList.SetToolSet(this);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor of class SVToolSetClass
////////////////////////////////////////////////////////////////////////////////
SVToolSetClass::~SVToolSetClass()
{
	Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVTaskObjectListClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_isObjectValid.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
	m_Enabled.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_Passed.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_Failed.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_Warned.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_ExplicitFailed.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_PassedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_FailedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_WarnedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_EnabledCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_ProcessedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_RegressionTestMode.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_DrawFlag.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ToolTime.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_ResetCounts.SetObjectAttributesAllowed(SvPb::remotelySetable | SvPb::setableOnline | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
	m_TriggerCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_PPQIndexAtCompletion.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	for (int i = 0; i < ToolSetTimes::MaxCount; ++i)
	{
		m_Times[i].SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	}
	m_MinToolsetTime.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_MaxToolsetTime.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_Width.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_Height.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);

	m_MainImageObject.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	//This is only required to be able to read old configurations with auxiliary extents set in the old format
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if (nullptr != pInspection)
	{
		m_EnableAuxiliaryExtents.SetValue(BOOL(pInspection->getInitialAuxiliaryExtents()));
	}

	m_isCreated = bOk;

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// Destroy Operator
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// .Title       : Destroy All Array Member operator of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Destroys all array members
////////////////////////////////////////////////////////////////////////////////
void SVToolSetClass::Destroy()
{
	// Delete our friends
	DestroyFriends();

	m_isCreated = false;

	DeleteAll();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Check the Enabled State
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Checks the Enabled state of the SVToolSetClass Object
//              : SVToolClass* argument
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::IsEnabled() const
{
	BOOL bEnabled(true);
	m_Enabled.GetValue(bEnabled);
	return bEnabled ? true : false;
}

bool SVToolSetClass::WasEnabled() const
{
	bool bEnabled = IsEnabled();

	if (bEnabled)
	{
		bEnabled = getConditionalResult();
	}

	return bEnabled;
}

void SVToolSetClass::updateToolPosition()
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
void SVToolSetClass::SetDefaultInputs()
{
	// Connects all not valid ( Invalid ) input objects to default...

	// Try to get all inputs and outputs...
	GetAllInputObjects();
	ConnectAllInputs();
	m_ResultList.Refresh(this);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Result List member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVResultListClass object
////////////////////////////////////////////////////////////////////////////////
SVResultListClass* SVToolSetClass::GetResultList()
{
	return &m_ResultList;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Image member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVImageClass object
////////////////////////////////////////////////////////////////////////////////
SvIe::SVImageClass* SVToolSetClass::getCurrentImage()
{
	return &m_MainImageObject;
}

SvOi::ISVImage* SVToolSetClass::getCurrentImageInterface()
{
	return &m_MainImageObject;
}

bool SVToolSetClass::getConditionalResult(bool bRunMode /*= false*/) const
{
	BOOL Value(false);

	SvVol::SVBoolValueObjectClass* pBoolObject = SvOl::getInput<SvVol::SVBoolValueObjectClass>(m_inputConditionBoolObjectInfo, bRunMode);
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
SvVol::SVEnumerateValueObjectClass* SVToolSetClass::GetDrawFlagObject()
{
	return &m_DrawFlag;
}

HRESULT SVToolSetClass::getResetCounts(bool& rResetCounts)  const
{
	BOOL Value(false);
	HRESULT Result = m_ResetCounts.GetValue(Value);
	rResetCounts = Value ? true : false;
	return Result;
}

void SVToolSetClass::goingOffline()
{
	for (SVTaskObjectPtrVector::const_iterator l_Iter = m_TaskObjectVector.begin(); l_Iter != m_TaskObjectVector.end(); ++l_Iter)
	{
		(*l_Iter)->goingOffline();
	}
}

#pragma region virtual method (IToolSet)
bool SVToolSetClass::IsToolPreviousToSelected(const SVGUID& p_rToolID) const
{
	bool Result(false);

	SVGuidVector ToolIds;
	GetToolIds(std::back_inserter(ToolIds));

	for (SVGuidVector::const_iterator l_Iter = ToolIds.begin(); !Result && l_Iter != ToolIds.end(); ++l_Iter)
	{
		Result = (p_rToolID == (*l_Iter));
	}

	return Result;
}

SvOi::IObjectClass* SVToolSetClass::getBand0Image() const
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
// .Title       : onRun member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : runs this toolset
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
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
bool SVToolSetClass::Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;
	BOOL bIsValid = FALSE;
	BOOL bDisabled = FALSE;
	clearRunErrorMessages();

	double l_Timer = SvTl::GetTimeStamp();
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

			SVRunStatusClass toolRunStatus;
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
					UpdateRunStatus(rRunStatus, toolRunStatus);
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

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get State
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

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

bool SVToolSetClass::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages)
{
	bool result = __super::resetAllObjects(pErrorMessages);
	m_isObjectValid.SetValue(BOOL(result));
	return result;
}

bool SVToolSetClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	SvOl::ValidateInput(m_inputConditionBoolObjectInfo);

	BOOL bResetCounter(false);
	if (S_OK == m_ResetCounts.GetValue(bResetCounter) && bResetCounter)
	{
		// Reset Counters...
		m_PassedCount.SetDefaultValue(0);
		m_FailedCount.SetDefaultValue(0);
		m_WarnedCount.SetDefaultValue(0);
		m_EnabledCount.SetDefaultValue(0);
		m_ProcessedCount.SetDefaultValue(0);
		m_RegressionTestMode.SetDefaultValue(SVSVIMStateClass::CheckState(SV_STATE_REGRESSION));

		m_bResetMinMaxToolsetTime = true;
	}
	else
	{
		m_RegressionTestMode.SetDefaultValue(SVSVIMStateClass::CheckState(SV_STATE_REGRESSION));
	}

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

HRESULT SVToolSetClass::ResetCounts()
{
	HRESULT Result = S_OK;

	try
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

		//add request to inspection process
		if (nullptr != pInspection && pInspection->AddInputRequest(&m_ResetCounts, _T("true")))
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

HRESULT SVToolSetClass::ClearResetCounts()
{
	return m_ResetCounts.SetValue(BOOL(false));
}

HRESULT SVToolSetClass::onCollectOverlays(SvIe::SVImageClass* pImage, SVExtentMultiLineStructVector& rMultiLineArray)
{
	// override TaskObjectList implementation
	return S_FALSE;	// no overlays for toolset
}

bool SVToolSetClass::createAllObjectsFromChild(SVObjectClass& rChildObject)
{
	//MZA: 5. Nov 2014: the method call SetDefaultInputs is missing in the other method
	//do we need this method call SetDefaultInputs here?
	// Set defaults, to ensure that no invalid input info exists...
	SetDefaultInputs();

	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();

	return rChildObject.createAllObjects(createStruct);
}

void SVToolSetClass::connectChildObject(SVTaskObjectClass& rChildObject)
{
	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();

	rChildObject.ConnectObject(createStruct);
}

void SVToolSetClass::UpdateRunStatus(SVRunStatusClass& rRunStatus, const SVRunStatusClass& rToolRunStatus) const
{
	if (rToolRunStatus.IsWarned())
	{
		rRunStatus.SetWarned();
	}
	if (rToolRunStatus.IsFailed())
	{
		rRunStatus.SetFailed();
	}
	if (rToolRunStatus.IsPassed())
	{
		rRunStatus.SetPassed();
	}
	if (rToolRunStatus.IsCriticalFailure())
	{
		rRunStatus.SetCriticalFailure();
	}
}

bool SVToolSetClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if (m_inputConditionBoolObjectInfo.IsConnected() && m_inputConditionBoolObjectInfo.GetInputObjectInfo().getObject())
	{
		return true;
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConditionalValue_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
}

void SVToolSetClass::setPostRunStatus(double timer, SVRunStatusClass &rRunStatus)
{
	double l_Elapsed = (SvTl::GetTimeStamp() - timer);
	m_EndTime = SvTl::ConvertTo(SvTl::Seconds, l_Elapsed);
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

		if (m_bResetMinMaxToolsetTime)
		{
			m_bResetMinMaxToolsetTime = false;
		}
	}
}
