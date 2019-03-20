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
#include "Operators/SVConditional.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVToolClass, SVToolClassGuid);

SVToolClass::SVToolClass(SVObjectClass* POwner, int StringResourceID /*= IDS_CLASSNAME_SVTOOL*/)
	: SVTaskObjectListClass(POwner, StringResourceID)
	, m_pToolConditional(nullptr)
	, m_toolExtent{m_imageExtent}
{
	init();
}

void SVToolClass::init()
{
	m_canResizeToParent = false;
	// Indentify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;

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

	m_toolExtent.SetTool(this);
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationShift);
	m_toolExtent.SetAlwaysUpdate(false);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointX, &m_ExtentLeft);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointY, &m_ExtentTop);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidth, &m_ExtentWidth);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeight, &m_ExtentHeight);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthScaleFactor, &m_ExtentWidthScaleFactor);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightScaleFactor, &m_ExtentHeightScaleFactor);

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
	m_AuxSourceImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_AuxSourceImageObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_AuxSourceImageObjectInfo, _T("ToolAuxSourceImage"));

	// instantiate the Conditional class
	SvOp::SVConditionalClass* pCondition = new SvOp::SVConditionalClass(this);
	AddFriend(pCondition->GetUniqueObjectID());

	// Identify our input type needs
	m_inputConditionBoolObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SVConditionalResultObjectGuid);
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
		m_pToolConditional = dynamic_cast<SvOp::SVConditionalClass*> (m_friendList[j].getObject());
		setSkipFirstFriendFromRun();
	}

	// Set / Reset Printable Flags
	enabled.SetObjectAttributesAllowed(SvPb::printable | SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_Passed.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_Failed.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_Warned.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_ExplicitFailed.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);

	m_PassedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_FailedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_WarnedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);

	m_EnabledCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_ProcessedCount.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_ToolTime.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);

	m_ExtentLeft.SetObjectAttributesAllowed(SvPb::printable | SvPb::setableOnline | SvPb::remotelySetable | SvPb::extentObject, SvOi::SetAttributeType::AddAttribute);
	m_ExtentTop.SetObjectAttributesAllowed(SvPb::printable | SvPb::setableOnline | SvPb::remotelySetable | SvPb::extentObject, SvOi::SetAttributeType::AddAttribute);
	m_ExtentRight.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentBottom.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentWidth.SetObjectAttributesAllowed(SvPb::printable | SvPb::remotelySetable | SvPb::extentObject | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeight.SetObjectAttributesAllowed(SvPb::printable | SvPb::remotelySetable | SvPb::extentObject | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute);

	// Defaults for the Scale Factors should be hidden (but NOT removed at this time, so 
	m_ExtentWidthScaleFactor.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentHeightScaleFactor.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);

	// Auxiliary Tool Source Extent
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvPb::printable | SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	m_drawToolFlag.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);

	// Tool Comment attributes...
	m_ToolComment.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ToolComment.SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);	// We do not want this to show up in the results picker.

	m_isCreated = bOk;

	return bOk;
}

bool SVToolClass::CloseObject()
{
	m_toolExtent.SetToolImage(nullptr);
	m_toolExtent.SetSelectedImage(nullptr);

	return SVTaskObjectListClass::CloseObject();
}

