//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShiftTool
//* .File Name       : $Workfile:   SVShiftTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:03:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <cmath>
#include "SVShiftTool.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Add String for SVEnumerateValueObjectClass
constexpr char* g_strShiftToolEnum = _T( "None=0,Absolute=1,Reference=2" );
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVShiftTool, SVShiftToolClassGuid);

#pragma region Constructor
SVShiftTool::SVShiftTool( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
{
	LocalInitialize();
}

SVShiftTool::~SVShiftTool()
{
	LocalDestroy();
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVShiftTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_Status = SVToolClass::CreateObject(rCreateStructure);

	l_Status &= (S_OK == m_OutputImage.InitializeImage(SvOl::getInput<SvIe::SVImageClass>(m_ImageInput)) );

	constexpr UINT cAttributes = SvPb::remotelySetable | SvPb::setableOnline | SvPb::printable;
	m_SourceImageName.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );
	m_TranslationX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_TranslationY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_LearnedTranslationX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_LearnedTranslationY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_DisplacementX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_DisplacementY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_LeftResult.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_TopResult.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );

	m_ExtentRight.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_ExtentBottom.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	
	m_LearnedTranslationX.SetObjectAttributesAllowed( SvPb::extentObject, SvOi::SetAttributeType::RemoveAttribute );
	m_LearnedTranslationY.SetObjectAttributesAllowed( SvPb::extentObject, SvOi::SetAttributeType::RemoveAttribute );

	// This value object is obsolete, hide it.
	m_EnableSourceImageExtents.SetObjectAttributesAllowed( SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_isCreated = l_Status;

	SetAttributeData();

	if ( l_Status )
	{
		l_Status = (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, false, true, true));
	}
	return l_Status;
}

bool SVShiftTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool inputImageAvailable = true;
	SvOl::SVInObjectInfoStructPtrVector InputList
	{
		&m_ImageInput,
		&m_TranslationYInput,
		&m_TranslationXInput
	};

	SvOl::ValidateInputList(InputList);

	SVGUID ParentGuid;

	SetAttributeData();

	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_ImageInput);
	if( nullptr != pInputImage )
	{
		ParentGuid = pInputImage->GetUniqueObjectID();

		//Set input name to source image name to display it in result picker
		m_SourceImageName.SetValue( pInputImage->GetCompleteName() );

		m_OutputImage.UpdateImage(ParentGuid, pInputImage->GetImageInfo());
	}
	else
	{
		inputImageAvailable = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	return SVToolClass::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages) && inputImageAvailable;
}

HRESULT SVShiftTool::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_toolExtent.UpdateExtentToParentExtents(NewExtent );

	if( S_OK == l_hrOk  )
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

SvVol::SVStringValueObjectClass* SVShiftTool::GetInputImageNames()
{
	return &m_SourceImageName;
}

SVToolClass* SVShiftTool::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool SVShiftTool::DoesObjectHaveExtents() const
{
	return true;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool SVShiftTool::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SvIe::SVImageClass* pImage = SvOl::getInput<SvIe::SVImageClass>(m_ImageInput);
	if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}
	return Result;
}

