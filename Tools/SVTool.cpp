//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVStatusLibrary/SvimState.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Overlay.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVUtilityLibrary/RaiiLifeFlag.h"
#include "SVUtilityLibrary/VectorHelper.h"

#pragma endregion Includes

namespace
{
bool isTranslationValidForAncestorOverlay(SvPb::SVExtentTranslationEnum translation)
{
	return SvPb::SVExtentTranslationShift == translation || SvPb::SVExtentTranslationProfileShift == translation || SvPb::SVExtentTranslationFigureShift == translation;
}
}

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

void getOverlays(const SvIe::SVImageClass& rImage, const SvIe::SVTaskObjectPtrVector& rVector, SVExtentMultiLineStructVector& rMultiLineArray)
{
	for (auto* pTask : rVector)
	{
		auto* pTool = dynamic_cast<SvTo::SVToolClass*>(pTask);
		if (nullptr != pTool)
		{
			pTool->getOverlays(rImage, rMultiLineArray);
		}
	}
}

void getOverlays(const SvIe::SVImageClass& rImage, const SvIe::SVTaskObjectPtrVector& rVector, SvPb::OverlayDesc& rOverlay)
{
	for (auto* pTask : rVector)
	{
		auto* pTool = dynamic_cast<SvTo::SVToolClass*>(pTask);
		if (nullptr != pTool)
		{
			pTool->getOverlays(rImage, rOverlay);
		}
	}
}

uint32_t InsertDependentTools(std::back_insert_iterator<std::vector<uint32_t>>  InIt, uint32_t toolobjectId)
{

	uint32_t res {0};
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getdependencyrequest();
	auto* idSet = pRequest->mutable_idsofobjectsdependedon();
	idSet->Add(toolobjectId);
	pRequest->set_objecttype(SvPb::SVToolObjectType);
	pRequest->set_tooldependecytype(SvPb::ToolDependencyEnum::Client);
	pRequest->set_dependecytype(SvPb::DependecyTypeEnum::Tool);
	pRequest->set_alldependecies(true);

	HRESULT hr = SvCmd::InspectionCommands(0, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getdependencyresponse())
	{
		for (const auto& pair : responseCmd.getdependencyresponse().dependencypair())
		{
			res++;
			InIt = pair.client().toolobjectid();
	
		}
	}
	return res;
}


EmbeddedExtents::EmbeddedExtents(SVToolClass* pOwner, ToolExtType tetype)
	:m_pOwner(pOwner), m_ToolExtType(tetype)
{

}

void EmbeddedExtents::SetTypeNameForExtents()
{
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
	;
}



void EmbeddedExtents::RegisterEmbeddedExtents()
{
	if (m_pOwner)
	{
		if (m_ToolExtType == ToolExtType::All)
		{
			m_pOwner->RegisterEmbeddedObject(&m_ExtentLeft, SvPb::ExtentRelativeLeftPositionEId, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemToolAndDependent, true);
			m_pOwner->RegisterEmbeddedObject(&m_ExtentTop, SvPb::ExtentRelativeTopPositionEId, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemToolAndDependent, true);
			m_pOwner->RegisterEmbeddedObject(&m_ExtentRight, SvPb::ExtentRelativeRightPositionEId, IDS_OBJECTNAME_EXTENT_RIGHT, false, SvOi::SVResetItemNone, false);
			m_pOwner->RegisterEmbeddedObject(&m_ExtentBottom, SvPb::ExtentRelativeBottomPositionEId, IDS_OBJECTNAME_EXTENT_BOTTOM, false, SvOi::SVResetItemNone, false);
			m_pOwner->RegisterEmbeddedObject(&m_ExtentWidth, SvPb::ExtentWidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemToolAndDependent, true);
			m_pOwner->RegisterEmbeddedObject(&m_ExtentHeight, SvPb::ExtentHeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemToolAndDependent, true);
		}
		m_pOwner->RegisterEmbeddedObject(&m_svAuxiliarySourceX, SvPb::AuxiliarySourceXEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_X, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_svAuxiliarySourceY, SvPb::AuxiliarySourceYEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_Y, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_svAuxiliarySourceAngle, SvPb::AuxiliarySourceAngleEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_ANGLE, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_svAuxiliarySourceImageName, SvPb::AuxiliarySourceImageNameEId, IDS_OBJECTNAME_AUXILIARY_SOURCE_IMAGE_NAME, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_svAuxiliaryDrawType, SvPb::AuxiliaryDrawTypeEId, IDS_OBJECTNAME_AUXILIARY_DRAW_TYPE_NAME, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_ExtentWidthFactorContent, SvPb::ExtentWidthFactorContentEId, IDS_OBJECTNAME_EXTENT_WIDTH_FACTOR_CONTENT, true, SvOi::SVResetItemToolAndDependent, true);
		m_ExtentWidthFactorContent.SetDefaultValue(_variant_t(0.0), true);

		m_pOwner->RegisterEmbeddedObject(&m_ExtentHeightFactorContent, SvPb::ExtentHeightFactorContentEId, IDS_OBJECTNAME_EXTENT_HEIGHT_FACTOR_CONTENT, true, SvOi::SVResetItemToolAndDependent, true);
		m_ExtentHeightFactorContent.SetDefaultValue(_variant_t(0.0), true);

		m_pOwner->RegisterEmbeddedObject(&m_ExtentWidthFactorFormat, SvPb::ExtentWidthFactorFormatEId, IDS_OBJECTNAME_EXTENT_WIDTH_FACTOR_FORMAT, true, SvOi::SVResetItemToolAndDependent, true);
		m_ExtentWidthFactorFormat.SetDefaultValue(_variant_t(0.0), true);

		m_pOwner->RegisterEmbeddedObject(&m_ExtentHeightFactorFormat, SvPb::ExtentHeightFactorFormatEId, IDS_OBJECTNAME_EXTENT_HEIGHT_FACTOR_FORMAT, true, SvOi::SVResetItemToolAndDependent, true);
		m_ExtentHeightFactorFormat.SetDefaultValue(_variant_t(0.0), true);

		m_pOwner->RegisterEmbeddedObject(&m_ancestorOffsetX, SvPb::OverlayInAncestorOffsetXEId, IDS_OBJECTNAME_OVERLAY_IN_ANCESTOR_OFFSET_X, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_ancestorOffsetY, SvPb::OverlayInAncestorOffsetYEId, IDS_OBJECTNAME_OVERLAY_IN_ANCESTOR_OFFSET_Y, false, SvOi::SVResetItemNone, false);
		m_pOwner->RegisterEmbeddedObject(&m_ancestorOffsetAngle, SvPb::OverlayInAncestorOffsetAngleEId, IDS_OBJECTNAME_OVERLAY_IN_ANCESTOR_OFFSET_ANGLE, false, SvOi::SVResetItemNone, false);
	}
}

