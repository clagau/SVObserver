//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTool
//* .File Name       : $Workfile:   SVTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   17 Dec 2014 10:48:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTool.h"
#include "SVObjectLibrary/SVClsids.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS(SVToolClass, SVToolClassGuid);

SVToolClass::SVToolClass(SVObjectClass* POwner, int StringResourceID /*= IDS_CLASSNAME_SVTOOL*/)
	: SVTaskObjectListClass(POwner, StringResourceID)
	, m_pToolConditional(nullptr)
{
	init();
}

void SVToolClass::init()
{
	m_canResizeToParent = false;
	// Indentify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject(&enabled, SVToolEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExplicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_PassedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_FailedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_WarnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_EnabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ProcessedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ToolPosition, SVToolPositionGuid, IDS_OBJECTNAME_TOOL_POSITION, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone);

	/////////////////////////////////////////////////////////////////////////
	// Extents - These are shadows of the extents in the SVImageInfoClass,
	// so that they can be exposed and be used by other objects in the system.
	// They are not updated directly (at this point in time).
	/////////////////////////////////////////////////////////////////////////
	//Special type names for extents
	m_ExtentWidth.SetTypeName(_T("Extent Width"));
	m_ExtentHeight.SetTypeName(_T("Extent Height"));
	m_ExtentLeft.SetTypeName(_T("Extent X"));
	m_ExtentTop.SetTypeName(_T("Extent Y"));
	m_ExtentRight.SetTypeName(_T("Extent X"));
	m_ExtentBottom.SetTypeName(_T("Extent Y"));
	m_svAuxiliarySourceX.SetTypeName(_T("Extent X"));
	m_svAuxiliarySourceY.SetTypeName(_T("Extent Y"));
	m_svAuxiliarySourceAngle.SetTypeName(_T("Extent Angle"));
	m_ExtentHeightScaleFactor.SetTypeName(_T("Extent Height Scale Factor"));
	m_ExtentWidthScaleFactor.SetTypeName(_T("Extent Width Scale Factor"));

	RegisterEmbeddedObject(&m_ExtentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentRight, SVExtentRelativeRightPositionObjectGuid, IDS_OBJECTNAME_EXTENT_RIGHT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExtentBottom, SVExtentRelativeBottomPositionObjectGuid, IDS_OBJECTNAME_EXTENT_BOTTOM, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExtentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentHeightScaleFactor, SVExtentHeightScaleFactorObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHTSF, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentWidthScaleFactor, SVExtentWidthScaleFactorObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTHSF, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&ToolSelectedForOperatorMove, SVToolSelectedForOperatorMoveObjectGuid, IDS_OBJECTNAME_TOOL_SELECTED_FOR_OPERATOR_MOVE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_drawToolFlag, SVConditionalToolDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svUpdateAuxiliaryExtents, SVUpdateAuxiliaryExtentsObjectGuid, IDS_OBJECTNAME_UPDATE_AUXILIARY_EXTENTS_OBJECT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_svAuxiliarySourceX, SVAuxiliarySourceXObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_X, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliarySourceY, SVAuxiliarySourceYObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_Y, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliarySourceAngle, SVAuxiliarySourceAngleObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_ANGLE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliarySourceImageName, SVAuxiliarySourceImageNameObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_IMAGE_NAME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliaryDrawType, SVAuxiliaryDrawTypeObjectGuid, IDS_OBJECTNAME_AUXILIARY_DRAW_TYPE_NAME, false, SvOi::SVResetItemNone);
	// Tool Comment...
	RegisterEmbeddedObject(&m_ToolComment, SVToolCommentTypeObjectGuid, IDS_OBJECTNAME_TOOL_COMMENT, false, SvOi::SVResetItemNone);

	m_svToolExtent.SetTool(this);
	m_svToolExtent.SetTranslation(SvDef::SVExtentTranslationShift);
	m_svToolExtent.SetAlwaysUpdate(false);
	m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyPositionPointX, &m_ExtentLeft);
	m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyPositionPointY, &m_ExtentTop);
	m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyWidth, &m_ExtentWidth);
	m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyHeight, &m_ExtentHeight);
	m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyWidthScaleFactor, &m_ExtentWidthScaleFactor);
	m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyHeightScaleFactor, &m_ExtentHeightScaleFactor);

	//
	// Set Embedded defaults
	enabled.SetDefaultValue(BOOL(true), true);
	m_Passed.SetDefaultValue(BOOL(false), true);			// Default for Passed is FALSE !!!
	m_Passed.setSaveValueFlag(false);
	m_Failed.SetDefaultValue(BOOL(true), true);			// Default for Failed is TRUE !!!
	m_Failed.setSaveValueFlag(false);
	m_Warned.SetDefaultValue(BOOL(true), true);			// Default for Warned is TRUE !!!
	m_Warned.setSaveValueFlag(false);
	m_ExplicitFailed.SetDefaultValue(BOOL(false), true);	// Default for Explicit Failed is FALSE !!!
	m_ExplicitFailed.setSaveValueFlag(false);

	m_PassedCount.SetDefaultValue(0L, true);
	m_PassedCount.setSaveValueFlag(false);
	m_FailedCount.SetDefaultValue(0L, true);
	m_FailedCount.setSaveValueFlag(false);
	m_WarnedCount.SetDefaultValue(0L, true);
	m_WarnedCount.setSaveValueFlag(false);

	m_EnabledCount.SetDefaultValue(0L, true);
	m_EnabledCount.setSaveValueFlag(false);
	m_ProcessedCount.SetDefaultValue(0L, true);
	m_ProcessedCount.setSaveValueFlag(false);

	m_ToolPosition.SetDefaultValue(-1L, true);
	m_ToolPosition.setSaveValueFlag(false);
	m_ToolTime.SetDefaultValue(0LL, true);
	m_ToolTime.setSaveValueFlag(false);

	/////////////////////////////////////////////////////////////////////////
	// Set Default values for Shadowed Extents
	/////////////////////////////////////////////////////////////////////////
	m_ExtentLeft.SetDefaultValue(SvDef::cDefaultWindowToolLeft, true);
	m_ExtentTop.SetDefaultValue(SvDef::cDefaultWindowToolTop, true);
	m_ExtentRight.SetDefaultValue(SvDef::cDefaultWindowToolLeft + SvDef::cDefaultWindowToolWidth, true);
	m_ExtentRight.setSaveValueFlag(false);
	m_ExtentBottom.SetDefaultValue(SvDef::cDefaultWindowToolTop + SvDef::cDefaultWindowToolHeight, true);
	m_ExtentBottom.setSaveValueFlag(false);
	m_ExtentWidth.SetDefaultValue(SvDef::cDefaultWindowToolWidth, true);
	m_ExtentHeight.SetDefaultValue(SvDef::cDefaultWindowToolHeight, true);
	m_ExtentWidthScaleFactor.SetDefaultValue(SvDef::cDefaultWindowToolWidthScaleFactor, true);
	m_ExtentHeightScaleFactor.SetDefaultValue(SvDef::cDefaultWindowToolHeightScaleFactor, true);

	m_drawToolFlag.SetEnumTypes(IDS_TOOLDRAW_ENUMOBJECT_LIST);
	m_drawToolFlag.SetDefaultValue(0L, true);

	m_svUpdateAuxiliaryExtents.SetDefaultValue(BOOL(false), true);

	m_svAuxiliarySourceX.SetDefaultValue(0.0, true);
	m_svAuxiliarySourceY.SetDefaultValue(0.0, true);
	m_svAuxiliarySourceAngle.SetDefaultValue(0.0, true);
	m_svAuxiliarySourceImageName.SetDefaultValue(_T(""), true);
	m_svAuxiliaryDrawType.SetDefaultValue("", true);

	m_pCurrentToolSet = nullptr;

	// Auxiliary Source Image.
	m_AuxSourceImageObjectInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_AuxSourceImageObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_AuxSourceImageObjectInfo, _T("ToolAuxSourceImage"));

	// instantiate the Conditional class
	SVConditionalClass* pCondition = new SVConditionalClass(this);
	AddFriend(pCondition->GetUniqueObjectID());

	// Identify our input type needs
	m_inputConditionBoolObjectInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVBoolValueObjectType, SVConditionalResultObjectGuid);
	m_inputConditionBoolObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_inputConditionBoolObjectInfo, _T("ToolConditionalValue"));

	// 
	addDefaultInputObjects();
}

