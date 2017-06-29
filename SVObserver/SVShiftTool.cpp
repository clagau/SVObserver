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
//Moved to precompiled header: #include <boost/math/special_functions/round.hpp>
#include "SVShiftTool.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ToolSizeAdjustTask.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVShiftTool, SVShiftToolClassGuid );

// Add String for SVEnumerateValueObjectClass
static const LPCTSTR g_strShiftToolEnum = _T( "None=0,Absolute=1,Reference=2" );

#pragma endregion Declarations

#pragma region Constructor
SVShiftTool::SVShiftTool( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	LocalInitialize();
}

SVShiftTool::~SVShiftTool()
{
	LocalDestroy();
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL SVShiftTool::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_Status = SVToolClass::CreateObject( PCreateStructure );

	l_Status &= (S_OK == m_OutputImage.InitializeImage( GetImageInput() ) );

	const UINT cAttributes = SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_PRINTABLE;
	m_SourceImageName.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	m_TranslationX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_TranslationY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_LearnedTranslationX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_LearnedTranslationY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_DisplacementX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_DisplacementY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_LeftResult.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	m_TopResult.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );

	m_ExtentRight.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ExtentBottom.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	
	m_LearnedTranslationX.SetObjectAttributesAllowed( SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::RemoveAttribute );
	m_LearnedTranslationY.SetObjectAttributesAllowed( SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::RemoveAttribute );

	// This value object is obsolete, hide it.
	m_EnableSourceImageExtents.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	m_isCreated = l_Status;

	SetAttributeData();

	if ( l_Status )
	{
		l_Status = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, false, true, true)); 
	}
	return l_Status;
}

bool SVShiftTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	SVGUID l_ParentGuid;
	SVImageClass* l_pInputImage = GetImageInput();

	SetAttributeData();

	if( nullptr != l_pInputImage )
	{
		l_ParentGuid = l_pInputImage->GetUniqueObjectID();

		//Set input name to source image name to display it in result picker
		m_SourceImageName.SetValue( l_pInputImage->GetCompleteName() );
	}

	m_OutputImage.UpdateImage( l_ParentGuid );
	
	return SVToolClass::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
}

HRESULT SVShiftTool::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_svToolExtent.UpdateExtentToParentExtents(NewExtent );

	if( S_OK == l_hrOk  )
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

SVStringValueObjectClass* SVShiftTool::GetInputImageNames()
{
	return &m_SourceImageName;
}

HRESULT SVShiftTool::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage && p_psvImage == GetImageInput() )
	{
		l_hrOk = S_OK;
	}
	return l_hrOk;
}

SVTaskObjectClass* SVShiftTool::GetObjectAtPoint( const SVExtentPointStruct &p_rPoint )
{
	SVImageExtentClass l_Extents;

	SVTaskObjectClass* l_pObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_Extents )  &&
	    l_Extents.GetLocationPropertyAt( p_rPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_pObject = this;
	}
	return l_pObject;
}

bool SVShiftTool::DoesObjectHaveExtents() const
{
	return true;
}
#pragma endregion Public Methods

#pragma region Protected Methods
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
		SVSmartHandlePointer OutImageHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		if (m_OutputImage.SetImageHandleIndex(rRunStatus.Images) && m_OutputImage.GetImageHandle(OutImageHandle) &&
			!OutImageHandle.empty() && (S_OK == OutImageHandle->GetData(l_OutMilHandle)) && !l_OutMilHandle.empty())
		{
			SVMatroxBufferInterface::ClearBuffer(l_OutMilHandle.GetBuffer(), 0);
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

		if (S_OK != m_ExtentLeft.GetValue(dLeft) || S_OK != m_ExtentTop.GetValue(dTop))
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
			BOOL PerformTranslation(false);
			long LearnedTranslationX(0L);
			long LearnedTranslationY(0L);
			double dInputTranslationX(0.0);
			double dInputTranslationY(0.0);

			if ((SV_SHIFT_ENUM::SV_SHIFT_REFERENCE == Mode) || (SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE == Mode))
			{
				SVDoubleValueObjectClass* l_pTranslationXInput = GetTranslationXInput();
				SVDoubleValueObjectClass* l_pTranslationYInput = GetTranslationYInput();
				double Value(0.0);
				Result = Result && (S_OK == m_LearnedTranslationX.GetValue(Value));
				LearnedTranslationX = static_cast<long> (Value);
				Result = Result && (S_OK == m_LearnedTranslationY.GetValue(Value));
				LearnedTranslationY = static_cast<long> (Value);

				Result = Result && (nullptr != l_pTranslationXInput && S_OK == l_pTranslationXInput->GetValue(dInputTranslationX));
				Result = Result && (nullptr != l_pTranslationYInput && S_OK == l_pTranslationYInput->GetValue(dInputTranslationY));

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
				long lInputTranslationX = boost::math::lround(dInputTranslationX);
				long lInputTranslationY = boost::math::lround(dInputTranslationY);

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

			SVImageExtentClass l_svExtents;

			if (S_OK == GetImageExtent(l_svExtents))
			{
				SVExtentFigureStruct l_svFigure;

				if (S_OK == l_svExtents.GetFigure(l_svFigure))
				{
					m_ExtentRight.SetValue(l_svFigure.m_svBottomRight.m_dPositionX);
					m_ExtentBottom.SetValue(l_svFigure.m_svBottomRight.m_dPositionY);
				}
			}
		}

		if (Result)
		{
			SVSmartHandlePointer InImageHandle;
			SVImageBufferHandleImage l_InMilHandle;

			SVImageClass* l_pImageInput = GetImageInput();

			double l_OffsetX = 0.0;
			double l_OffsetY = 0.0;

			Result = Result && (S_OK == m_LeftResult.GetValue(l_OffsetX));
			Result = Result && (S_OK == m_TopResult.GetValue(l_OffsetY));
			Result = Result && (nullptr != l_pImageInput);
			Result = Result && (l_pImageInput->GetImageHandle(InImageHandle));
			Result = Result && !(InImageHandle.empty());
			Result = Result && (S_OK == InImageHandle->GetData(l_InMilHandle));
			Result = Result && !(l_InMilHandle.empty());

			if (Result)
			{
				//[MEC][7.40][11.10.2016] this correction  was inserted with SVO1017 but for  mode==SV_SHIFT_NONE the offset is counted twice
				if ((Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE) || (Mode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE))
				{
					m_OutputImage.SetTranslationOffset(l_OffsetX, l_OffsetY);
				}

				HRESULT MatroxCode = SVMatroxBufferInterface::CopyBuffer(l_OutMilHandle.GetBuffer(), l_InMilHandle.GetBuffer(), static_cast<long>(-l_OffsetX), static_cast<long>(-l_OffsetY));

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

			SetInvalid();
		}
	}
	return Result;
}

