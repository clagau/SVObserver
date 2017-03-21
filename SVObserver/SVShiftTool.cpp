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

	m_SourceImageName.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE );
	m_TranslationX.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_TranslationY.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_LearnedTranslationX.ObjectAttributesAllowedRef() |= ( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE );
	m_LearnedTranslationY.ObjectAttributesAllowedRef() |= ( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE );
	m_DisplacementX.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_DisplacementY.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_LeftResult.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_TopResult.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );

	m_ExtentRight.ObjectAttributesAllowedRef() &= (~SV_PRINTABLE);
	m_ExtentBottom.ObjectAttributesAllowedRef() &= (~SV_PRINTABLE);
	
	m_LearnedTranslationX.ObjectAttributesAllowedRef() &= (~SV_EXTENT_OBJECT);
	m_LearnedTranslationY.ObjectAttributesAllowedRef() &= (~SV_EXTENT_OBJECT);

	// This value object is obsolete, hide it.
	m_EnableSourceImageExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;

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
		m_SourceImageName.SetValue( 0/*Static value, this parameter will not used*/, SVString( l_pInputImage->GetCompleteName() ) );
	}

	m_OutputImage.UpdateImage( l_ParentGuid );
	
	return SVToolClass::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
}

HRESULT SVShiftTool::SetImageExtentToParent( unsigned long p_ulIndex )
{
	SVImageExtentClass l_NewExtent;
	HRESULT l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( p_ulIndex, l_NewExtent );

	if( S_OK == l_hrOk  )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

SVStaticStringValueObjectClass* SVShiftTool::GetInputImageNames()
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
bool SVShiftTool::onRun( SVRunStatusClass& p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool l_Status = __super::onRun( p_rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages);
	long l_Mode;

	if( l_Status )
	{
		if( S_OK != m_evoShiftMode.GetValue( l_Mode ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if ( l_Status )
	{
		if (!p_rRunStatus.IsDisabled() && !p_rRunStatus.IsDisabledByCondition())
		{
			SVSmartHandlePointer OutImageHandle;
			SVImageBufferHandleImage l_OutMilHandle;

			if ( m_OutputImage.SetImageHandleIndex( p_rRunStatus.Images ) && m_OutputImage.GetImageHandle( OutImageHandle ) &&
					!OutImageHandle.empty() && ( S_OK == OutImageHandle->GetData( l_OutMilHandle ) ) && !l_OutMilHandle.empty()  )
			{
				SVMatroxBufferInterface::ClearBuffer( l_OutMilHandle.GetBuffer(), 0 );
			}
			else
			{
				l_Status = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}

			double l_Left = 0.0;
			double l_Top = 0.0;

			if ( S_OK != m_ExtentLeft.GetValue( l_Left ) || S_OK != m_ExtentTop.GetValue( l_Top ) )
			{
				l_Status = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}

			if( l_Status )
			{
				BOOL l_PerformTranslation = false;
				long l_LearnedTranslationX = 0;
				long l_LearnedTranslationY = 0;
				double fInputTranslationX = 0.0;
				double fInputTranslationY = 0.0;

				if ( ( SV_SHIFT_ENUM::SV_SHIFT_REFERENCE == l_Mode ) || ( SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE == l_Mode ) )
				{
					SVDoubleValueObjectClass* l_pTranslationXInput = GetTranslationXInput();
					SVDoubleValueObjectClass* l_pTranslationYInput = GetTranslationYInput();

					l_Status = l_Status && ( S_OK == m_LearnedTranslationX.GetValue( l_LearnedTranslationX ) );
					l_Status = l_Status && ( S_OK == m_LearnedTranslationY.GetValue( l_LearnedTranslationY ) );

					l_Status = l_Status && ( nullptr != l_pTranslationXInput && S_OK == l_pTranslationXInput->GetValue( fInputTranslationX ) );
					l_Status = l_Status && ( nullptr != l_pTranslationYInput && S_OK == l_pTranslationYInput->GetValue( fInputTranslationY ) );

					if (!l_Status)
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}

				if ( (l_Status ) && ( (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE) || (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE) ) )
				{
					long nInputTranslationX = boost::math::lround(fInputTranslationX);
					long nInputTranslationY = boost::math::lround(fInputTranslationY);

					if ( l_Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE )
					{
						long l_DisplacementX = nInputTranslationX - l_LearnedTranslationX;
						long l_DisplacementY = nInputTranslationY - l_LearnedTranslationY;
						long l_NewLeft = static_cast< long >( l_Left ) + l_DisplacementX;
						long l_NewTop = static_cast< long >( l_Top ) + l_DisplacementY;

						m_TranslationX.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationX );
						m_TranslationY.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationY );
						m_DisplacementX.SetValue( p_rRunStatus.m_lResultDataIndex, l_DisplacementX );
						m_DisplacementY.SetValue( p_rRunStatus.m_lResultDataIndex, l_DisplacementY );	
						m_LeftResult.SetValue( p_rRunStatus.m_lResultDataIndex, l_NewLeft );
						m_TopResult.SetValue( p_rRunStatus.m_lResultDataIndex, l_NewTop );
					}
					else //Absolute Mode
					{
						m_LearnedTranslationX.SetValue( p_rRunStatus.m_lResultDataIndex, 0.0);
						m_LearnedTranslationY.SetValue( p_rRunStatus.m_lResultDataIndex, 0.0);

						m_TranslationX.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationX );
						m_TranslationY.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationY );
						m_DisplacementX.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationX);
						m_DisplacementY.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationY );
						m_ExtentLeft.SetValue(p_rRunStatus.m_lResultDataIndex, 0.0);
						m_ExtentTop.SetValue(p_rRunStatus.m_lResultDataIndex, 0.0);

						m_LeftResult.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationX  );
						m_TopResult.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationY );
					}
				}
				else
				{
					m_TranslationX.SetValue( p_rRunStatus.m_lResultDataIndex, 0.0 );
					m_TranslationY.SetValue( p_rRunStatus.m_lResultDataIndex, 0.0 );
					m_DisplacementX.SetValue( p_rRunStatus.m_lResultDataIndex, 0.0 );
					m_DisplacementY.SetValue( p_rRunStatus.m_lResultDataIndex, 0.0 );
					m_LeftResult.SetValue( p_rRunStatus.m_lResultDataIndex, l_Left );
					m_TopResult.SetValue( p_rRunStatus.m_lResultDataIndex, l_Top );
				}

				SVImageExtentClass l_svExtents;

				if ( S_OK == GetImageExtent( l_svExtents ) )
				{
					SVExtentFigureStruct l_svFigure;

					if ( S_OK == l_svExtents.GetFigure( l_svFigure )  )
					{
						long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

						m_ExtentRight.SetValue( p_rRunStatus.m_lResultDataIndex, l_lValue );
						
						l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionY);

						m_ExtentBottom.SetValue( p_rRunStatus.m_lResultDataIndex, l_lValue );
					}
				}
			}
			else
			{
				m_TranslationX.CopyLastSetValue( p_rRunStatus.m_lResultDataIndex );
				m_TranslationY.CopyLastSetValue( p_rRunStatus.m_lResultDataIndex );
				m_DisplacementX.CopyLastSetValue( p_rRunStatus.m_lResultDataIndex );
				m_DisplacementY.CopyLastSetValue( p_rRunStatus.m_lResultDataIndex );
				m_LeftResult.CopyLastSetValue( p_rRunStatus.m_lResultDataIndex );
				m_TopResult.CopyLastSetValue( p_rRunStatus.m_lResultDataIndex );
			}

			if( l_Status )
			{
				SVSmartHandlePointer InImageHandle;
				SVImageBufferHandleImage l_InMilHandle;

				SVImageClass* l_pImageInput = GetImageInput();

				double l_OffsetX = 0.0;
				double l_OffsetY = 0.0;

				l_Status = l_Status && ( S_OK == m_LeftResult.GetValue( l_OffsetX ) );
				l_Status = l_Status && ( S_OK == m_TopResult.GetValue( l_OffsetY ) );
				l_Status = l_Status && ( nullptr != l_pImageInput );
				l_Status = l_Status && ( l_pImageInput->GetImageHandle( InImageHandle ) );
				l_Status = l_Status && !( InImageHandle.empty() );
				l_Status = l_Status && ( S_OK == InImageHandle->GetData( l_InMilHandle ) );
				l_Status = l_Status && !( l_InMilHandle.empty() );

				if( l_Status )
				{
					//[MEC][7.40][11.10.2016] this correction  was inserted with SVO1017 but for  mode==SV_SHIFT_NONE the offset is counted twice
					if( (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE) || (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE) )
					{
						m_OutputImage.SetTranslationOffset(l_OffsetX, l_OffsetY);
					}

					SVMatroxImageInterface::SVStatusCode l_Code;

					l_Code = SVMatroxBufferInterface::CopyBuffer( l_OutMilHandle.GetBuffer(), l_InMilHandle.GetBuffer(), static_cast< long >( -l_OffsetX ), static_cast< long >( -l_OffsetY ) );

					if ( SVMEE_STATUS_OK != l_Code )
					{
						l_Status = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}
				else
				{
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
				}
			}

			if( l_Status )
			{
				p_rRunStatus.SetPassed();
			}
			else
			{
				p_rRunStatus.SetFailed();
				p_rRunStatus.SetInvalid();

				SetInvalid();
			}
		}
		else
		{
			m_OutputImage.CopyImageTo(p_rRunStatus.Images);
		}
	}
	return l_Status;
}