SVToolClass::~SVToolClass()
{
}

bool SVToolClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVTaskObjectListClass::CreateObject(rCreateStructure);

	if (bOk)
	{
		if (GetInspectionInterface())
		{
			SvOi::IInspectionProcess* iIP = GetInspectionInterface();
			m_pCurrentToolSet = dynamic_cast<SvOi::IToolSet*>(iIP->GetToolSetInterface());
		}
		else
		{
			bOk = false;
		}
	}

	for (size_t j = 0; nullptr == m_pToolConditional && j < m_friendList.size(); j++)
	{
		//conditional must be the first friend because it will be blocked in runFriends if tool
		assert(0 == j);
		m_pToolConditional = dynamic_cast<SVConditionalClass *>(m_friendList[j].getObject());
		setSkipFirstFriendFromRun();
	}

	// Set / Reset Printable Flags
	enabled.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
	m_Passed.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_Failed.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_Warned.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_ExplicitFailed.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);

	m_PassedCount.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_FailedCount.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_WarnedCount.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);

	m_EnabledCount.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_ProcessedCount.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_ToolTime.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);

	m_ExtentLeft.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE | SvDef::SV_EXTENT_OBJECT, SvOi::SetAttributeType::AddAttribute);
	m_ExtentTop.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE | SvDef::SV_EXTENT_OBJECT, SvOi::SetAttributeType::AddAttribute);
	m_ExtentRight.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute);
	m_ExtentBottom.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute);
	m_ExtentWidth.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE | SvDef::SV_EXTENT_OBJECT | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeight.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE | SvDef::SV_EXTENT_OBJECT | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute);

	// Defaults for the Scale Factors should be hidden (but NOT removed at this time, so 
	m_ExtentWidthScaleFactor.SetObjectAttributesAllowed(SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentHeightScaleFactor.SetObjectAttributesAllowed(SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);

	// Auxiliary Tool Source Extent
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);

	m_drawToolFlag.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute);

	// Tool Comment attributes...
	m_ToolComment.setStatic(true);
	m_ToolComment.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute);
	m_ToolComment.SetObjectAttributesAllowed(SvDef::SV_VIEWABLE, SvOi::SetAttributeType::RemoveAttribute);	// We do not want this to show up in the results picker.

	m_isCreated = bOk;

	return bOk;
}