bool SVShiftTool::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool Result = __super::onRun( rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages);
	long Mode;

	if( Result )
	{
		if( S_OK != m_evoShiftMode.GetValue( Mode ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (Result)
	{
		SvTrc::IImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
		if (nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
		{
			SVMatroxBufferInterface::ClearBuffer(pOutputImageBuffer->getHandle()->GetBuffer(), 0);
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}

		double dLeft(0.0);
		double dTop(0.0);

		if (S_OK != m_ExtentLeft.GetValue(dLeft) || !std::isfinite(dLeft)  || S_OK != m_ExtentTop.GetValue(dTop) || !std::isfinite(dTop))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}

		if (Result)
		{
			long LearnedTranslationX(0L);
			long LearnedTranslationY(0L);
			double dInputTranslationX(0.0);
			double dInputTranslationY(0.0);

			if ((SV_SHIFT_ENUM::SV_SHIFT_REFERENCE == Mode) || (SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE == Mode))
			{
				SvVol::SVDoubleValueObjectClass* pTranslationXInput = GetTranslationXInput(true);
				SvVol::SVDoubleValueObjectClass* pTranslationYInput = GetTranslationYInput(true);
				double Value(0.0);
				Result = Result && (S_OK == m_LearnedTranslationX.GetValue(Value));
				LearnedTranslationX = static_cast<long> (Value);
				Result = Result && (S_OK == m_LearnedTranslationY.GetValue(Value));
				LearnedTranslationY = static_cast<long> (Value);

				Result = Result &&  (nullptr != pTranslationXInput && S_OK == pTranslationXInput->GetValue(dInputTranslationX) );
				Result = Result && std::isfinite(dInputTranslationX);
				Result = Result && (nullptr != pTranslationYInput && S_OK == pTranslationYInput->GetValue(dInputTranslationY) );
				Result = Result && std::isfinite(dInputTranslationY);
				

				if (!Result)
				{
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
						pErrorMessages->push_back(Msg);
					}
				}
			}

			if ((Result) && ((Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE) || (Mode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE)))
			{
				long lInputTranslationX = std::lround(dInputTranslationX);
				long lInputTranslationY = std::lround(dInputTranslationY);

				if (Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE)
				{
					double dDisplacementX = static_cast<double> (lInputTranslationX - LearnedTranslationX);
					double dDisplacementY = static_cast<double> (lInputTranslationY - LearnedTranslationY);
					double dNewLeft = dLeft + dDisplacementX;
					double dNewTop = dTop + dDisplacementY;

					m_TranslationX.SetValue(lInputTranslationX);
					m_TranslationY.SetValue(lInputTranslationY);
					m_DisplacementX.SetValue(dDisplacementX);
					m_DisplacementY.SetValue(dDisplacementY);
					m_LeftResult.SetValue(dNewLeft);
					m_TopResult.SetValue(dNewTop);
				}
				else //Absolute Mode
				{
					double dDisplacementX = static_cast<double> (lInputTranslationX);
					double dDisplacementY = static_cast<double> (lInputTranslationY);

					m_LearnedTranslationX.SetValue(0.0);
					m_LearnedTranslationY.SetValue(0.0);

					m_TranslationX.SetValue(lInputTranslationX);
					m_TranslationY.SetValue(lInputTranslationY);
					m_DisplacementX.SetValue(dDisplacementX);
					m_DisplacementY.SetValue(dDisplacementY);
					m_ExtentLeft.SetValue(0.0);
					m_ExtentTop.SetValue(0.0);

					m_LeftResult.SetValue(dDisplacementX);
					m_TopResult.SetValue(dDisplacementY);
				}
			}
			else
			{
				m_TranslationX.SetValue(0L);
				m_TranslationY.SetValue(0L);
				m_DisplacementX.SetValue(0.0);
				m_DisplacementY.SetValue(0.0);
				m_LeftResult.SetValue(dLeft);
				m_TopResult.SetValue(dTop);
			}

			//This update call is required for the image extents which may have changed above
			updateImageExtent();
			if (GetImageExtent().hasFigure())
			{
				const SVExtentFigureStruct& rFigure = GetImageExtent().GetFigure();
				m_ExtentRight.SetValue(rFigure.m_svBottomRight.m_x);
				m_ExtentBottom.SetValue(rFigure.m_svBottomRight.m_y);
			}
		}

		if (Result)
		{
			SvIe::SVImageClass* pImageInput = SvOl::getInput<SvIe::SVImageClass>(m_ImageInput, true);
			SvTrc::IImagePtr pInputImageBuffer = pImageInput->getImageReadOnly(rRunStatus.m_triggerRecord.get());

			double l_OffsetX = 0.0;
			double l_OffsetY = 0.0;

			Result = Result && (S_OK == m_LeftResult.GetValue(l_OffsetX) && std::isfinite(l_OffsetX));
			Result = Result && (S_OK == m_TopResult.GetValue(l_OffsetY) && std::isfinite(l_OffsetY));
			Result = Result && (nullptr != pImageInput);
			Result = Result && (nullptr != pInputImageBuffer);
			Result = Result && !(pInputImageBuffer->isEmpty());

			if (Result)
			{
				//[MEC][7.40][11.10.2016] this correction  was inserted with SVO1017 but for  mode==SV_SHIFT_NONE the offset is counted twice
				if ((Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE) || (Mode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE))
				{
					m_OutputImage.SetTranslationOffset(l_OffsetX, l_OffsetY);
				}

				HRESULT MatroxCode = SVMatroxBufferInterface::CopyBuffer(pOutputImageBuffer->getHandle()->GetBuffer(), pInputImageBuffer->getHandle()->GetBuffer(), static_cast<long>(-l_OffsetX), static_cast<long>(-l_OffsetY));

				if (S_OK != MatroxCode)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
						pErrorMessages->push_back(Msg);
					}
				}
			}
			else
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}
		}

		if (Result)
		{
			rRunStatus.SetPassed();
		}
		else
		{
			rRunStatus.SetFailed();
			rRunStatus.SetInvalid();
		}
	}
	return Result;
}