SVImageClass* SVShiftTool::GetImageInput() const
{
	SVImageClass* l_pImage = nullptr;

	if( m_ImageInput.IsConnected() )
	{
		l_pImage = dynamic_cast< SVImageClass* >( m_ImageInput.GetInputObjectInfo().m_pObject );
	}
	return l_pImage;
}

SVDoubleValueObjectClass* SVShiftTool::GetTranslationXInput() const
{
	SVDoubleValueObjectClass* l_pValue = nullptr;

	if( m_TranslationXInput.IsConnected() )
	{
		l_pValue = dynamic_cast< SVDoubleValueObjectClass* >( m_TranslationXInput.GetInputObjectInfo().m_pObject );
	}
	return l_pValue;
}

SVDoubleValueObjectClass* SVShiftTool::GetTranslationYInput() const
{
	SVDoubleValueObjectClass* l_pValue = nullptr;

	if( m_TranslationYInput.IsConnected() )
	{
		l_pValue = dynamic_cast< SVDoubleValueObjectClass* >( m_TranslationYInput.GetInputObjectInfo().m_pObject );
	}
	return l_pValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVShiftTool::LocalInitialize()
{
	m_canResizeToParent = true;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVShiftToolObjectType;

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

	ToolSizeAdjustTask::AddToFriendlist(this, false, true, true);

	// Identify our input type needs...
	m_ImageInput.SetInputObjectType( SVImageObjectType );
	m_ImageInput.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_ImageInput, _T( "ShiftToolImage" ) );

	m_TranslationXInput.SetInputObjectType( SVOutputEvaluateTranslationXResultObjectGuid, SVDoubleValueObjectType );
	m_TranslationXInput.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_TranslationXInput, _T( "ShiftToolTranslationX" ) );

	m_TranslationYInput.SetInputObjectType( SVOutputEvaluateTranslationYResultObjectGuid, SVDoubleValueObjectType );
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

	m_LearnedTranslationX.SetObjectAttributesAllowed( SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::RemoveAttribute );
	m_LearnedTranslationY.SetObjectAttributesAllowed( SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::RemoveAttribute );

	//Set Extent Information
	m_svToolExtent.SetExtentObject( SVExtentPropertyTranslationOffsetX, &m_DisplacementX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyTranslationOffsetY, &m_DisplacementY );

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

	m_OutputImage.InitializeImage( SvOi::SVImageTypeEnum::SVImageTypePhysical );

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
		SVExtentPropertyInfoStruct info;
		if ( SV_SHIFT_ENUM::SV_SHIFT_REFERENCE == l_Mode )
		{
			//Turn Extent data on
			//Set Extent Property Info so items show up in the Adjust Tool Position Dlg

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX, info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY, info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX, info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointX ,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY, info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointY, info);
		}
		else if ( SV_SHIFT_ENUM::SV_SHIFT_NONE == l_Mode )
		{
			//Set Extent Property Info so items don't show up in the Adjust Tool Position Dlg
			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX, info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY, info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY, info);

			//Set Extent Property Info so items show up in the Adjust Tool Position Dlg
			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX, info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointX, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY, info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointY, info);
		}
		else
		{
			//Set Extent Property Info so items don't show up in the Adjust Tool Position Dlg
			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX, info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY, info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX, info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointX, info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY, info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointY, info);

			//set default value to be 0, 0
			m_ExtentLeft.SetDefaultValue(0, true);
			m_ExtentTop.SetDefaultValue(0, true);
		}
	}
}

EAutoSize SVShiftTool::GetAutoSizeEnabled()
{
	long shiftMode;
	m_evoShiftMode.GetValue(shiftMode);
	if ( SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE == shiftMode )
	{
		return EnableSize;
	}
	return EnableAll;
}

bool SVShiftTool::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result = true;
	long mode = SV_SHIFT_NONE;
	m_evoShiftMode.GetValue(mode);

	if ( nullptr == GetImageInput() )
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