bool SVToolClass::CloseObject()
{
	m_svToolExtent.SetToolImage(nullptr);
	m_svToolExtent.SetSelectedImage(nullptr);

	return SVTaskObjectListClass::CloseObject();
}

bool SVToolClass::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pInObjectInfo)
{
	bool Result(SVTaskObjectListClass::DisconnectObjectInput(pInObjectInfo) && nullptr != pInObjectInfo);

	if (nullptr != pInObjectInfo)
	{
		if (pInObjectInfo->GetInputObjectInfo().getObject() == m_svToolExtent.GetToolImage())
		{
			m_svToolExtent.SetToolImage(nullptr);
		}

		if (pInObjectInfo->GetInputObjectInfo().getObject() == m_svToolExtent.GetSelectedImage())
		{
			m_svToolExtent.SetSelectedImage(nullptr);
		}
	}

	return Result;
}

bool SVToolClass::IsEnabled() const
{
	BOOL bEnabled(true);
	enabled.GetValue(bEnabled);

	return bEnabled ? true : false;
}

bool SVToolClass::WasEnabled() const
{
	bool bEnabled = true;

	if (nullptr != m_pCurrentToolSet)
	{
		bEnabled = m_pCurrentToolSet->WasEnabled();
	}

	if (bEnabled)
	{
		bEnabled = IsEnabled();

		if (bEnabled)
		{
			bEnabled = (false != getConditionalResult());
		}
	}

	return bEnabled;
}

HRESULT SVToolClass::GetDrawInfo(SVExtentMultiLineStruct& rMultiLine)
{
	HRESULT l_Status = S_OK;

	if (m_pCurrentToolSet)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (m_pCurrentToolSet->GetDrawFlagObject());

		if (nullptr != pObject)
		{
			double Value;
			pObject->getValue(Value);
			rMultiLine.m_ToolSetDrawFlag = static_cast<long> (Value);
		}
	}

	m_drawToolFlag.GetValue(rMultiLine.m_ToolDrawFlag);

	BOOL bPassed = true;
	BOOL bFailed = true;
	BOOL bWarned = true;

	m_Passed.GetValue(bPassed);
	m_Failed.GetValue(bFailed);
	m_Warned.GetValue(bWarned);

	rMultiLine.m_Passed = (FALSE != bPassed);
	rMultiLine.m_Failed = (FALSE != bFailed);
	rMultiLine.m_Warned = (FALSE != bWarned);

	return l_Status;
}