void EmbeddedExtents::SetDefaults()
{
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

	m_ancestorOffsetX.SetDefaultValue(0.0, true);
	m_ancestorOffsetX.setSaveValueFlag(false);
	m_ancestorOffsetY.SetDefaultValue(0.0, true);
	m_ancestorOffsetY.setSaveValueFlag(false);
	m_ancestorOffsetAngle.SetDefaultValue(0.0, true);
	m_ancestorOffsetAngle.setSaveValueFlag(false);
}

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS(SVToolClass, SVToolClassId);

void EmbeddedExtents::SetAttributes()
{

	m_ExtentLeft.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentTop.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentRight.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ExtentBottom.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_ExtentWidth.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeight.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	// Defaults for the Scale Factors should be hidden but NOT removed at this time
	m_ExtentWidthFactorContent.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentHeightFactorContent.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentWidthFactorFormat.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_ExtentHeightFactorFormat.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);


}

SVToolClass::SVToolClass(ToolExtType toolExtType, SVObjectClass* POwner, int StringResourceID /*= IDS_CLASSNAME_SVTOOL*/)
	: SVTaskObjectListClass(POwner, StringResourceID)
	, m_pToolConditional(nullptr)
{
	if (ToolExtType::None != toolExtType)
	{
		m_pEmbeddedExtents = std::make_unique<EmbeddedExtents>(this, toolExtType);
	}
	init();
}