bool SVToolClass::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pInObjectInfo)
{
	bool Result(SVTaskObjectListClass::DisconnectObjectInput(pInObjectInfo) && nullptr != pInObjectInfo);

	if (nullptr != pInObjectInfo)
	{
		if (pInObjectInfo->GetInputObjectInfo().getObject() == m_toolExtent.GetToolImage())
		{
			m_toolExtent.SetToolImage(nullptr);
		}

		if (pInObjectInfo->GetInputObjectInfo().getObject() == m_toolExtent.GetSelectedImage())
		{
			m_toolExtent.SetSelectedImage(nullptr);
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

		m_toolExtent.UpdateOffsetData(l_bForceOffsetReset);

		HRESULT hr = m_svUpdateAuxiliaryExtents.GetValue(l_bUpdateSourceExtents);
		if (S_OK == hr && l_bUpdateSourceExtents)
		{
			SVExtentOffsetStruct l_svOffsetData;
			SvIe::SVImageClass* pAuxSourceImage = SvOl::getInput<SvIe::SVImageClass>(m_AuxSourceImageObjectInfo, true);

			hr = m_toolExtent.GetSelectedOffsetData(l_svOffsetData);
			if (S_OK != hr || !l_svOffsetData.m_bIsLinear)
			{
				m_toolExtent.UpdateOffsetDataToImage(l_svOffsetData, pAuxSourceImage);
			}

			if (nullptr != pAuxSourceImage)
			{
				m_svAuxiliarySourceImageName.SetValue(pAuxSourceImage->GetCompleteName());
			}

			std::string DrawType = m_toolExtent.GetAuxiliaryDrawTypeString();
			m_svAuxiliaryDrawType.SetValue(DrawType);

			SVPoint<double> point;
			SvIe::SVImageClass* pImage = m_toolExtent.GetToolImage();
			if (nullptr != pImage && nullptr != pAuxSourceImage)
			{
				pImage->TranslateFromOutputSpaceToImage(pAuxSourceImage, point, point);
			}
			m_svAuxiliarySourceX.SetValue(point.m_x);
			m_svAuxiliarySourceY.SetValue(point.m_y);

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
	SvVol::SVBoolValueObjectClass* pBoolObject = SvOl::getInput<SvVol::SVBoolValueObjectClass>(m_inputConditionBoolObjectInfo);
	if (nullptr != pBoolObject)
	{
		pBoolObject->GetValue(Value);
	}

	return Value ? true : false;
}

void SVToolClass::SetImageExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject)
{
	m_toolExtent.SetExtentObject(p_eProperty, pValueObject);
}

HRESULT SVToolClass::GetRootOffsetData(SVExtentOffsetStruct& p_rsvOffsetData)
{
	return m_toolExtent.GetRootOffsetData(p_rsvOffsetData);
}

HRESULT SVToolClass::UpdateOffsetData(SvIe::SVImageClass* pToolImage)
{
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (nullptr != pInspection)
	{
		pInspection->ForceOffsetUpdate();
	}

	return m_toolExtent.UpdateOffsetData(true, pToolImage);
}

HRESULT SVToolClass::TranslatePointToSource(SVPoint<double> inPoint, SVPoint<double>& rOutPoint)
{
	return m_toolExtent.TranslatePointToSource(inPoint, rOutPoint);
}

void SVToolClass::setAuxiliaryExtents()
{
	BOOL bEnabled{false};
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	
	if (nullptr != pInspection && pInspection->getEnableAuxiliaryExtent())
	{
		m_svUpdateAuxiliaryExtents.GetValue(bEnabled);
	}
	else
	{
		m_svUpdateAuxiliaryExtents.SetObjectAttributesSet(SvDef::selectableAttributes, SvOi::SetAttributeType::RemoveAttribute);
	}
	SvOi::SetAttributeType AllowedAttribute = bEnabled ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;

	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvDef::selectableAttributes, AllowedAttribute);
	m_svAuxiliarySourceX.SetObjectAttributesAllowed(SvDef::selectableAttributes, AllowedAttribute);
	m_svAuxiliarySourceY.SetObjectAttributesAllowed(SvDef::selectableAttributes, AllowedAttribute);
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(SvDef::selectableAttributes, AllowedAttribute);
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed(SvDef::selectableAttributes, AllowedAttribute);
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed(SvDef::selectableAttributes, AllowedAttribute);

	m_svAuxiliarySourceX.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_svAuxiliarySourceY.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
}

bool SVToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

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

	setAuxiliaryExtents();

	SvOl::ValidateInput(m_AuxSourceImageObjectInfo);
	SvOl::ValidateInput(m_inputConditionBoolObjectInfo);

	return Result;
}