void SVToolClass::UpdateAuxiliaryExtents()
{
	if (GetInspectionInterface()->getEnableAuxiliaryExtent())
	{
		BOOL l_bUpdateSourceExtents = false;

		bool l_bForceOffsetReset = GetInspectionInterface()->IsOffsetUpdateForced();

		m_svToolExtent.UpdateOffsetData(l_bForceOffsetReset);

		HRESULT hr = m_svUpdateAuxiliaryExtents.GetValue(l_bUpdateSourceExtents);
		if (S_OK == hr && l_bUpdateSourceExtents)
		{
			SVExtentOffsetStruct l_svOffsetData;
			SVImageClass* pAuxSourceImage = SvOl::getInput<SVImageClass>(m_AuxSourceImageObjectInfo, true);

			hr = m_svToolExtent.GetSelectedOffsetData(l_svOffsetData);
			if (S_OK != hr || !l_svOffsetData.m_bIsLinear)
			{
				hr = m_svToolExtent.UpdateOffsetDataToImage(l_svOffsetData, pAuxSourceImage);
			}

			if (nullptr != pAuxSourceImage)
			{
				m_svAuxiliarySourceImageName.SetValue(pAuxSourceImage->GetCompleteName());
			}

			std::string DrawType = m_svToolExtent.GetAuxiliaryDrawTypeString();
			m_svAuxiliaryDrawType.SetValue(DrawType);

			SVExtentPointStruct pt;
			SVImageClass* pImage = m_svToolExtent.GetToolImage();
			if (nullptr != pImage && nullptr != pAuxSourceImage)
			{
				pImage->TranslateFromOutputSpaceToImage(pAuxSourceImage, pt, pt);
			}
			m_svAuxiliarySourceX.SetValue(pt.m_dPositionX);
			m_svAuxiliarySourceY.SetValue(pt.m_dPositionY);

			m_svAuxiliarySourceAngle.SetValue(l_svOffsetData.m_dRotationAngle);
		}
	}
}

inline  void SVToolClass::UpdateStateAndCounter(SVRunStatusClass& rRunStatus)
{
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

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);
}

bool SVToolClass::Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
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
			retVal = __super::Run(rRunStatus, &m_RunErrorMessages);

			if (retVal)
			{
				rRunStatus.SetValid();
			}
			else
			{
				rRunStatus.SetInvalid();
				SetInvalid();
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

	if (GetInspectionInterface()->getEnableAuxiliaryExtent())
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

bool SVToolClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::onRun(rRunStatus, pErrorMessages);

	if (Result)
	{
		if (SvDef::SV_NO_ATTRIBUTES != (m_ExtentTop.ObjectAttributesAllowed() & SvDef::SV_NO_ATTRIBUTES))
		{
			Result = (S_OK == m_ExtentTop.CopyValue(rRunStatus.m_lResultDataIndex)) && Result;
		}
		if (SvDef::SV_NO_ATTRIBUTES != (m_ExtentLeft.ObjectAttributesAllowed() & SvDef::SV_NO_ATTRIBUTES))
		{
			Result = (S_OK == m_ExtentLeft.CopyValue(rRunStatus.m_lResultDataIndex)) && Result;
		}
		if (SvDef::SV_NO_ATTRIBUTES != (m_ExtentWidth.ObjectAttributesAllowed() & SvDef::SV_NO_ATTRIBUTES))
		{
			Result = (S_OK == m_ExtentWidth.CopyValue(rRunStatus.m_lResultDataIndex)) && Result;
		}
		if (SvDef::SV_NO_ATTRIBUTES != (m_ExtentHeight.ObjectAttributesAllowed() & SvDef::SV_NO_ATTRIBUTES))
		{
			Result = (S_OK == m_ExtentHeight.CopyValue(rRunStatus.m_lResultDataIndex)) && Result;
		}
		if (SvDef::SV_NO_ATTRIBUTES != (m_ExtentWidthScaleFactor.ObjectAttributesAllowed() & SvDef::SV_NO_ATTRIBUTES))
		{
			Result = (S_OK == m_ExtentWidthScaleFactor.CopyValue(rRunStatus.m_lResultDataIndex)) && Result;
		}
		if (SvDef::SV_NO_ATTRIBUTES != (m_ExtentHeightScaleFactor.ObjectAttributesAllowed() & SvDef::SV_NO_ATTRIBUTES))
		{
			Result = (S_OK == m_ExtentHeightScaleFactor.CopyValue(rRunStatus.m_lResultDataIndex)) && Result;
		}

		if (!Result)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5076_EXTENTSNOTCOPIED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}

		// Friends were running, validation was successfully
		// Check conditional execution
		if (!getConditionalResult(true))
		{
			rRunStatus.SetDisabledByCondition();
		}
	}

	return Result;
}