void SVToolClass::init()
{
	// Identify our type in the Output List
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_isObjectValid, SvPb::TaskObjectClassIsObjectValidEId, IDS_OBJECTNAME_ISVALID, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_EnableTool, SvPb::ToolEnabledEId, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject(&m_IsToolActive, SvPb::ToolActiveEId, IDS_OBJECTNAME_ACTIVE, false, SvOi::SVResetItemNone, true);

	m_pwf.registerPwfAndSetDefaults(*this);
	RegisterEmbeddedObject(&m_ExplicitFailed, SvPb::ExplicitFailedEId, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone, false);

	RegisterEmbeddedObject(&m_PassedCount, SvPb::PassedCountEId, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_FailedCount, SvPb::FailedCountEId, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_WarnedCount, SvPb::WarnedCountEId, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone, false);

	RegisterEmbeddedObject(&m_EnabledCount, SvPb::EnabledCountEId, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_ProcessedCount, SvPb::ProcessedCountEId, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_ToolPosition, SvPb::ToolPositionEId, IDS_OBJECTNAME_TOOL_POSITION, false, SvOi::SVResetItemNone, false);

	RegisterEmbeddedObject(&m_ToolTime, SvPb::ToolTimeEId, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_editFreezeFlag, SvPb::ConditionalEditFreezeFlagEId, IDS_OBJECTNAME_EDITMODE_FREEZEFLAG, false, SvOi::SVResetItemNone, true);

	if (m_pEmbeddedExtents.get())
	{
		m_pEmbeddedExtents->SetTypeNameForExtents();
		m_pEmbeddedExtents->RegisterEmbeddedExtents();

	}

	RegisterEmbeddedObject(&m_drawToolFlag, SvPb::ConditionalToolDrawFlagEId, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject(&m_svUpdateAuxiliaryExtents, SvPb::UpdateAuxiliaryExtentsEId, IDS_OBJECTNAME_UPDATE_AUXILIARY_EXTENTS_OBJECT, false, SvOi::SVResetItemToolAndDependent, true);
	RegisterEmbeddedObject(&m_isShowOverlayInAncestorEnabled, SvPb::IsShowOverlayInAncestorEnabledEId, IDS_OBJECTNAME_Is_SHOW_OVERLAY_IN_ANCESTOR_ENABLED, false, SvOi::SVResetItemToolAndDependent, true);

	m_toolExtent.SetTool(this);
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationShift);
	m_toolExtent.SetAlwaysUpdate(false);
	bool bEx = (m_pEmbeddedExtents.get() != nullptr);

	if (bEx && m_pEmbeddedExtents->m_ToolExtType == ToolExtType::All)
	{
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointX, &(m_pEmbeddedExtents->m_ExtentLeft));
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointY, &(m_pEmbeddedExtents->m_ExtentTop));
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidth, &(m_pEmbeddedExtents->m_ExtentWidth));
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeight, &(m_pEmbeddedExtents->m_ExtentHeight));
	}
	else
	{
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointX, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyPositionPointY, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidth, nullptr);
		m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeight, nullptr);
	}

	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthFactorContent, bEx ? &(m_pEmbeddedExtents->m_ExtentWidthFactorContent) : nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightFactorContent, bEx ? &(m_pEmbeddedExtents->m_ExtentHeightFactorContent) : nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyWidthFactorFormat, bEx ? &(m_pEmbeddedExtents->m_ExtentWidthFactorFormat) : nullptr);
	m_toolExtent.SetExtentObject(SvPb::SVExtentPropertyHeightFactorFormat, bEx ? &(m_pEmbeddedExtents->m_ExtentHeightFactorFormat) : nullptr);


	// Set Embedded defaults
	m_isObjectValid.SetDefaultValue(BOOL(false), true);
	m_isObjectValid.setSaveValueFlag(false);
	m_EnableTool.SetDefaultValue(BOOL(true), true);
	m_IsToolActive.SetDefaultValue(BOOL(true), true);
	m_ExplicitFailed.SetDefaultValue(BOOL(false), true);	// Default for Explicit Failed is FALSE !!!
	m_ExplicitFailed.setSaveValueFlag(false);

	m_PassedCount.SetDefaultValue(0L, true);
	m_PassedCount.setSaveValueFlag(false);
	m_PassedCount.SetObjectAttributesAllowed(SvPb::ObjectAttributes::shortMode, SvOi::AddAttribute);
	m_FailedCount.SetDefaultValue(0L, true);
	m_FailedCount.setSaveValueFlag(false);
	m_FailedCount.SetObjectAttributesAllowed(SvPb::ObjectAttributes::shortMode, SvOi::AddAttribute);
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
	if (m_pEmbeddedExtents.get())
	{
		m_pEmbeddedExtents->SetDefaults();
	}

	m_drawToolFlag.SetEnumTypes(IDS_TOOLDRAW_ENUMOBJECT_LIST);
	m_drawToolFlag.SetDefaultValue(0L, true);

	m_svUpdateAuxiliaryExtents.SetDefaultValue(BOOL(false), true);
	m_isShowOverlayInAncestorEnabled.SetDefaultValue(BOOL(false), true);

	m_pCurrentToolSet = nullptr;

	// Auxiliary Source Image.
	m_AuxSourceImageInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_AuxSourceImageInput, _T("ToolAuxSourceImage"), SvPb::AuxImageInputEId);

	m_AncestorForOverlayInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_AncestorForOverlayInput, _T("AncestorForOverlay"), SvPb::AncestorForOverlayInputEId);
	
	// instantiate the Conditional class
	SvOp::SVConditional* pCondition = new SvOp::SVConditional(this);
	AddFriend(pCondition);

	// Identify our input type needs
	m_conditionBoolInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::ConditionalResultEId);
	registerInputObject(&m_conditionBoolInput, _T("ToolConditionalValue"), SvPb::ToolConditionalInputEId);
	m_conditionBoolInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_overlayColorToolInput.SetInputObjectType(SvPb::SVToolObjectType);
	registerInputObject(&m_overlayColorToolInput, _T("OverlayColor_Tool"), SvPb::OverlayColorInputEId);
}