SVImageClass* SVShiftTool::GetImageInput() const
{
	SVImageClass* l_pImage = nullptr;

	if( m_ImageInput.IsConnected() )
	{
		l_pImage = dynamic_cast< SVImageClass* >( m_ImageInput.GetInputObjectInfo().PObject );
	}
	return l_pImage;
}

SVDoubleValueObjectClass* SVShiftTool::GetTranslationXInput() const
{
	SVDoubleValueObjectClass* l_pValue = nullptr;

	if( m_TranslationXInput.IsConnected() )
	{
		l_pValue = dynamic_cast< SVDoubleValueObjectClass* >( m_TranslationXInput.GetInputObjectInfo().PObject );
	}
	return l_pValue;
}

SVDoubleValueObjectClass* SVShiftTool::GetTranslationYInput() const
{
	SVDoubleValueObjectClass* l_pValue = nullptr;

	if( m_TranslationYInput.IsConnected() )
	{
		l_pValue = dynamic_cast< SVDoubleValueObjectClass* >( m_TranslationYInput.GetInputObjectInfo().PObject );
	}
	return l_pValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVShiftTool::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVShiftToolObjectType;

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

	//Register Embedded Objects
	RegisterEmbeddedObject( &m_evoShiftMode, SVShiftToolModeGuid, IDS_OBJECTNAME_SHIFT_TOOL_MODE, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_SourceImageName, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_TranslationX, SVTranslationXObjectGuid, IDS_OBJECTNAME_TRANSLATION_X_RESULT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_TranslationY, SVTranslationYObjectGuid, IDS_OBJECTNAME_TRANSLATION_Y_RESULT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_LearnedTranslationX, SVShiftToolReferenceXObjectGuid, IDS_OBJECTNAME_SHIFTTOOL_REFERENCE_X, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_LearnedTranslationY, SVShiftToolReferenceYObjectGuid, IDS_OBJECTNAME_SHIFTTOOL_REFERENCE_Y, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_DisplacementX, SVImageTransformDisplacementXGuid, IDS_TRANSFORM_DISPLACEMENTX, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_DisplacementY, SVImageTransformDisplacementYGuid, IDS_TRANSFORM_DISPLACEMENTY, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_LeftResult, SVLeftResultObjectGuid, IDS_OBJECTNAME_LEFT_RESULT, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_TopResult, SVTopResultObjectGuid, IDS_OBJECTNAME_TOP_RESULT, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_EnableSourceImageExtents, SVShiftToolEnableSourceImageExtentsGuid, IDS_OBJECTNAME_SHIFTTOOL_ENABLE_SOURCE_IMAGE_EXTENTS, false, SVResetItemTool);

	m_LearnedTranslationX.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_LearnedTranslationY.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;

	//Set Extent Information
	m_svToolExtent.SetExtentObject( SVExtentPropertyTranslationOffsetX, &m_DisplacementX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyTranslationOffsetY, &m_DisplacementY );

	//Set Default Values
	m_ExtentTop.SetDefaultValue( 10, true );
	m_ExtentLeft.SetDefaultValue( 10, true );
	m_ExtentWidth.SetDefaultValue( 100, true );
	m_ExtentHeight.SetDefaultValue( 100, true );

	m_TranslationX.SetDefaultValue( 0, true );
	m_TranslationY.SetDefaultValue( 0, true );

	m_LearnedTranslationX.SetDefaultValue( 0, true );
	m_LearnedTranslationY.SetDefaultValue( 0, true );

	m_DisplacementX.SetDefaultValue( 0, true );
	m_DisplacementY.SetDefaultValue( 0, true );

	m_LeftResult.SetDefaultValue( 10, true );
	m_TopResult.SetDefaultValue( 10, true );

	m_EnableSourceImageExtents.SetDefaultValue(true, true); // this embedded value object is not used anymore

	m_OutputImage.InitializeImage( SVImageTypePhysical );

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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	if ( SV_SHIFT_NONE != mode && (nullptr == GetTranslationXInput() || nullptr == GetTranslationYInput()) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}
#pragma endregion Private Methods