bool SVToolClass::getConditionalResult(bool bRunMode /*= false*/) const
{
	BOOL Value(false);
	SVBoolValueObjectClass* pBoolObject = SvOl::getInput<SVBoolValueObjectClass>(m_inputConditionBoolObjectInfo);
	if (nullptr != pBoolObject)
	{
		pBoolObject->GetValue(Value);
	}

	return Value ? true : false;
}

HRESULT SVToolClass::SetImageExtentProperty(SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject)
{
	return m_svToolExtent.SetExtentObject(p_eProperty, pValueObject);
}

HRESULT SVToolClass::GetRootOffsetData(SVExtentOffsetStruct& p_rsvOffsetData)
{
	return m_svToolExtent.GetRootOffsetData(p_rsvOffsetData);
}

HRESULT SVToolClass::UpdateOffsetData(SVImageClass* p_svToolImage)
{
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (nullptr != pInspection)
	{
		pInspection->ForceOffsetUpdate();
	}

	return m_svToolExtent.UpdateOffsetData(true, p_svToolImage);
}

HRESULT SVToolClass::TranslatePointToSource(SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut)
{
	return m_svToolExtent.TranslatePointToSource(p_svIn, p_rsvOut);
}

HRESULT SVToolClass::EnableAuxiliaryExtents(bool enableExtents)
{
	SvOi::SetAttributeType AllowedAttribute = (enableExtents && m_hasToolExtents) ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svAuxiliarySourceX.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, AllowedAttribute);
	m_svAuxiliarySourceY.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, AllowedAttribute);
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, AllowedAttribute);
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, AllowedAttribute);
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, AllowedAttribute);

	m_svAuxiliarySourceX.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_svAuxiliarySourceY.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute);

	return S_OK;
}

bool SVToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	SvOl::ValidateInput(m_AuxSourceImageObjectInfo);
	SvOl::ValidateInput(m_inputConditionBoolObjectInfo);

	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (nullptr != pInspection)
	{
		SvOi::IToolSet* pToolSet = dynamic_cast<SvOi::IToolSet*>(pInspection->GetToolSetInterface());
		if (nullptr != pToolSet)
		{
			bool bResetCounter = false;
			if (S_OK == pToolSet->getResetCounts(bResetCounter) && bResetCounter)
			{
				// Reset Counter...
				m_PassedCount.SetDefaultValue(0, true);
				m_FailedCount.SetDefaultValue(0, true);
				m_WarnedCount.SetDefaultValue(0, true);
				m_EnabledCount.SetDefaultValue(0, true);
				m_ProcessedCount.SetDefaultValue(0, true);
			}
		}
	}
	///UpdateBottomAndRight is called again when imageExtents are changed by ToolsizeAdjust
	if (Result)
	{
		UpdateBottomAndRight();
	}

	// Auxiliary Extents
	BOOL bValue(false);
	if (GetInspectionInterface()->getEnableAuxiliaryExtent() && m_hasToolExtents)
	{
		m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::AddAttribute);
		m_svUpdateAuxiliaryExtents.GetValue(bValue);
	}
	else
	{
		m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
		m_svUpdateAuxiliaryExtents.SetObjectAttributesSet(SvDef::SV_SELECTABLE_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
	}

	EnableAuxiliaryExtents(bValue ? true : false);

	return Result;
}

void SVToolClass::UpdateBottomAndRight()
{
	SVImageExtentClass l_svExtents;

	if (S_OK == GetImageExtent(l_svExtents))
	{
		SVExtentFigureStruct l_svFigure;

		if (S_OK == l_svExtents.GetFigure(l_svFigure))
		{
			if (SvDef::SV_NO_ATTRIBUTES != m_ExtentRight.ObjectAttributesAllowed())
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

				m_ExtentRight.SetValue(l_lValue);
			}

			if (SvDef::SV_NO_ATTRIBUTES != m_ExtentBottom.ObjectAttributesAllowed())
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionY);

				m_ExtentBottom.SetValue(l_lValue);
			}
		}
	}
}

const SVImageClass* SVToolClass::GetToolImage() const
{
	return m_svToolExtent.GetToolImage();
}

void SVToolClass::SetAlwaysUpdate(bool p_bAlwaysUpdate)
{
	m_svToolExtent.SetAlwaysUpdate(p_bAlwaysUpdate);
}

HRESULT SVToolClass::UpdateOffsetDataToImage(SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage)
{
	return m_svToolExtent.UpdateOffsetDataToImage(p_rsvOffsetData, p_svToolImage);
}