SVToolClass::~SVToolClass()
{}

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
		Log_Assert(0 == j);
		m_pToolConditional = dynamic_cast<SvOp::SVConditional*> (m_friendList[j]);
		setSkipFirstFriendFromRun();
	}

	// Set / Reset Printable Flags
	m_isObjectValid.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_EnableTool.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_IsToolActive.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable , SvOi::SetAttributeType::RemoveAttribute);
	m_pwf.setPwfObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ExplicitFailed.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_PassedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_FailedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_WarnedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_EnabledCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ProcessedCount.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ToolTime.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	if (m_pEmbeddedExtents.get())
	{
		m_pEmbeddedExtents->SetAttributes();
	}

	//// Auxiliary Tool Source Extent
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_isShowOverlayInAncestorEnabled.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	m_drawToolFlag.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	if (areAuxExtentsAvailable())
	{
		BOOL bEnabled {false};
		m_svUpdateAuxiliaryExtents.GetValue(bEnabled);
		m_AuxSourceImageInput.SetObjectAttributesAllowed(bEnabled ? SvPb::audittrail | SvPb::useable : SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

		m_isShowOverlayInAncestorEnabled.GetValue(bEnabled);
		m_AncestorForOverlayInput.SetObjectAttributesAllowed(bEnabled ? SvPb::audittrail | SvPb::useable : SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}
	else
	{
		m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		m_AncestorForOverlayInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
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

bool SVToolClass::resetAllObjects(SvStl::MessageContainerVector* pErrorMessages, int nResetDepth)
{

	BOOL freezeFlag(false);
	m_editFreezeFlag.GetValue(freezeFlag);
	setEditModeFreezeFlag(TRUE == freezeFlag);

	bool result = __super::resetAllObjects(pErrorMessages);
	m_isObjectValid.SetValue(BOOL(result));
	if (nResetDepth >0 )
	{
		resetAllDepedentObjects(nullptr, nResetDepth -1 );
	}

	return result;
}

bool SVToolClass::resetAllDepedentObjects(SvStl::MessageContainerVector*,int dependend )
{
	bool ret {true};
	if (m_ressetAll_Active)
	{
		Log_Assert(false);
		return ret;
	}
	SvDef::RaiiLifeFlag resetCheck(m_ressetAll_Active);

	std::vector<uint32_t> dependentTools;
	std::back_insert_iterator<std::vector<uint32_t>>  InsertIt(dependentTools);
	SvTo::InsertDependentTools(InsertIt, getObjectId());
	SvUl::RemoveDuplicates(dependentTools);
	for (const auto id : dependentTools)
	{
		SVObjectClass* pObj = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(id, pObj);
		if (pObj && pObj != this )
		{
			ret = pObj->resetAllObjects(nullptr, dependend) && ret;
		}

	}
	return ret;
}


bool SVToolClass::IsEnabled() const
{
	BOOL bEnabled(true);
	m_EnableTool.GetValue(bEnabled);

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

void SVToolClass::GetDrawInfo(SVExtentMultiLineStruct& rMultiLine) const
{
	if (m_pCurrentToolSet)
	{
		auto* pObject = dynamic_cast<SVObjectClass*> (m_pCurrentToolSet->GetDrawFlagObject());

		if (nullptr != pObject)
		{
			double Value;
			pObject->getValue(Value);
			rMultiLine.m_ToolSetDrawFlag = static_cast<long> (Value);
		}
	}

	m_drawToolFlag.GetValue(rMultiLine.m_ToolDrawFlag);

	std::tie(rMultiLine.m_Passed, rMultiLine.m_Warned, rMultiLine.m_Failed) = m_pwf.GetPassedWarnedFailedStatus();
}

void SVToolClass::UpdateAuxiliaryExtents()
{
	if (GetInspectionInterface()->getEnableAuxiliaryExtent())
	{
		bool l_bForceOffsetReset = GetInspectionInterface()->IsOffsetUpdateForced();
		m_toolExtent.UpdateOffsetData(l_bForceOffsetReset);

		if (m_pEmbeddedExtents)
		{
			BOOL isEnabled = false;
			HRESULT 	hr = m_svUpdateAuxiliaryExtents.GetValue(isEnabled);
			if (S_OK == hr && isEnabled)
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
					m_pEmbeddedExtents->m_svAuxiliarySourceImageName.SetValue(pAuxSourceImage->GetCompleteName());
				}

				std::string DrawType = m_toolExtent.GetAuxiliaryDrawTypeString();
				m_pEmbeddedExtents->m_svAuxiliaryDrawType.SetValue(DrawType);

				SVPoint<double> point;
				SvIe::SVImageClass* pImage = m_toolExtent.GetToolImage();
				if (nullptr != pImage && nullptr != pAuxSourceImage)
				{
					//check
					pImage->TranslateFromOutputSpaceToImageFromTool(pAuxSourceImage, point, point);
				}
				m_pEmbeddedExtents->m_svAuxiliarySourceX.SetValue(point.m_x);
				m_pEmbeddedExtents->m_svAuxiliarySourceY.SetValue(point.m_y);

				m_pEmbeddedExtents->m_svAuxiliarySourceAngle.SetValue(l_svOffsetData.m_dRotationAngle);
			}

			m_isShowOverlayInAncestorEnabled.GetValue(isEnabled);
			if (isEnabled)
			{
				SVExtentOffsetStruct l_svOffsetData;
				SvIe::SVImageClass* pSourceImage = m_AncestorForOverlayInput.getInput<SvIe::SVImageClass>(true);

				m_toolExtent.SetSelectedImage(pSourceImage);

				hr = m_toolExtent.GetSelectedOffsetData(l_svOffsetData);
				if (S_OK != hr || !l_svOffsetData.m_bIsLinear)
				{
					m_toolExtent.UpdateOffsetDataToImage(l_svOffsetData, pSourceImage);
				}

				SVPoint<double> point;
				SvIe::SVImageClass* pImage = m_toolExtent.GetToolImage();
				if (nullptr != pImage && nullptr != pSourceImage)
				{
					//check
					pImage->TranslateFromOutputSpaceToImageFromTool(pSourceImage, point, point);
				}
				m_pEmbeddedExtents->m_ancestorOffsetX.SetValue(point.m_x);
				m_pEmbeddedExtents->m_ancestorOffsetY.SetValue(point.m_y);

				m_pEmbeddedExtents->m_ancestorOffsetAngle.SetValue(l_svOffsetData.m_dRotationAngle);
			}
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
		m_pwf.m_Passed.SetValue(BOOL(rRunStatus.isPassed()));
		m_pwf.m_Failed.SetValue(BOOL(rRunStatus.isFailed()));
		m_ExplicitFailed.SetValue(BOOL(rRunStatus.isFailed()));
		m_pwf.m_Warned.SetValue(BOOL(rRunStatus.isWarned()));

		if (rRunStatus.m_UpdateCounters)
		{
			// Set Counts...
			long lCount = 0;
			if (rRunStatus.isPassed())
			{
				m_PassedCount.GetValue(lCount);
				m_PassedCount.SetValue(++lCount);
			}
			lCount = 0;
			if (rRunStatus.isFailed())
			{
				m_FailedCount.GetValue(lCount);
				m_FailedCount.SetValue(++lCount);
			}
			lCount = 0;
			if (rRunStatus.isWarned())
			{
				m_WarnedCount.GetValue(lCount);
				m_WarnedCount.SetValue(++lCount);
			}
		}
		SetToolActiveFlag(true);
	}
	else
	{
		SetToolActiveFlag(false);
	}
	setStatus(rRunStatus);
}

bool  SVToolClass::isToolActive() const
{
	if (!IsEnabled())
	{
		return false;
	}
	BOOL freezeFlag(false);
	m_editFreezeFlag.GetValue(freezeFlag);

	if (freezeFlag
		&& SvimState::CheckState(SV_STATE_READY | SV_STATE_EDIT)
		&& (false == SvimState::CheckState(SV_STATE_REGRESSION | SV_STATE_TEST)))
	{
		return false;
	}

	return true;
}
bool SVToolClass::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	bool retVal = true;
	clearRunErrorMessages();
	m_ToolTime.Start();
	bool isToolActive = IsEnabled();
	
	BOOL freezeFlag(false);
	m_editFreezeFlag.GetValue(freezeFlag);
	if (TRUE == freezeFlag && isToolActive)
	{
		if (SvimState::CheckState(SV_STATE_READY | SV_STATE_EDIT) && (false == SvimState::CheckState(SV_STATE_REGRESSION | SV_STATE_TEST)))
		{
			Log_Assert(rRunStatus.m_triggerRecord);
			if (nullptr != rRunStatus.m_triggerRecord)
			{
				copiedSavedImage(rRunStatus.m_triggerRecord);
			}
			isToolActive = false;
		}
	}

	if (isToolActive)
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
			bool isDisabled =  rRunStatus.IsDisabled();
			bool isDisabledByCondition  = rRunStatus.IsDisabledByCondition();
			
			retVal = __super::Run(rRunStatus, &m_RunErrorMessages);

			if (retVal)
			{
				rRunStatus.SetValid();
			}
			else
			{
				rRunStatus.SetInvalid();
			}
			rRunStatus.SetDisabled(isDisabled);
			rRunStatus.SetDisabledByCondition(isDisabledByCondition);

		}
		else
		{
			rRunStatus.SetDisabledByCondition();
			//@TODO[MEC][10.30][31.01.2023] The next line would make sense, but is commented out for performance reasons 
			//check if performance could be improved
			//SetDisabled();
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

void  SVToolClass::SetToolActiveFlag(bool isToolActiveFlag)
{
	m_IsToolActive.SetValue(isToolActiveFlag);
	if (isToolActiveFlag == false)
	{
		for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
		{
			SVTaskObjectClass* pTaskObject = getTaskObject(i);
			if (nullptr != pTaskObject)
			{
				pTaskObject->SetToolActiveFlag(FALSE);
			}
		}
	}
}
bool SVToolClass::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
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
	BOOL bEnabled {false};
	BOOL bShowAncestorOverlay {false};
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();

	if (nullptr != pInspection && pInspection->getEnableAuxiliaryExtent())
	{
		m_svUpdateAuxiliaryExtents.GetValue(bEnabled);
		m_isShowOverlayInAncestorEnabled.GetValue(bShowAncestorOverlay);
	}

	UINT auxAttribute = bEnabled ? SvDef::defaultValueObjectAttributes : SvPb::noAttributes;

	if (m_pEmbeddedExtents)
	{
		m_pEmbeddedExtents->m_svAuxiliarySourceX.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
		m_pEmbeddedExtents->m_svAuxiliarySourceY.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
		m_pEmbeddedExtents->m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
		m_pEmbeddedExtents->m_svAuxiliaryDrawType.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);
		m_pEmbeddedExtents->m_svAuxiliarySourceImageName.SetObjectAttributesAllowed(auxAttribute, SvOi::SetAttributeType::OverwriteAttribute);

		m_pEmbeddedExtents->m_svAuxiliarySourceX.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
		m_pEmbeddedExtents->m_svAuxiliarySourceY.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
		m_pEmbeddedExtents->m_svAuxiliarySourceAngle.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

		m_pEmbeddedExtents->m_ancestorOffsetX.SetObjectAttributesAllowed(bShowAncestorOverlay, SvOi::SetAttributeType::OverwriteAttribute);
		m_pEmbeddedExtents->m_ancestorOffsetY.SetObjectAttributesAllowed(bShowAncestorOverlay, SvOi::SetAttributeType::OverwriteAttribute);
		m_pEmbeddedExtents->m_ancestorOffsetAngle.SetObjectAttributesAllowed(bShowAncestorOverlay, SvOi::SetAttributeType::OverwriteAttribute);
	}
}

bool SVToolClass::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	///UpdateBottomAndRight is called again when imageExtents are changed by ToolsizeAdjust
	if (Result)
	{
		UpdateBottomAndRight();
	}

	setAuxiliaryExtents();

	m_AncestorIdForOverlay = SvDef::InvalidObjectId;
	if (areAuxExtentsAvailable())
	{
		SvPb::GetAvailableAncestorImagesResponse ancestorList;
		getAncestorImages(ancestorList);
		BOOL bEnabled {false};
		m_svUpdateAuxiliaryExtents.GetValue(bEnabled);
		if (bEnabled)
		{
			m_AuxSourceImageInput.validateInput();
			m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::useable, SvOi::SetAttributeType::OverwriteAttribute);
			auto objectId = m_AuxSourceImageInput.GetInputObjectInfo().getObjectId();
			if (std::ranges::none_of(ancestorList.list(), [objectId](const auto& rEntry){ return rEntry.objectid() == objectId; }))
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidImageForAux, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				Result = false;
			}
		}
		else
		{
			m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}

		m_isShowOverlayInAncestorEnabled.GetValue(bEnabled);
		if (bEnabled)
		{
			m_AncestorForOverlayInput.validateInput();
			m_AncestorForOverlayInput.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::useable, SvOi::SetAttributeType::OverwriteAttribute);
			m_AncestorIdForOverlay = m_AncestorForOverlayInput.GetInputObjectInfo().getObjectId();
			auto objectId = m_AncestorForOverlayInput.GetInputObjectInfo().getObjectId();
			auto iter = std::ranges::find_if(ancestorList.list(), [objectId](const auto& rEntry){ return rEntry.objectid() == objectId; });
			if (ancestorList.list().end() != iter)
			{
				while (ancestorList.list().end() != iter)
				{
					if (false == isTranslationValidForAncestorOverlay(iter->translationtype()))
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidImageForAncestorOverlay, SvStl::SourceFileParams(StdMessageParams), getObjectId());
							pErrorMessages->push_back(Msg);
						}
						Result = false;
						break;
					}
					++iter;
				}
			}
			else
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidImageForAncestorOverlay, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				Result = false;
			}
		}
		else
		{
			m_AncestorForOverlayInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}
	}
	else
	{
		m_AuxSourceImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		m_AncestorForOverlayInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
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
	updateImageExtent(true);

	if (m_pEmbeddedExtents && GetImageExtent().hasFigure())
	{
		const SVExtentFigureStruct& rFigure = GetImageExtent().GetFigure();
		if (SvPb::noAttributes != m_pEmbeddedExtents->m_ExtentRight.ObjectAttributesAllowed())
		{
			m_pEmbeddedExtents->m_ExtentRight.SetValue(static_cast<long>(rFigure.m_svBottomRight.m_x));
		}

		if (SvPb::noAttributes != m_pEmbeddedExtents->m_ExtentBottom.ObjectAttributesAllowed())
		{
			m_pEmbeddedExtents->m_ExtentBottom.SetValue(static_cast<long>(rFigure.m_svBottomRight.m_y));
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

SvPb::SVExtentLocationPropertyEnum SVToolClass::getLocationPropertyAtPoint(const SVPoint<double>& rPoint, bool isAncestorOverlay) const
{
	auto point = rPoint;
	if (isAncestorOverlay)
	{
		movePointFromAncestorToSourceImageCoordinate(point);
	}
	return getLocationPropertyAtPoint(point);
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
HRESULT SVToolClass::updateImageExtent(bool init)
{
	return m_toolExtent.updateImageExtent(init);
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

std::pair<SvPb::SVExtentLocationPropertyEnum, bool> SVToolClass::getLocationAtPointOnImage(uint32_t imageId, const SVPoint<double>& rPoint) const
{
	bool isValid = false;
	bool isAncestorOverlay = false;
	SvPb::SVExtentLocationPropertyEnum locationEnum = SvPb::SVExtentLocationPropertyUnknown;
	if (isInputImage(imageId))
	{
		isValid = true;
	}
	else if (const auto* pImageExtent = GetImageExtentPtr(); nullptr != pImageExtent && pImageExtent->hasFigure() && getAncestorIdForOverlay() == imageId && isTranslationValidForAncestorOverlay(pImageExtent->GetTranslation()))
	{
		isAncestorOverlay = true;
		isValid = true;
	}

	if (isValid)
	{
		locationEnum = getLocationPropertyAtPoint(rPoint, isAncestorOverlay);
		if (SvPb::SVExtentLocationPropertyUnknown != locationEnum)
		{
			////check if move or sizing is allowed  
			if (false == isAllowedLocation(locationEnum))
			{
				locationEnum = SvPb::SVExtentLocationPropertyDisabled;
			}
		}
	}
	return {locationEnum, isAncestorOverlay};
}

std::tuple<bool, SVImageExtentClass, RECT> SVToolClass::calcChangedExtents(const SVPoint<double>& startPoint, const SVPoint<double>& stopPoint, SvPb::SVExtentLocationPropertyEnum mousePickLocation, bool isAncestorOverlay)
{
	auto toolExtent = getToolExtent();

	SvTo::SVToolExtentClass tempToolExtent;
	SVImageExtentClass extent = toolExtent.getImageExtent();
	tempToolExtent.getImageExtent() = extent;
	tempToolExtent.SetTranslation(extent.GetTranslation());
	auto l_startPoint {startPoint};
	auto l_stopPoint {stopPoint};
	if (isAncestorOverlay)
	{
		movePointFromAncestorToSourceImageCoordinate(l_startPoint);
		movePointFromAncestorToSourceImageCoordinate(l_stopPoint);
	}
	if ((SvPb::SVExtentLocationPropertyRotate == mousePickLocation ||
		mousePickLocation == toolExtent.GetLocationPropertyAt(l_startPoint)) &&
		S_OK == tempToolExtent.UpdateDraggingROI(mousePickLocation, l_startPoint, l_stopPoint))
	{
		SVImageExtentClass tempExtent = tempToolExtent.getImageExtent();
		SVPoint<long> tmpPoint;
		long width = 0;
		long height = 0;
		extent.GetExtentProperty(SvPb::SVExtentPropertyPositionPointX, tmpPoint.m_x);
		extent.GetExtentProperty(SvPb::SVExtentPropertyPositionPointY, tmpPoint.m_y);
		extent.GetExtentProperty(SvPb::SVExtentPropertyWidth, width);
		extent.GetExtentProperty(SvPb::SVExtentPropertyHeight, height);
		if (isAncestorOverlay)
		{
			movePointFromSourceToAncestorImageCoordinate(tmpPoint);
		}
		return {true, tempExtent, {tmpPoint.m_x, tmpPoint.m_y, tmpPoint.m_x + width, tmpPoint.m_y + height}};
	}

	return {false, {}, {}};
}

SvPb::EAutoSize SVToolClass::GetAutoSizeEnabled() const
{
	return (SvPb::EnableSizeAndPosition);
}

HRESULT SVToolClass::updateExtentFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eAction, long dx, long dy)
{

	m_toolExtent.UpdateFromOutputSpace(eAction, dx, dy);

	HRESULT retVal {S_OK};
	auto* pInspection = GetInspectionInterface();
	if (nullptr != pInspection)
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
	imageExtents.UpdateDataRecalculateOutput();
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
	imageExtents.UpdateDataRecalculateOutput();
	HRESULT retVal = SetImageExtent(imageExtents);
	auto* pInspection = GetInspectionInterface();
	if (S_OK == retVal && nullptr != pInspection)
	{

		retVal = pInspection->resetToolAndDependends(static_cast<SvOi::IObjectClass*>(this));
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
	return m_toolExtent.SetImageExtentAndProperty(rImageExtent);
}

const SVImageExtentClass& SVToolClass::GetImageExtent() const
{
	return m_toolExtent.getImageExtent();
}

SVImageExtentClass& SVToolClass::GetImageExtentRef()
{
	return m_toolExtent.getImageExtent();
}

SvTo::SVToolExtentClass& SVToolClass::GetToolExtent()
{
	return m_toolExtent;
}
const SVImageExtentClass* SVToolClass::GetImageExtentPtr() const
{
	return &(m_toolExtent.getImageExtent());
};




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

void SVToolClass::UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine) const
{
	__super::UpdateOverlayIDs(p_rMultiLine);

	if (SvDef::InvalidObjectId == p_rMultiLine.m_ToolID)
	{
		p_rMultiLine.m_ToolID = getObjectId();
	}
}

void SVToolClass::getOverlays(const SvIe::SVImageClass& rImage, SVExtentMultiLineStructVector& rMultiLineArray) const
{
	if (const auto* pImageExtent = GetImageExtentPtr(); nullptr != pImageExtent && pImageExtent->hasFigure())
	{
		if (isInputImage(rImage.getObjectId()))
		{
			addOverlays(rMultiLineArray, *pImageExtent);
		}
		else if (getAncestorIdForOverlay() == rImage.getObjectId() && m_pEmbeddedExtents && isTranslationValidForAncestorOverlay(pImageExtent->GetTranslation()))
		{
			SVImageExtentClass rImageExtents = *pImageExtent;
			double offsetX, offsetY;
			m_pEmbeddedExtents->m_ancestorOffsetX.GetValue(offsetX);
			m_pEmbeddedExtents->m_ancestorOffsetY.GetValue(offsetY);
			rImageExtents.SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, offsetX);
			rImageExtents.SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, offsetY);
			//[MZA][10.40][24.07.2023] by now rotation is not implemented. 
			//double offsetAngle;
			//m_pEmbeddedExtents->m_ancestorOffsetAngle.GetValue(offsetAngle);
			//rImageExtents.SetExtentProperty(SvPb::SVExtentPropertyRotationAngle, offsetAngle);
			rImageExtents.UpdateDataRecalculateOutput();
			
			addOverlays(rMultiLineArray, rImageExtents);
		}
	}
}

void SVToolClass::addOverlays(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const
{
	SVExtentMultiLineStruct multiLine;
	UpdateOverlayIDs(multiLine);
	UpdateOverlayName(multiLine, rImageExtents);
	auto* pTaskObject = dynamic_cast<SVTaskObjectClass*> (GetTool());
	if (nullptr != pTaskObject)
	{
		pTaskObject->UpdateOverlayColor(multiLine);
		pTaskObject->GetDrawInfo(multiLine);
	}
	else
	{
		UpdateOverlayColor(multiLine);
		GetDrawInfo(multiLine);
	}

	multiLine.AssignExtentFigure(rImageExtents.GetFigure(), multiLine.m_Color);
	rMultiLineArray.push_back(multiLine);

	collectOverlays(rMultiLineArray, rImageExtents);
}

void SVToolClass::UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine) const
{
	SVToolClass* pTool = m_overlayColorToolInput.getInput<SVToolClass>();
	COLORREF color = (nullptr != pTool) ? pTool->GetObjectColor() : GetObjectColor();
	p_rMultiLine.m_Color = color;
}

void SVToolClass::getOverlays(const SvIe::SVImageClass& rImage, SvPb::OverlayDesc& rOverlay) const
{
	if (isInputImage(rImage.getObjectId()))
	{
		addOverlays(rOverlay);
	}
}

void SVToolClass::addOverlays(SvPb::OverlayDesc& rOverlay) const
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
			if (m_pEmbeddedExtents)
			{
				SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentLeft, *pRect->mutable_x());
				SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentTop, *pRect->mutable_y());
				SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentWidth, *pRect->mutable_w());
				SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentHeight, *pRect->mutable_h());
			}
			setStateValueToOverlay(*pOverlay);
			collectOverlays(*pOverlay);
			break;
		}
		default:
			return;
	}
}