SvVol::SVDoubleValueObjectClass* SVShiftTool::GetTranslationXInput(bool bRunMode /*= false*/) const
{
	return SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_TranslationXInput, bRunMode);
}

SvVol::SVDoubleValueObjectClass* SVShiftTool::GetTranslationYInput(bool bRunMode /*= false*/) const
{
	return SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_TranslationYInput, bRunMode);
}

void SVShiftTool::addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const
{
	auto* pOverlay = rOverlay.add_overlays();
	pOverlay->set_name(GetName());
	SvPb::SetGuidInProtoBytes(pOverlay->mutable_guid(), GetUniqueObjectID());
	pOverlay->mutable_color()->set_trpos(m_statusColor.getTrPos() + 1);
	pOverlay->set_displaybounding(true);
	auto* pBoundingBox = pOverlay->mutable_boundingshape();
	auto* pRect = pBoundingBox->mutable_rect();
	setValueObject(m_LeftResult, *pRect->mutable_x(), true);
	setValueObject(m_TopResult, *pRect->mutable_y(), true);
	setValueObject(m_ExtentWidth, *pRect->mutable_w());
	setValueObject(m_ExtentHeight, *pRect->mutable_h());
	collectOverlays(pImage, *pOverlay);
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVShiftTool::LocalInitialize()
{
	m_canResizeToParent = true;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVShiftToolObjectType;

	SVObjectClass* l_pObject = nullptr;

	SVObjectManagerClass::Instance().ConstructObject( SVEvaluateTranslationXClassGuid, l_pObject );

	if( nullptr != l_pObject )
	{
		l_pObject->SetObjectOwner( this );

		AddFriend( l_pObject->GetUniqueObjectID() );
	}

	SVObjectManagerClass::Instance().ConstructObject( SVEvaluateTranslationYClassGuid, l_pObject );

	if( nullptr != l_pObject )
	{
		l_pObject->SetObjectOwner( this );

		AddFriend( l_pObject->GetUniqueObjectID() );
	}

	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, false, true, true);

	// Identify our input type needs...
	m_ImageInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType); //this is not a problem even if color images will be used since m_ImageInput will be updated in ResetObject() anyway
	m_ImageInput.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_ImageInput, _T( "ShiftToolImage" ) );

	m_TranslationXInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateTranslationXResultObjectGuid);
	m_TranslationXInput.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_TranslationXInput, _T( "ShiftToolTranslationX" ) );

	m_TranslationYInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SVOutputEvaluateTranslationYResultObjectGuid);
	m_TranslationYInput.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_TranslationYInput, _T( "ShiftToolTranslationY" ) );

	//Special type names for extents
	m_LeftResult.SetTypeName( _T("Extent X") );
	m_TopResult.SetTypeName( _T("Extent Y") );
	//Register Embedded Objects
	RegisterEmbeddedObject( &m_evoShiftMode, SVShiftToolModeGuid, IDS_OBJECTNAME_SHIFT_TOOL_MODE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_SourceImageName, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_TranslationX, SVTranslationXObjectGuid, IDS_OBJECTNAME_TRANSLATION_X_RESULT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_TranslationY, SVTranslationYObjectGuid, IDS_OBJECTNAME_TRANSLATION_Y_RESULT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_LearnedTranslationX, SVShiftToolReferenceXObjectGuid, IDS_OBJECTNAME_SHIFTTOOL_REFERENCE_X, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_LearnedTranslationY, SVShiftToolReferenceYObjectGuid, IDS_OBJECTNAME_SHIFTTOOL_REFERENCE_Y, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_DisplacementX, SVImageTransformDisplacementXGuid, IDS_TRANSFORM_DISPLACEMENTX, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_DisplacementY, SVImageTransformDisplacementYGuid, IDS_TRANSFORM_DISPLACEMENTY, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_LeftResult, SVLeftResultObjectGuid, IDS_OBJECTNAME_LEFT_RESULT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_TopResult, SVTopResultObjectGuid, IDS_OBJECTNAME_TOP_RESULT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_EnableSourceImageExtents, SVShiftToolEnableSourceImageExtentsGuid, IDS_OBJECTNAME_SHIFTTOOL_ENABLE_SOURCE_IMAGE_EXTENTS, false, SvOi::SVResetItemTool);

	m_LearnedTranslationX.SetObjectAttributesAllowed( SvPb::extentObject, SvOi::SetAttributeType::RemoveAttribute );
	m_LearnedTranslationY.SetObjectAttributesAllowed( SvPb::extentObject, SvOi::SetAttributeType::RemoveAttribute );

	//Set Extent Information
	m_toolExtent.SetExtentObject( SvPb::SVExtentPropertyTranslationOffsetX, &m_DisplacementX );
	m_toolExtent.SetExtentObject( SvPb::SVExtentPropertyTranslationOffsetY, &m_DisplacementY );

	//Set Default Values
	m_ExtentTop.SetDefaultValue( 10, true );
	m_ExtentLeft.SetDefaultValue( 10, true );
	m_ExtentWidth.SetDefaultValue( 100, true );
	m_ExtentHeight.SetDefaultValue( 100, true );

	m_TranslationX.SetDefaultValue( 0, true );
	m_TranslationX.setSaveValueFlag(false);
	m_TranslationY.SetDefaultValue( 0, true );
	m_TranslationY.setSaveValueFlag(false);

	m_LearnedTranslationX.SetDefaultValue( 0, true );
	m_LearnedTranslationY.SetDefaultValue( 0, true );

	m_DisplacementX.SetDefaultValue( 0, true );
	m_DisplacementX.setSaveValueFlag(false);
	m_DisplacementY.SetDefaultValue( 0, true );
	m_DisplacementY.setSaveValueFlag(false);

	m_LeftResult.SetDefaultValue( 10, true );
	m_LeftResult.setSaveValueFlag(false);
	m_TopResult.SetDefaultValue( 10, true );
	m_TopResult.setSaveValueFlag(false);

	m_EnableSourceImageExtents.SetDefaultValue(true, true); // this embedded value object is not used anymore

	m_OutputImage.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	m_evoShiftMode.SetEnumTypes(g_strShiftToolEnum);
	m_evoShiftMode.SetDefaultValue(SV_SHIFT_NONE, true);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