HRESULT SVToolClass::UpdateImageWithExtent()
{
	HRESULT l_Status = S_OK;

	l_Status = m_svToolExtent.UpdateImageWithExtent(SVToolExtentClass::SVToolExtent);

	return l_Status;
}

HRESULT SVToolClass::GetParentExtent(SVImageExtentClass& p_rParent) const
{
	HRESULT l_hr = S_OK;
	m_svToolExtent.GetParentExtent(p_rParent);
	return l_hr;
}

EAutoSize SVToolClass::GetAutoSizeEnabled()
{
	return (EnableSizeAndPosition);
}

bool SVToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	return true;
}

HRESULT SVToolClass::GetImageExtent(SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.GetImageExtent(rImageExtent);

	return l_hrOk;
}

HRESULT SVToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svToolExtent.SetImageExtent(rImageExtent);

	return l_hrOk;
}

void SVToolClass::removeEmbeddedExtents(bool p_DisconnectExtents)
{
	if (p_DisconnectExtents)
	{
		m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyPositionPointX, nullptr);
		m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyPositionPointY, nullptr);
		m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyWidth, nullptr);
		m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyHeight, nullptr);
		m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyWidthScaleFactor, nullptr);
		m_svToolExtent.SetExtentObject(SvDef::SVExtentPropertyHeightScaleFactor, nullptr);
	}

	RemoveEmbeddedObject(&m_ExtentLeft); // Remove it from the Embedded List so it is not scripted
	RemoveEmbeddedObject(&m_ExtentTop);
	RemoveEmbeddedObject(&m_ExtentRight);
	RemoveEmbeddedObject(&m_ExtentBottom);
	RemoveEmbeddedObject(&m_ExtentWidth);
	RemoveEmbeddedObject(&m_ExtentHeight);
	RemoveEmbeddedObject(&m_ExtentWidthScaleFactor);
	RemoveEmbeddedObject(&m_ExtentHeightScaleFactor);
}

HRESULT SVToolClass::GetFilteredImageExtentPropertyList(SVExtentPropertyListType& p_rPropertyList)
{
	HRESULT hr = S_OK;

	p_rPropertyList.clear();
	m_svToolExtent.GetFilteredImageExtentPropertyList(p_rPropertyList);

	// objects - on ResetObject - set toolextent flag
	return hr;
}

HRESULT SVToolClass::GetPropertyInfo(SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo) const
{
	return m_svToolExtent.GetExtentPropertyInfo(p_eProperty, p_rInfo);
}

HRESULT SVToolClass::SetExtentPropertyInfo(SvDef::SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo)
{
	return m_svToolExtent.SetExtentPropertyInfo(p_eProperty, p_rInfo);
}

HRESULT SVToolClass::UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine)
{
	HRESULT l_Status = SVTaskObjectListClass::UpdateOverlayIDs(p_rMultiLine);

	if (S_OK == l_Status)
	{
		if (p_rMultiLine.m_ToolID.empty())
		{
			p_rMultiLine.m_ToolID = GetUniqueObjectID();
		}
	}

	return l_Status;
}

HRESULT SVToolClass::CollectOverlays(SVImageClass *pImage, SVExtentMultiLineStructVector &MultiLineArray)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pImage && isInputImage(pImage->GetUniqueObjectID()))
	{
		l_Status = SVTaskObjectListClass::CollectOverlays(pImage, MultiLineArray);
	}

	return l_Status;
}

// Source Image Functions
HRESULT SVToolClass::GetSourceImages(SVImageClassPtrVector* pImageList) const
{
	HRESULT l_hr = S_OK;

	SVImageClass* pImageParent = nullptr;
	const SVToolClass* pTool = this;
	if (m_svToolExtent.GetToolImage())
	{
		pImageParent = m_svToolExtent.GetToolImage()->GetParentImage();
		if (nullptr != pImageParent)
		{
			pTool = dynamic_cast<SVToolClass*>(pImageParent->GetTool());

			if (nullptr != pTool && pTool != this)
			{
				pTool->GetSourceImages(pImageList);
			}
			pImageList->push_back(pImageParent);
		}
	}
	else
	{
		l_hr = -77001;
	}
	return l_hr;
}

