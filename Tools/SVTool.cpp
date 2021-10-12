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
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVExtentPropertyInfoStruct.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "Operators/SVConditional.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Overlay.h"
#pragma endregion Includes

namespace SvTo
{

constexpr long MinScaleFactorThreshold = 0; // Scale Factor may not 
											// be less than or equal 
											// to 0.
constexpr long MaxScaleFactor = 1000;		// Maximum allowed Scale Factor. 
											// Arbitrary limit of 1000.

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS(SVToolClass, SVToolClassId);

SVToolClass::SVToolClass(SVObjectClass* POwner, int StringResourceID /*= IDS_CLASSNAME_SVTOOL*/)
	: SVTaskObjectListClass(POwner, StringResourceID)
	, m_pToolConditional(nullptr)
	, m_toolExtent{m_imageExtent}
{
	init();
}

void SVToolClass::init()
{
	// Indentify our type in the Output List
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_isObjectValid, SvPb::TaskObjectClassIsObjectValidEId, IDS_OBJECTNAME_ISVALID, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&enabled, SvPb::ToolEnabledEId, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Passed, SvPb::PassedEId, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Failed, SvPb::FailedEId, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_Warned, SvPb::WarnedEId, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExplicitFailed, SvPb::ExplicitFailedEId, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_PassedCount, SvPb::PassedCountEId, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_FailedCount, SvPb::FailedCountEId, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_WarnedCount, SvPb::WarnedCountEId, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_EnabledCount, SvPb::EnabledCountEId, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ProcessedCount, SvPb::ProcessedCountEId, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ToolPosition, SvPb::ToolPositionEId, IDS_OBJECTNAME_TOOL_POSITION, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_ToolTime, SvPb::ToolTimeEId, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_editFreezeFlag, SvPb::ConditionalEditFreezeFlagEId, IDS_OBJECTNAME_EDITMODE_FREEZEFLAG, false, SvOi::SVResetItemNone);

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
	m_ExtentHeightFactorContent.SetTypeName(_T("Scale Factor")); 
	m_ExtentWidthFactorContent.SetTypeName(_T("Scale Factor"));
	m_ExtentHeightFactorFormat.SetTypeName(_T("Scale Factor"));
	m_ExtentWidthFactorFormat.SetTypeName(_T("Scale Factor"));

	RegisterEmbeddedObject(&m_ExtentLeft, SvPb::ExtentRelativeLeftPositionEId, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentTop, SvPb::ExtentRelativeTopPositionEId, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentRight, SvPb::ExtentRelativeRightPositionEId, IDS_OBJECTNAME_EXTENT_RIGHT, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExtentBottom, SvPb::ExtentRelativeBottomPositionEId, IDS_OBJECTNAME_EXTENT_BOTTOM, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ExtentWidth, SvPb::ExtentWidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ExtentHeight, SvPb::ExtentHeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&ToolSelectedForOperatorMove, SvPb::ToolSelectedForOperatorMoveEId, IDS_OBJECTNAME_TOOL_SELECTED_FOR_OPERATOR_MOVE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_drawToolFlag, SvPb::ConditionalToolDrawFlagEId, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svUpdateAuxiliaryExtents, SvPb::UpdateAuxiliaryExtentsEId, IDS_OBJECTNAME_UPDATE_AUXILIARY_EXTENTS_OBJECT, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_svAuxiliarySourceX, SvPb::AuxiliarySourceXEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_X, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliarySourceY, SvPb::AuxiliarySourceYEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_Y, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliarySourceAngle, SvPb::AuxiliarySourceAngleEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_ANGLE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliarySourceImageName, SvPb::AuxiliarySourceImageNameEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_IMAGE_NAME, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svAuxiliaryDrawType, SvPb::AuxiliaryDrawTypeEId, IDS_OBJECTNAME_AUXILIARY_DRAW_TYPE_NAME, false, SvOi::SVResetItemNone);
	// Tool Comment...
	RegisterEmbeddedObject(&m_ToolComment, SvPb::ToolCommentTypeEId, IDS_OBJECTNAME_TOOL_COMMENT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_ExtentWidthFactorContent, SvPb::ExtentWidthFactorContentEId, IDS_OBJECTNAME_EXTENT_WIDTH_FACTOR_CONTENT, true, SvOi::SVResetItemTool);
	m_ExtentWidthFactorContent.SetDefaultValue(_variant_t(0.0), true);