void SVShiftTool::LocalDestroy()
{
}

void SVShiftTool::SetAttributeData()
{
	long l_Mode = SV_SHIFT_NONE;
	HRESULT hrRes = m_evoShiftMode.GetValue(l_Mode);
	if ( S_OK == hrRes )
	{
		SvIe::SVExtentPropertyInfoStruct info;
		if ( SV_SHIFT_ENUM::SV_SHIFT_REFERENCE == l_Mode )
		{
			//Turn Extent data on
			//Set Extent Property Info so items show up in the Adjust Tool Position Dlg

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetX, info);
			info.bHidden = false;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetX, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetY, info);
			info.bHidden = false;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetY, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
			info.bHidden = false;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX ,info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
			info.bHidden = false;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
		}
		else if ( SV_SHIFT_ENUM::SV_SHIFT_NONE == l_Mode )
		{
			//Set Extent Property Info so items don't show up in the Adjust Tool Position Dlg
			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetX, info);
			info.bHidden = true;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetX, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetY, info);
			info.bHidden = true;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetY, info);

			//Set Extent Property Info so items show up in the Adjust Tool Position Dlg
			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
			info.bHidden = false;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
			info.bHidden = false;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
		}
		else
		{
			//Set Extent Property Info so items don't show up in the Adjust Tool Position Dlg
			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetX, info);
			info.bHidden = true;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetX, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetY, info);
			info.bHidden = true;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyTranslationOffsetY, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
			info.bHidden = true;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);

			m_toolExtent.GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
			info.bHidden = true;
			m_toolExtent.SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);

			//set default value to be 0, 0
			m_ExtentLeft.SetDefaultValue(0, true);
			m_ExtentTop.SetDefaultValue(0, true);
		}
	}
}

SvPb::EAutoSize SVShiftTool::GetAutoSizeEnabled() const
{
	long shiftMode;
	m_evoShiftMode.GetValue(shiftMode);
	if ( SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE == shiftMode )
	{
		return SvPb::EnableSize;
	}
	return SvPb::EnableAll;
}

bool SVShiftTool::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result = true;
	long mode = SV_SHIFT_NONE;
	m_evoShiftMode.GetValue(mode);

	if ( nullptr == SvOl::getInput<SvIe::SVImageClass>(m_ImageInput))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	if ( SV_SHIFT_NONE != mode && (nullptr == GetTranslationXInput() || nullptr == GetTranslationYInput()) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}
#pragma endregion Private Methods

} //namespace SvTo