void SVToolClass::getAncestorImages(SvPb::GetAvailableAncestorImagesResponse& rResponse) const
{
	if (m_toolExtent.GetToolImage())
	{
		SvIe::SVImageClass* pImageParent = m_toolExtent.GetToolImage()->GetParentImage();
		if (nullptr != pImageParent && checkIfValidDependency(pImageParent))
		{
			const SVToolClass* pTool = dynamic_cast<SVToolClass*>(pImageParent->GetTool());

			if (nullptr != pTool && pTool != this)
			{
				pTool->getAncestorImages(rResponse);
			}
			auto* pEntry = rResponse.add_list();
			pEntry->set_objectname(pImageParent->getDisplayedName().c_str());
			pEntry->set_objectid(pImageParent->getObjectId());
			pEntry->set_translationtype(m_toolExtent.GetTranslation());
		}
	}
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
	auto* pImage = dynamic_cast<SvIe::SVImageClass*>(getFirstObject(objectInfo, false));
	if (nullptr != pImage)
	{
		pRetVal = &pImage->GetImageInfo();
	}

	return pRetVal;
}

bool SVToolClass::addEntryToMonitorList(SvOi::ParametersForML& retList, SvPb::EmbeddedIdEnum embeddedID) const
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

SvPb::GetAvailableAncestorImagesResponse SVToolClass::getAvailableAncestorImages() const
{
	SvPb::GetAvailableAncestorImagesResponse response;
	getAncestorImages(response);
	return response;
}