	RegisterEmbeddedObject(&m_ExtentHeightFactorContent, SvPb::ExtentHeightFactorContentEId, IDS_OBJECTNAME_EXTENT_HEIGHT_FACTOR_CONTENT, true, SvOi::SVResetItemTool);
	m_ExtentHeightFactorContent.SetDefaultValue(_variant_t(0.0), true);

	RegisterEmbeddedObject(&m_ExtentWidthFactorFormat, SvPb::ExtentWidthFactorFormatEId, IDS_OBJECTNAME_EXTENT_WIDTH_FACTOR_FORMAT, true, SvOi::SVResetItemTool);
	m_ExtentWidthFactorFormat.SetDefaultValue(_variant_t(0.0), true);

	RegisterEmbeddedObject(&m_ExtentHeightFactorFormat, SvPb::ExtentHeightFactorFormatEId, IDS_OBJECTNAME_EXTENT_HEIGHT_FACTOR_FORMAT, true, SvOi::SVResetItemTool);
	m_ExtentHeightFactorFormat.SetDefaultValue(_variant_t(0.0), true);

	m_toolExtent.SetTool(this);
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationShift);
	m_toolExtent.SetAlwaysUpdate(false);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointX, &m_ExtentLeft);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointY, &m_ExtentTop);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidth, &m_ExtentWidth);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeight, &m_ExtentHeight);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthFactorContent, &m_ExtentWidthFactorContent);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightFactorContent, &m_ExtentHeightFactorContent);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthFactorFormat, &m_ExtentWidthFactorFormat);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightFactorFormat, &m_ExtentHeightFactorFormat);

	//
	// Set Embedded defaults
	m_isObjectValid.SetDefaultValue(BOOL(false), true);
	m_isObjectValid.setSaveValueFlag(false);
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
	m_editFreezeFlag.SetDefaultValue(BOOL(false), false);

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
	m_ExtentWidthFactorContent.SetDefaultValue(SvDef::cDefaultScaleFactor, true);
	m_ExtentHeightFactorContent.SetDefaultValue(SvDef::cDefaultScaleFactor, true);
	m_ExtentWidthFactorFormat.SetDefaultValue(0.0, true);
	//this needs to be 0.0 rather than SvDef::cDefaultScaleFactor to ensure proper behaviour 
	//with pre-10.10 configurations in ResizeTool::CreateObject()
	m_ExtentHeightFactorFormat.SetDefaultValue(0.0, true);
	//this needs to be 0.0 rather than SvDef::cDefaultScaleFactor to ensure proper behaviour 
	//with pre-10.10 configurations in ResizeTool::CreateObject()

	m_drawToolFlag.SetEnumTypes(IDS_TOOLDRAW_ENUMOBJECT_LIST);
	m_drawToolFlag.SetDefaultValue(0L, true);

	m_svUpdateAuxiliaryExtents.SetDefaultValue(BOOL(false), true);

	m_svAuxiliarySourceX.SetDefaultValue(0.0, true);
	m_svAuxiliarySourceX.setSaveValueFlag(false);
	m_svAuxiliarySourceY.SetDefaultValue(0.0, true);
	m_svAuxiliarySourceY.setSaveValueFlag(false);
	m_svAuxiliarySourceAngle.SetDefaultValue(0.0, true);
	m_svAuxiliarySourceAngle.setSaveValueFlag(false);
	m_svAuxiliarySourceImageName.SetDefaultValue(_T(""), true);
	m_svAuxiliarySourceImageName.setSaveValueFlag(false);
	m_svAuxiliaryDrawType.SetDefaultValue("", true);
	m_svAuxiliaryDrawType.setSaveValueFlag(false);

	m_pCurrentToolSet = nullptr;

	// Auxiliary Source Image.
	m_AuxSourceImageInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_AuxSourceImageInput, _T("ToolAuxSourceImage"), SvPb::AuxImageInputEId);

	// instantiate the Conditional class
	SvOp::SVConditional* pCondition = new SvOp::SVConditional(this);
	AddFriend(pCondition->getObjectId());

	// Identify our input type needs
	m_conditionBoolInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::ConditionalResultEId);
	registerInputObject(&m_conditionBoolInput, _T("ToolConditionalValue"), SvPb::ToolConditionalInputEId);
	m_conditionBoolInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_overlayColorToolInput.SetInputObjectType(SvPb::SVToolObjectType);
	registerInputObject(&m_overlayColorToolInput, _T("OverlayColor_Tool"), SvPb::OverlayColorInputEId);
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
		m_pToolConditional = dynamic_cast<SvOp::SVConditional*> (m_friendList[j].getObject());
		setSkipFirstFriendFromRun();
	}

	// Set / Reset Printable Flags
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_isObjectValid.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
	enabled.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_Passed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Failed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_Warned.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ExplicitFailed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_PassedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_FailedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_WarnedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_EnabledCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ProcessedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ToolTime.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_ExtentLeft.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable | SvPb::extentObject, SvOi::SetAttributeType::AddAttribute);
	m_ExtentTop.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable | SvPb::extentObject, SvOi::SetAttributeType::AddAttribute);
	m_ExtentRight.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ExtentBottom.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ExtentWidth.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable | SvPb::extentObject | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeight.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable | SvPb::extentObject | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute);

	// Defaults for the Scale Factors should be hidden but NOT removed at this time
	m_ExtentWidthFactorContent.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentHeightFactorContent.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentWidthFactorFormat.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentHeightFactorFormat.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);

	// Auxiliary Tool Source Extent
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	m_drawToolFlag.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	// Tool Comment attributes...
	m_ToolComment.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ToolComment.SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);	// We do not want this to show up in the results picker.

	if (areAuxExtentsAvailable())
	{
		BOOL bEnabled{ false };
		m_svUpdateAuxiliaryExtents.GetValue(bEnabled);
		m_AuxSourceImageInput.SetObjectAttributesAllowed(bEnabled ? SvPb::audittrail | SvPb::embedable : SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}
	else
	{
		m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}

	if (false == useOverlayColorTool())
	{
		m_overlayColorToolInput.SetInputObject(nullptr);
		m_overlayColorToolInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}
	else if (false == m_OverlayColorToolInputIsSet)
	{
		m_overlayColorToolInput.SetInputObject(this);
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVToolClass::CloseObject()
{
	m_toolExtent.SetToolImage(nullptr);
	m_toolExtent.SetSelectedImage(nullptr);

	return SVTaskObjectListClass::CloseObject();
}

bool SVToolClass::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages)
{
	BOOL freezeFlag(false);
	m_editFreezeFlag.GetValue(freezeFlag);
	setEditModeFreezeFlag(TRUE == freezeFlag);

	bool result = __super::resetAllObjects(pErrorMessages);
	m_isObjectValid.SetValue(BOOL(result));
	return result;
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

void SVToolClass::GetDrawInfo(SVExtentMultiLineStruct& rMultiLine)
{
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
			SvIe::SVImageClass* pAuxSourceImage = m_AuxSourceImageInput.getInput<SvIe::SVImageClass>(true);
			
			m_toolExtent.SetSelectedImage(pAuxSourceImage);

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

inline  void SVToolClass::UpdateStateAndCounter(SvIe::RunStatus& rRunStatus)
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

	setStatus(rRunStatus);
}

bool SVToolClass::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool retVal = true;
	clearRunErrorMessages();
	m_ToolTime.Start();
	bool isEnabled = IsEnabled();
	BOOL freezeFlag(false);
	m_editFreezeFlag.GetValue(freezeFlag);
	if (TRUE == freezeFlag && isEnabled)
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_READY|SV_STATE_EDIT) && (false == SVSVIMStateClass::CheckState(SV_STATE_REGRESSION|SV_STATE_TEST)))
		{
			assert(rRunStatus.m_triggerRecord);
			if (nullptr != rRunStatus.m_triggerRecord)
			{
				copiedSavedImage(rRunStatus.m_triggerRecord);
			}
			isEnabled = false;
		}
	}
	
	if (isEnabled)
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

	if (retVal && isErrorMessageEmpty())
	{
		m_isObjectValid.SetValue(BOOL(true));
	}
	else
	{
		m_isObjectValid.SetValue(BOOL(false));
		rRunStatus.SetInvalid();
		retVal = false;
	}

	return retVal;
}// end Run