void SVToolClass::UpdateBottomAndRight()
{
	//This update call is required for the image extents which can have changed
	updateImageExtent();

	if(GetImageExtent().hasFigure())
	{
		const SVExtentFigureStruct& rFigure = GetImageExtent().GetFigure();
		if (SvPb::noAttributes != m_ExtentRight.ObjectAttributesAllowed())
		{
			m_ExtentRight.SetValue(static_cast<long>(rFigure.m_svBottomRight.m_x));
		}

		if (SvPb::noAttributes != m_ExtentBottom.ObjectAttributesAllowed())
		{
			m_ExtentBottom.SetValue(static_cast<long>(rFigure.m_svBottomRight.m_y));
		}
	}
}

const SvIe::SVImageClass* SVToolClass::GetToolImage() const
{
	return m_toolExtent.GetToolImage();
}

void SVToolClass::SetAlwaysUpdate(bool p_bAlwaysUpdate)
{
	m_toolExtent.SetAlwaysUpdate(p_bAlwaysUpdate);
}

HRESULT SVToolClass::UpdateOffsetDataToImage(SVExtentOffsetStruct& p_rsvOffsetData, SvIe::SVImageClass* pToolImage)
{
	return m_toolExtent.UpdateOffsetDataToImage(p_rsvOffsetData, pToolImage);
}

HRESULT SVToolClass::propagateSizeAndPosition()
{
	if (SvOp::ToolSizeAdjustTask::UseSizeAdjust(this))
	{
		return resetAllObjects();
	}
	return S_OK;
}

bool SVToolClass::usePropagateSizeAndPosition() const
{
	if (SvOp::ToolSizeAdjustTask::UseSizeAdjust(this))
	{
		return true;
	}
	return false;
}

HRESULT SVToolClass::updateImageExtent()
{
	return m_toolExtent.updateImageExtent();
}

HRESULT SVToolClass::UpdateImageWithExtent()
{
	return m_toolExtent.UpdateImageWithExtent(SVToolExtentClass::SVToolExtent);
}

HRESULT SVToolClass::GetParentExtent(SVImageExtentClass& p_rParent) const
{
	HRESULT l_hr = S_OK;
	m_toolExtent.GetParentExtent(p_rParent);
	return l_hr;
}

SvPb::EAutoSize SVToolClass::GetAutoSizeEnabled() const
{
	return (SvPb::EnableSizeAndPosition);
}

HRESULT SVToolClass::updateExtentFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eAction, long dx, long dy)
{
	auto imageExtents = GetImageExtent();
	imageExtents.UpdateFromOutputSpace(eAction, dx, dy);
	HRESULT retVal = SetImageExtent(imageExtents);
	auto* pInspection = GetInspectionInterface();
	if (S_OK == retVal && nullptr != pInspection)
	{
		retVal = pInspection->resetTool(*this);
	}
	else
	{
		retVal = E_FAIL;
	}

	return retVal;
}

HRESULT SVToolClass::setExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double value)
{
	auto imageExtents = GetImageExtent();
	imageExtents.SetExtentProperty(eProperty, value);
	imageExtents.UpdateData();
	HRESULT retVal = SetImageExtent(imageExtents);
	auto* pInspection = GetInspectionInterface();
	if (S_OK == retVal && nullptr != pInspection)
	{
		retVal = pInspection->resetTool(*this);
	}
	else
	{
		retVal = E_FAIL;
	}

	return retVal;
}

HRESULT SVToolClass::setExtentList(const ::google::protobuf::RepeatedPtrField<::SvPb::ExtentParameter> param)
{
	auto imageExtents = GetImageExtent();
	for (auto data : param)
	{
		imageExtents.SetExtentProperty(data.type(), data.value());
	}

	HRESULT retVal = SetImageExtent(imageExtents);
	auto* pInspection = GetInspectionInterface();
	if (S_OK == retVal && nullptr != pInspection)
	{
		retVal = pInspection->resetTool(*this);
	}
	else
	{
		retVal = E_FAIL;
	}

	return retVal;
}