HRESULT SVToolClass::SetAuxSourceImage(SVImageClass* pImage)
{
	HRESULT l_hr = S_FALSE;

	SVImageClassPtrVector svImageList;

	if (S_OK == GetSourceImages(&svImageList))
	{
		SVImageClass* pConnectImage = nullptr;

		long l_lCount = static_cast<long> (svImageList.size());

		if (0 < l_lCount)
		{
			pConnectImage = svImageList[0];

			for (int i = l_lCount - 1; S_OK != l_hr && i > 0; i--)
			{
				if (svImageList[i] == pImage)
				{
					pConnectImage = svImageList[i];

					l_hr = S_OK;
				}
			}
		}

		::KeepPrevError(l_hr, ConnectToObject(&m_AuxSourceImageObjectInfo, pConnectImage));

		m_svToolExtent.SetSelectedImage(SvOl::getInput<SVImageClass>(m_AuxSourceImageObjectInfo));

		SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
		if (nullptr != pInspection)
		{
			pInspection->ForceOffsetUpdate();
		}
		UpdateAuxiliaryExtents();
		dynamic_cast<SVObjectClass*>(GetInspectionInterface())->resetAllObjects();
	}

	return l_hr;
}

HRESULT SVToolClass::IsAuxInputImage(const SvOl::SVInObjectInfoStruct* p_psvInfo) const
{
	HRESULT l_hrOk = S_FALSE;

	if (p_psvInfo == &m_AuxSourceImageObjectInfo)
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

const SVImageInfoClass* SVToolClass::getFirstImageInfo() const
{
	const SVImageInfoClass* pRetVal = nullptr;
	SvDef::SVObjectTypeInfoStruct objectInfo(SvDef::SVImageObjectType);
	SVImageClass* pImage = dynamic_cast<SVImageClass*>(getFirstObject(objectInfo, false));
	if (nullptr != pImage)
	{
		pRetVal = &pImage->GetImageInfo();
	}

	return pRetVal;
}

bool SVToolClass::addEntryToMonitorList(SvOi::ParametersForML &retList, const SVGUID& rEmbeddedId) const
{
	auto* pResultObject = getFirstObject(SvDef::SVObjectTypeInfoStruct(SvDef::SVObjectTypeEnum::SVNotSetObjectType, SvDef::SVObjectSubTypeEnum::SVNotSetSubObjectType, rEmbeddedId));
	if (nullptr != pResultObject)
	{
		if (SvDef::SV_NO_ATTRIBUTES != (pResultObject->ObjectAttributesAllowed()))
		{
			retList.push_back(SvOi::ParameterPairForML(pResultObject->GetCompleteName(), pResultObject->GetUniqueObjectID()));
			return true;
		}
	}
	return false;
}

#pragma region ITool methods
bool SVToolClass::areAuxExtentsAvailable() const
{
	return (nullptr != GetToolImage()) ? GetInspectionInterface()->getEnableAuxiliaryExtent() : false;
}

SvUl::NameGuidList SVToolClass::getAvailableAuxSourceImages() const
{
	SvUl::NameGuidList list;
	SVImageClassPtrVector ImageList;
	HRESULT hr = GetSourceImages(&ImageList);
	if (S_OK == hr)
	{
		for (SVImageClassPtrVector::const_iterator it = ImageList.begin(); it != ImageList.end(); ++it)
		{
			list.push_back(std::make_pair((*it)->getDisplayedName(), (*it)->GetUniqueObjectID()));
		}
	}
	return list;
}

SvUl::NameGuidPair SVToolClass::getAuxSourceImage() const
{
	SvUl::NameGuidPair result;
	SVImageClass* pImage = SvOl::getInput<SVImageClass>(m_AuxSourceImageObjectInfo);
	if (pImage)
	{
		result = std::make_pair(pImage->getDisplayedName(), pImage->GetUniqueObjectID());
	}
	return result;
}

HRESULT SVToolClass::setAuxSourceImage(const SVGUID& rObjectID)
{
	HRESULT hr = E_POINTER;
	SVImageClass* pImage = dynamic_cast<SVImageClass*> (SvOi::getObject(rObjectID));
	if (nullptr != pImage)
	{
		hr = SetAuxSourceImage(pImage);
	}
	return hr;
}

void SVToolClass::SetToolImage(const SVGUID& rObjectID)
{
	SVImageClass* pImage = dynamic_cast<SVImageClass*> (SvOi::getObject(rObjectID));
	m_svToolExtent.SetToolImage(pImage);
}

long SVToolClass::getToolPosition() const
{
	long Result(-1);
	m_ToolPosition.GetValue(Result);
	return Result;
};

HRESULT SVToolClass::getExtentProperty(const SvDef::SVExtentPropertyEnum& rExtentProperty, double& rValue)
{
	HRESULT Result {E_FAIL};
	SVImageExtentClass ImageExtents;
	Result = GetImageExtent(ImageExtents);
	if (S_OK == Result)
	{
		Result = ImageExtents.GetExtentProperty(rExtentProperty, rValue);
	}

	return Result;
};

SvOi::ParametersForML SVToolClass::getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const
{
	return SvOi::ParametersForML();
}
void SVToolClass::finishAddTool()
{
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (nullptr != pInspection)
	{
		pInspection->BuildValueObjectMap();
	}

	// Set default formulas in newly instantiated tool...
	SvStl::MessageContainerVector ErrorMessages;
	SetDefaultFormulas(&ErrorMessages);
	for (SvStl::MessageContainerVector::iterator iter = ErrorMessages.begin(); ErrorMessages.end() != iter; ++iter)
	{
		SvStl::MessageMgrStd message(SvStl::MsgType::Log | SvStl::MsgType::Display);
		message.setMessage(iter->getMessage());
	}
}
#pragma endregion ITool methods

SVStringValueObjectClass* SVToolClass::GetInputImageNames()
{
	return nullptr;
}


bool SVToolClass::SetFirstInputImageName(LPCTSTR FirstName)
{
	SVStringValueObjectClass* lstrVO = GetInputImageNames();

	if (lstrVO)
	{
		lstrVO->SetValue(FirstName);
		return true;
	}
	return false;
}

bool SVToolClass::createAllObjectsFromChild(SVObjectClass& rChildObject)
{
	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = dynamic_cast<SVObjectClass*>(GetInspectionInterface());
	createStruct.m_pTool = this;

	return rChildObject.createAllObjects(createStruct);
}

void SVToolClass::connectChildObject(SVTaskObjectClass& rChildObject)
{
	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = dynamic_cast<SVObjectClass*>(GetInspectionInterface());
	createStruct.m_pTool = this;

	rChildObject.ConnectObject(createStruct);
}

bool SVToolClass::IsAllowedLocation(const SvDef::SVExtentLocationPropertyEnum Location, SvDef::SVExtentDirectionsEnum Direction) const
{
	SVExtentPropertyInfoStruct info;
	m_svToolExtent.GetExtentPropertyInfo(SvDef::SVExtentPropertyHeight, info);
	bool bAllowHeight = !(info.bFormula || info.bSetByReset);

	m_svToolExtent.GetExtentPropertyInfo(SvDef::SVExtentPropertyWidth, info);
	bool bAllowWidth = !(info.bFormula || info.bSetByReset);

	m_svToolExtent.GetExtentPropertyInfo(SvDef::SVExtentPropertyPositionPointX, info);
	bool bAllowMoveX = !(info.bFormula || info.bSetByReset);

	m_svToolExtent.GetExtentPropertyInfo(SvDef::SVExtentPropertyPositionPointY, info);
	bool bAllowMoveY = !(info.bFormula || info.bSetByReset);

	bool ret(true);

	switch (Location)
	{

		case  SvDef::SVExtentLocationPropertyTopLeft:
			if (!bAllowMoveX || !bAllowMoveY || !bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case  SvDef::SVExtentLocationPropertyTopRight:
			if (!bAllowMoveY || !bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;
		case  SvDef::SVExtentLocationPropertyBottomRight:
			if (!bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case  SvDef::SVExtentLocationPropertyBottomLeft:
			if (!bAllowMoveX || !bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case  SvDef::SVExtentLocationPropertyLeft:
			if (!bAllowMoveX || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case  SvDef::SVExtentLocationPropertyRight:
			if (!bAllowWidth)
			{
				ret = false;
			}
			break;

		case  SvDef::SVExtentLocationPropertyTop:
			if (!bAllowMoveY || !bAllowHeight)
			{
				ret = false;;
			}
			break;

		case  SvDef::SVExtentLocationPropertyBottom:
			if (!bAllowHeight)
			{
				ret = false;;
			}
			break;
		case	SvDef::SVExtentLocationPropertyCenter:

			if (Direction == SvDef::SVExtentDirectionBoth && (!bAllowMoveX || !bAllowMoveY))
			{
				ret = false;
			}
			else if (Direction == SvDef::SVExtentDirectionHorizontal && !bAllowMoveX)
			{
				ret = false;;

			}
			else   if (Direction == SvDef::SVExtentDirectionVertical && !bAllowMoveY)
			{

				ret = false;;
			}
			break;
	}
	return ret;
}

bool SVToolClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
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