bool SVToolClass::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
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

bool SVToolClass::getConditionalResult(bool /*= false*/) const
{
	BOOL Value(false);
	SvVol::SVBoolValueObjectClass* pBoolObject = m_conditionBoolInput.getInput<SvVol::SVBoolValueObjectClass>();
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

	UINT auxAttribute = bEnabled ? SvDef::defaultValueObjectAttributes : SvPb::noAttributes;

	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
	m_svAuxiliarySourceX.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
	m_svAuxiliarySourceY.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);

	m_svAuxiliarySourceX.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_svAuxiliarySourceY.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
}

bool SVToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	///UpdateBottomAndRight is called again when imageExtents are changed by ToolsizeAdjust
	if (Result)
	{
		UpdateBottomAndRight();
	}

	setAuxiliaryExtents();

	if (areAuxExtentsAvailable())
	{
		m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
		BOOL bEnabled{ false };
		m_svUpdateAuxiliaryExtents.GetValue(bEnabled);
		if (bEnabled)
		{
			m_AuxSourceImageInput.validateInput();
		}
		m_AuxSourceImageInput.SetObjectAttributesAllowed(bEnabled? SvPb::audittrail | SvPb::embedable : SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}
	else
	{
		m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}

	m_conditionBoolInput.validateInput();

	if (useOverlayColorTool())
	{
		m_overlayColorToolInput.validateInput();
	}
	
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

void SVToolClass::SetAlwaysUpdate(bool p_bAlwaysUpdate)
{
	m_toolExtent.SetAlwaysUpdate(p_bAlwaysUpdate);
}

HRESULT SVToolClass::UpdateOffsetDataToImage(SVExtentOffsetStruct& p_rsvOffsetData, SvIe::SVImageClass* pToolImage)
{
	return m_toolExtent.UpdateOffsetDataToImage(p_rsvOffsetData, pToolImage);
}

bool SVToolClass::propagateSizeAndPosition()
{
	if (SvOp::ToolSizeAdjustTask::UseSizeAdjust(this))
	{
		return resetAllObjects();
	}
	return true;
}

bool SVToolClass::usePropagateSizeAndPosition() const
{
	if (SvOp::ToolSizeAdjustTask::UseSizeAdjust(this))
	{
		return true;
	}
	return false;
}

void SVToolClass::removeTaskMessages(DWORD MessageCode, SvStl::MessageTextEnum AdditionalTextId)
{
	removeTaskMessage(MessageCode, AdditionalTextId);
	for (auto& TaskObj : m_TaskObjectVector)
	{
		TaskObj->removeTaskMessage(MessageCode, AdditionalTextId);
	}
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

bool SVToolClass::SetDefaultFormulas(SvStl::MessageContainerVector*)
{
	return true;
}

HRESULT SVToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	return m_toolExtent.SetImageExtent(rImageExtent);
}

void SVToolClass::removeEmbeddedExtents()
{
	m_hasExtents = false;
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointX, nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointY, nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidth, nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeight, nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthFactorContent, nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightFactorContent, nullptr);

	RemoveEmbeddedObject(&m_ExtentLeft); // Remove it from the Embedded List so it is not scripted
	RemoveEmbeddedObject(&m_ExtentTop);
	RemoveEmbeddedObject(&m_ExtentRight);
	RemoveEmbeddedObject(&m_ExtentBottom);
	RemoveEmbeddedObject(&m_ExtentWidth);
	RemoveEmbeddedObject(&m_ExtentHeight);
	RemoveEmbeddedObject(&m_ExtentWidthFactorContent);
	RemoveEmbeddedObject(&m_ExtentHeightFactorContent);
	RemoveEmbeddedObject(&m_ExtentWidthFactorFormat);
	RemoveEmbeddedObject(&m_ExtentHeightFactorFormat);
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

void SVToolClass::UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine)
{
	SVTaskObjectListClass::UpdateOverlayIDs(p_rMultiLine);

	if (SvDef::InvalidObjectId == p_rMultiLine.m_ToolID)
	{
		p_rMultiLine.m_ToolID = getObjectId();
	}
}