void SVToolClass::SetToolImage(uint32_t objectID)
{
	auto* pImage = dynamic_cast<SvIe::SVImageClass*> (SvOi::getObject(objectID));
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
	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = dynamic_cast<SVObjectClass*>(GetInspectionInterface());
	createStruct.m_pTool = this;

	return rChildObject.createAllObjects(createStruct);
}

void SVToolClass::connectChildObject(SVTaskObjectClass& rChildObject)
{
	SVObjectLevelCreateStruct createStruct(*this);
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

	SvPb::setValueObject(m_pwf.m_Passed, *rOverlay.mutable_passed(), true);
	SvPb::setValueObject(m_pwf.m_Failed, *rOverlay.mutable_failed(), true);
	SvPb::setValueObject(m_pwf.m_Warned, *rOverlay.mutable_warned(), true);

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

bool SVToolClass::ValidateLocal(SvStl::MessageContainerVector* pErrorMessages) const
{
	if (false == m_conditionBoolInput.IsConnected() && nullptr == m_conditionBoolInput.GetInputObjectInfo().getObject())
	{
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConditionalValue_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

void SVToolClass::movePointFromAncestorToSourceImageCoordinate(SVPoint<double>& rPoint) const
{
	if (const auto* pImageExtent = GetImageExtentPtr(); nullptr != pImageExtent && pImageExtent->hasFigure() && isTranslationValidForAncestorOverlay(pImageExtent->GetTranslation()))
	{
		double offsetX, offsetY;
		m_pEmbeddedExtents->m_ancestorOffsetX.GetValue(offsetX);
		m_pEmbeddedExtents->m_ancestorOffsetY.GetValue(offsetY);
		rPoint -= {offsetX, offsetY};
		rPoint += pImageExtent->GetFigure().m_svTopLeft;
	}
	else
	{
		Log_Assert(false);
	}
}

template<typename T>
void SVToolClass::movePointFromSourceToAncestorImageCoordinate(SVPoint<T>& rPoint) const
{
	if (const auto* pImageExtent = GetImageExtentPtr(); nullptr != pImageExtent && pImageExtent->hasFigure() && isTranslationValidForAncestorOverlay(pImageExtent->GetTranslation()))
	{
		SVPoint<double> offsetPoint;
		m_pEmbeddedExtents->m_ancestorOffsetX.GetValue(offsetPoint.m_x);
		m_pEmbeddedExtents->m_ancestorOffsetY.GetValue(offsetPoint.m_y);
		offsetPoint -= pImageExtent->GetFigure().m_svTopLeft;
		rPoint.m_x += static_cast<T>(offsetPoint.m_x);
		rPoint.m_y += static_cast<T>(offsetPoint.m_y);
	}
	else
	{
		Log_Assert(false);
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