HRESULT SVToolClass::setExtentToParent()
{
	HRESULT retVal = SetImageExtentToParent();
	auto* pInspection = GetInspectionInterface();
	if (S_OK == retVal && nullptr != pInspection)
	{
		retVal = pInspection->resetTool(*this);
	}
	else
	{
		retVal = E_FAIL;
	}

	return retVal;
}

bool SVToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	return true;
}

HRESULT SVToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	return m_toolExtent.SetImageExtent(rImageExtent);
}

void SVToolClass::removeEmbeddedExtents(bool p_DisconnectExtents)
{
	if (p_DisconnectExtents)
	{
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointX, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointY, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidth, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeight, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthScaleFactor, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightScaleFactor, nullptr);
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

HRESULT SVToolClass::GetPropertyInfo(SvPb::SVExtentPropertyEnum p_eProperty, SvIe::SVExtentPropertyInfoStruct& p_rInfo) const
{
	return m_toolExtent.GetExtentPropertyInfo(p_eProperty, p_rInfo);
}

void SVToolClass::getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const
{
	m_toolExtent.getExtentProperties(rExtentProperties);
	rTranslationType = m_toolExtent.GetTranslation();
}

HRESULT SVToolClass::getParentExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const
{
	SVImageExtentClass parentExtent;
	HRESULT retValue = m_toolExtent.GetParentExtent(parentExtent);
	if (S_OK == retValue)
	{
		parentExtent.getExtentProperties(rExtentProperties);
		rTranslationType = parentExtent.GetTranslation();
	}
	return retValue;
}

HRESULT SVToolClass::SetExtentPropertyInfo(SvPb::SVExtentPropertyEnum p_eProperty, const SvIe::SVExtentPropertyInfoStruct& p_rInfo)
{
	return m_toolExtent.SetExtentPropertyInfo(p_eProperty, p_rInfo);
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

HRESULT SVToolClass::CollectOverlays(SvIe::SVImageClass *pImage, SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pImage && isInputImage(pImage->GetUniqueObjectID()))
	{
		l_Status = SVTaskObjectListClass::CollectOverlays(pImage, rMultiLineArray);
	}

	return l_Status;
}

// Source Image Functions
HRESULT SVToolClass::GetSourceImages(SvIe::SVImageClassPtrVector* pImageList) const
{
	HRESULT l_hr = S_OK;

	SvIe::SVImageClass* pImageParent = nullptr;
	const SVToolClass* pTool = this;
	if (m_toolExtent.GetToolImage())
	{
		pImageParent = m_toolExtent.GetToolImage()->GetParentImage();
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

HRESULT SVToolClass::SetAuxSourceImage(SvIe::SVImageClass* pImage)
{
	HRESULT l_hr = S_FALSE;

	SvIe::SVImageClassPtrVector imageList;

	if (S_OK == GetSourceImages(&imageList))
	{
		SvIe::SVImageClass* pConnectImage = nullptr;

		if (0 < imageList.size())
		{
			pConnectImage = imageList[0];

			for (auto* pTmpImage : imageList)
			{
				if (pTmpImage == pImage)
				{
					pConnectImage = pTmpImage;
					l_hr = S_OK;
					break;
				}
			}
		}

		::KeepPrevError(l_hr, ConnectToObject(&m_AuxSourceImageObjectInfo, pConnectImage));

		m_toolExtent.SetSelectedImage(SvOl::getInput<SvIe::SVImageClass>(m_AuxSourceImageObjectInfo));

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
	SvDef::SVObjectTypeInfoStruct objectInfo(SvPb::SVImageObjectType);
	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(getFirstObject(objectInfo, false));
	if (nullptr != pImage)
	{
		pRetVal = &pImage->GetImageInfo();
	}

	return pRetVal;
}

bool SVToolClass::addEntryToMonitorList(SvOi::ParametersForML &retList, const SVGUID& rEmbeddedId) const
{
	auto* pResultObject = getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType, rEmbeddedId));
	if (nullptr != pResultObject)
	{
		if (SvPb::noAttributes != (pResultObject->ObjectAttributesAllowed()))
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
	SvIe::SVImageClassPtrVector ImageList;
	HRESULT hr = GetSourceImages(&ImageList);
	if (S_OK == hr)
	{
		for (const auto pImage : ImageList)
		{
			list.push_back(std::make_pair(pImage->getDisplayedName(), pImage->GetUniqueObjectID()));
		}
	}
	return list;
}

SvUl::NameGuidPair SVToolClass::getAuxSourceImage() const
{
	SvUl::NameGuidPair result;
	SvIe::SVImageClass* pImage = SvOl::getInput<SvIe::SVImageClass>(m_AuxSourceImageObjectInfo);
	if (pImage)
	{
		result = std::make_pair(pImage->getDisplayedName(), pImage->GetUniqueObjectID());
	}
	return result;
}

HRESULT SVToolClass::setAuxSourceImage(const SVGUID& rObjectID)
{
	HRESULT hr = E_POINTER;
	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SvOi::getObject(rObjectID));
	if (nullptr != pImage)
	{
		hr = SetAuxSourceImage(pImage);
	}
	return hr;
}

void SVToolClass::SetToolImage(const SVGUID& rObjectID)
{
	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SvOi::getObject(rObjectID));
	m_toolExtent.SetToolImage(pImage);
}

long SVToolClass::getToolPosition() const
{
	long Result(-1);
	m_ToolPosition.GetValue(Result);
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

SvPb::EAutoSize SVToolClass::getAutoSizeEnabled() const
{
	return GetAutoSizeEnabled();
}
#pragma endregion ITool methods

SvVol::SVStringValueObjectClass* SVToolClass::GetInputImageNames()
{
	return nullptr;
}


bool SVToolClass::SetFirstInputImageName(LPCTSTR FirstName)
{
	SvVol::SVStringValueObjectClass* lstrVO = GetInputImageNames();

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

bool SVToolClass::isAllowedLocation(const SvPb::SVExtentLocationPropertyEnum Location, SvPb::SVExtentDirectionsEnum Direction) const
{
	SvIe::SVExtentPropertyInfoStruct info;
	m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyHeight, info);
	bool bAllowHeight = !(info.bFormula || info.bSetByReset);

	m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyWidth, info);
	bool bAllowWidth = !(info.bFormula || info.bSetByReset);

	m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
	bool bAllowMoveX = !(info.bFormula || info.bSetByReset);

	m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
	bool bAllowMoveY = !(info.bFormula || info.bSetByReset);

	bool ret(true);

	switch (Location)
	{

		case SvPb::SVExtentLocationPropertyTopLeft:
			if (!bAllowMoveX || !bAllowMoveY || !bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case SvPb::SVExtentLocationPropertyTopRight:
			if (!bAllowMoveY || !bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;
		case SvPb::SVExtentLocationPropertyBottomRight:
			if (!bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case SvPb::SVExtentLocationPropertyBottomLeft:
			if (!bAllowMoveX || !bAllowHeight || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case SvPb::SVExtentLocationPropertyLeft:
			if (!bAllowMoveX || !bAllowWidth)
			{
				ret = false;
			}
			break;

		case SvPb::SVExtentLocationPropertyRight:
			if (!bAllowWidth)
			{
				ret = false;
			}
			break;

		case SvPb::SVExtentLocationPropertyTop:
			if (!bAllowMoveY || !bAllowHeight)
			{
				ret = false;;
			}
			break;

		case SvPb::SVExtentLocationPropertyBottom:
			if (!bAllowHeight)
			{
				ret = false;
			}
			break;
		case SvPb::SVExtentLocationPropertyCenter:

			if (Direction == SvPb::SVExtentDirectionBoth && (!bAllowMoveX || !bAllowMoveY))
			{
				ret = false;
			}
			else if (Direction == SvPb::SVExtentDirectionHorizontal && !bAllowMoveX)
			{
				ret = false;

			}
			else   if (Direction == SvPb::SVExtentDirectionVertical && !bAllowMoveY)
			{

				ret = false;
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

} //namespace SvTo