HRESULT SVToolClass::CollectOverlays(SvIe::SVImageClass *pImage, SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pImage && isInputImage(pImage->getObjectId()))
	{
		l_Status = SVTaskObjectListClass::CollectOverlays(pImage, rMultiLineArray);
	}

	return l_Status;
}

void SVToolClass::UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine) const
{
	SVToolClass* pTool = m_overlayColorToolInput.getInput<SVToolClass>();
	COLORREF color = (nullptr != pTool) ? pTool->GetObjectColor() : GetObjectColor();
	p_rMultiLine.m_Color = color;
}

void SVToolClass::addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const
{
	auto figure = GetImageExtent().GetFigure();
	switch (figure.m_eShape)
	{
		case SvDef::SVExtentShapeRectangle:
		{
			auto* pOverlay = rOverlay.add_overlays();
			pOverlay->set_name(GetName());
			pOverlay->set_objectid(getObjectId());
			pOverlay->set_displaybounding(true);
			auto* pBoundingBox = pOverlay->mutable_boundingshape();
			auto* pRect = pBoundingBox->mutable_rect();
			SvPb::setValueObject(m_ExtentLeft, *pRect->mutable_x());
			SvPb::setValueObject(m_ExtentTop, *pRect->mutable_y());
			SvPb::setValueObject(m_ExtentWidth, *pRect->mutable_w());
			SvPb::setValueObject(m_ExtentHeight, *pRect->mutable_h());
			setStateValueToOverlay(*pOverlay);
			collectOverlays(pImage, *pOverlay);
			break;
		}
		default:
			return;
	}
}

// Source Image Functions
HRESULT SVToolClass::GetSourceImages(SvIe::SVImageClassPtrVector* pImageList) const
{
	HRESULT l_hr = S_OK;

	if (m_toolExtent.GetToolImage())
	{
		SvIe::SVImageClass* pImageParent = m_toolExtent.GetToolImage()->GetParentImage();
		if (nullptr != pImageParent)
		{
			const SVToolClass* pTool = dynamic_cast<SVToolClass*>(pImageParent->GetTool());

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
		if (0 < imageList.size())
		{
			auto iter = std::find_if(imageList.begin(), imageList.end(), [&pImage](const auto& rEntry) { return rEntry == pImage; });
			if (imageList.end() != iter)
			{
				m_AuxSourceImageInput.SetInputObject(*iter); 
				l_hr = S_OK;
			}
			else
			{
				m_AuxSourceImageInput.SetInputObject(imageList[0]);
			}
		}
		else
		{
			m_AuxSourceImageInput.SetInputObject(nullptr);
		}
		
		m_toolExtent.SetSelectedImage(m_AuxSourceImageInput.getInput<SvIe::SVImageClass>());

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

SVObjectClass* SVToolClass::overwriteInputObject(uint32_t uniqueId, SvPb::EmbeddedIdEnum embeddedId)
{
	m_OverlayColorToolInputIsSet = m_OverlayColorToolInputIsSet || (SvPb::OverlayColorInputEId == embeddedId);
	return __super::overwriteInputObject(uniqueId, embeddedId);
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

bool SVToolClass::addEntryToMonitorList(SvOi::ParametersForML &retList, SvPb::EmbeddedIdEnum embeddedID) const
{
	auto* pResultObject = getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType, embeddedID));
	if (nullptr != pResultObject)
	{
		if (SvPb::noAttributes != (pResultObject->ObjectAttributesAllowed()))
		{
			retList.push_back(SvOi::ParameterPairForML(pResultObject->GetCompleteName(), pResultObject->getObjectId()));
			return true;
		}
	}
	return false;
}

#pragma region ITool methods
bool SVToolClass::areAuxExtentsAvailable() const
{
	return (SvDef::InvalidObjectId != GetToolImage()) ? GetInspectionInterface()->getEnableAuxiliaryExtent() : false;
}

SvUl::NameObjectIdList SVToolClass::getAvailableAuxSourceImages() const
{
	SvUl::NameObjectIdList list;
	SvIe::SVImageClassPtrVector ImageList;
	HRESULT hr = GetSourceImages(&ImageList);
	if (S_OK == hr)
	{
		std::transform(ImageList.begin(), ImageList.end(), std::back_inserter(list), 
			[](const auto& rEntry) { return std::make_pair(rEntry->getDisplayedName(), rEntry->getObjectId()); });
	}
	return list;
}

SvUl::NameObjectIdPair SVToolClass::getAuxSourceImage() const
{
	SvUl::NameObjectIdPair result;
	SvIe::SVImageClass* pImage = m_AuxSourceImageInput.getInput<SvIe::SVImageClass>();
	if (pImage)
	{
		result = std::make_pair(pImage->getDisplayedName(), pImage->getObjectId());
	}
	return result;
}

HRESULT SVToolClass::setAuxSourceImage(uint32_t objectID)
{
	HRESULT hr = E_POINTER;
	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SvOi::getObject(objectID));
	if (nullptr != pImage)
	{
		hr = SetAuxSourceImage(pImage);
	}
	return hr;
}

void SVToolClass::SetToolImage(uint32_t objectID)
{
	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SvOi::getObject(objectID));
	m_toolExtent.SetToolImage(pImage);
}


uint32_t SVToolClass::GetToolImage() const
{
	auto* pImage = m_toolExtent.GetToolImage();
	return nullptr != pImage ? pImage->getObjectId() : SvDef::InvalidObjectId;
}

long SVToolClass::getToolPosition() const
{
	long Result(-1);
	m_ToolPosition.GetValue(Result);
	return Result;
};

SvOi::ParametersForML SVToolClass::getParameterForMonitorList(SvStl::MessageContainerVector&) const
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
		SvStl::MessageManager message(SvStl::MsgType::Log | SvStl::MsgType::Display);
		message.setMessage(iter->getMessage());
	}
}

SvPb::EAutoSize SVToolClass::getAutoSizeEnabled() const
{
	return GetAutoSizeEnabled();
}

void SVToolClass::resetCounters()
{
	// Reset Counter...
	m_PassedCount.SetDefaultValue(0, true);
	m_FailedCount.SetDefaultValue(0, true);
	m_WarnedCount.SetDefaultValue(0, true);
	m_EnabledCount.SetDefaultValue(0, true);
	m_ProcessedCount.SetDefaultValue(0, true);
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

void SVToolClass::setStateValueToOverlay(SvPb::Overlay& rOverlay) const
{
	SVToolClass* pTool = m_overlayColorToolInput.getInput<SVToolClass>();
	if (nullptr != pTool)
	{
		rOverlay.mutable_color()->set_trpos(pTool->getColorObject().getTrPos() + 1);
	}
	else
	{
		rOverlay.mutable_color()->set_trpos(m_statusColor.getTrPos() + 1);
	}

	SvPb::setValueObject(m_Passed, *rOverlay.mutable_passed(), true);
	SvPb::setValueObject(m_Failed, *rOverlay.mutable_failed(), true);
	SvPb::setValueObject(m_Warned, *rOverlay.mutable_warned(), true);
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
	if (m_conditionBoolInput.IsConnected() && m_conditionBoolInput.GetInputObjectInfo().getObject())
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

bool isValidScaleFactor(double value)
{
	if ((value <= MinScaleFactorThreshold) || (value > MaxScaleFactor))
	{
		return false;
	}

	return true;
}

bool isValidScaleFactorLV(SvVol::LinkedValue& scaleFactorValue)
{
	double scaleFactor = 0.0;
	scaleFactorValue.getValue(scaleFactor);
	return isValidScaleFactor(scaleFactor);
}

} //namespace SvTo
