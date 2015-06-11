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
#include "SVShiftTool.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ToolSizeAdjustTask.h"
#include <boost/math/special_functions/round.hpp>
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVShiftTool, SVShiftToolClassGuid );
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

	l_Status &= m_OutputImage.InitializeImage( GetImageInput() ) == S_OK;

	m_SourceImageNames.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE );
	m_TranslationX.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_TranslationY.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_LearnedTranslationX.ObjectAttributesAllowedRef() |= ( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE );
	m_LearnedTranslationY.ObjectAttributesAllowedRef() |= ( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE );
	m_DisplacementX.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_DisplacementY.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_LeftResult.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );
	m_TopResult.ObjectAttributesAllowedRef() &= ( ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE & ~SV_PRINTABLE );

	extentRight.ObjectAttributesAllowedRef() &= (~SV_PRINTABLE);
	extentBottom.ObjectAttributesAllowedRef() &= (~SV_PRINTABLE);
	
	m_LearnedTranslationX.ObjectAttributesAllowedRef() &= (~SV_EXTENT_OBJECT);
	m_LearnedTranslationY.ObjectAttributesAllowedRef() &= (~SV_EXTENT_OBJECT);

	isCreated = l_Status;

	SetAttributeData();

	if(l_Status)
	{
		l_Status  = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this,false,true,true)); 
	}

	
	return l_Status;
}

HRESULT SVShiftTool::ResetObject()
{
	SVGUID l_ParentGuid;
	SVImageClass* l_pInputImage = GetImageInput();

	SetAttributeData();

	if( l_pInputImage != NULL )
	{
		l_ParentGuid = l_pInputImage->GetUniqueObjectID();
	}

	m_OutputImage.UpdateImage( l_ParentGuid );
	
	HRESULT l_Status = SVToolClass::ResetObject();

	return l_Status;
}

HRESULT SVShiftTool::SetImageExtentToParent( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( p_ulIndex, l_NewExtent );

	if( l_hrOk == S_OK )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

HRESULT SVShiftTool::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	HRESULT l_hr = S_FALSE;

	SVImageClass* l_pInputImage = GetImageInput();

	if( l_pInputImage != NULL )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		m_SourceImageNames.SetValue( RRunStatus.m_lResultDataIndex, 0, l_strName );

		l_hr = S_OK;
	}

	return l_hr;
}

HRESULT SVShiftTool::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_SourceImageNames;

	return S_OK;
}

HRESULT SVShiftTool::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL && p_psvImage == GetImageInput() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

SVTaskObjectClass* SVShiftTool::GetObjectAtPoint( const SVExtentPointStruct &p_rPoint )
{
	SVImageExtentClass l_Extents;

	SVTaskObjectClass* l_pObject = NULL;

	if( m_svToolExtent.GetImageExtent( l_Extents ) == S_OK &&
	    l_Extents.GetLocationPropertyAt( p_rPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_pObject = this;
	}

	return l_pObject;
}

HRESULT SVShiftTool::DoesObjectHaveExtents() const
{
	return S_OK;
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL SVShiftTool::onRun( SVRunStatusClass& p_rRunStatus )
{
	BOOL l_Status = SVToolClass::onRun( p_rRunStatus );
	long l_Mode;

	if( l_Status )
	{
		l_Status = ( m_evoShiftMode.GetValue( l_Mode ) == S_OK );
	}

	if ( l_Status )
	{
		if (!p_rRunStatus.IsDisabled() && !p_rRunStatus.IsDisabledByCondition())
		{
			SVSmartHandlePointer OutImageHandle;
			SVImageBufferHandleImage l_OutMilHandle;

			l_Status = l_Status && m_OutputImage.SetImageHandleIndex( p_rRunStatus.Images );
			l_Status = l_Status && m_OutputImage.GetImageHandle( OutImageHandle );
			l_Status = l_Status && !( OutImageHandle.empty() );
			l_Status = l_Status && ( OutImageHandle->GetData( l_OutMilHandle ) == S_OK );
			l_Status = l_Status && !( l_OutMilHandle.empty() );

			if( l_Status )
			{
				SVMatroxBufferInterface::ClearBuffer( l_OutMilHandle.GetBuffer(), 0 );
			}

			double l_Left = 0.0;
			double l_Top = 0.0;

			l_Status = l_Status && ( extentLeft.GetValue( l_Left ) == S_OK );
			l_Status = l_Status && ( extentTop.GetValue( l_Top ) == S_OK );

			if( l_Status )
			{
				BOOL l_PerformTranslation = false;
				long l_LearnedTranslationX = 0;
				long l_LearnedTranslationY = 0;
				double fInputTranslationX = 0.0;
				double fInputTranslationY = 0.0;

				if ( (l_Status ) && ( (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE) || (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE) ) )
				{
					SVDoubleValueObjectClass* l_pTranslationXInput = GetTranslationXInput();
					SVDoubleValueObjectClass* l_pTranslationYInput = GetTranslationYInput();

					l_Status = l_Status && ( m_LearnedTranslationX.GetValue( l_LearnedTranslationX ) == S_OK );
					l_Status = l_Status && ( m_LearnedTranslationY.GetValue( l_LearnedTranslationY ) == S_OK );

					l_Status = l_Status && ( l_pTranslationXInput != NULL && l_pTranslationXInput->GetValue( fInputTranslationX ) == S_OK );
					l_Status = l_Status && ( l_pTranslationYInput != NULL && l_pTranslationYInput->GetValue( fInputTranslationY ) == S_OK );
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
						m_LearnedTranslationX.SetValue( p_rRunStatus.m_lResultDataIndex,0.0);
						m_LearnedTranslationY.SetValue( p_rRunStatus.m_lResultDataIndex,0.0);

						m_TranslationX.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationX );
						m_TranslationY.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationY );
						m_DisplacementX.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationX);
						m_DisplacementY.SetValue( p_rRunStatus.m_lResultDataIndex, nInputTranslationY );
						extentLeft.SetValue(p_rRunStatus.m_lResultDataIndex, 0.0);
						extentTop.SetValue(p_rRunStatus.m_lResultDataIndex, 0.0);

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

				if ( GetImageExtent( l_svExtents ) == S_OK )
				{
					SVExtentFigureStruct l_svFigure;

					if ( l_svExtents.GetFigure( l_svFigure ) == S_OK )
					{
						long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

						extentRight.SetValue( p_rRunStatus.m_lResultDataIndex, l_lValue );
						
						l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionY);

						extentBottom.SetValue( p_rRunStatus.m_lResultDataIndex, l_lValue );
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

			BOOL bUpdateImage = FALSE;

			m_EnableSourceImageExtents.GetValue(bUpdateImage);

			if ( (l_Mode != SV_SHIFT_ENUM::SV_SHIFT_NONE) && (bUpdateImage) )
			{
				UpdateImageWithExtent(p_rRunStatus.m_lResultDataIndex);
			}

			if( l_Status )
			{
				SVSmartHandlePointer InImageHandle;
				SVImageBufferHandleImage l_InMilHandle;

				SVImageClass* l_pImageInput = GetImageInput();

				double l_OffsetX = 0.0;
				double l_OffsetY = 0.0;

				l_Status = l_Status && ( m_LeftResult.GetValue( l_OffsetX ) == S_OK );
				l_Status = l_Status && ( m_TopResult.GetValue( l_OffsetY ) == S_OK );
				l_Status = l_Status && ( l_pImageInput != NULL );
				l_Status = l_Status && ( l_pImageInput->GetImageHandle( InImageHandle ) );
				l_Status = l_Status && !( InImageHandle.empty() );
				l_Status = l_Status && ( InImageHandle->GetData( l_InMilHandle ) == S_OK );
				l_Status = l_Status && !( l_InMilHandle.empty() );

				if( l_Status )
				{
					SVMatroxImageInterface::SVStatusCode l_Code;

					l_Code = SVMatroxBufferInterface::CopyBuffer( l_OutMilHandle.GetBuffer(), l_InMilHandle.GetBuffer(), static_cast< long >( -l_OffsetX ), static_cast< long >( -l_OffsetY ) );

					l_Status = ( l_Code == SVMEE_STATUS_OK );
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

		CollectInputImageNames( p_rRunStatus );
	}

	return l_Status;
}

BOOL SVShiftTool::OnValidate()
{
	BOOL l_Status = true;
	BOOL l_PerformTranslation = false;
	long l_Mode = 0;

	m_evoShiftMode.GetValue(l_Mode);
	if ( l_Mode != 0 )
	{
		l_PerformTranslation = TRUE;
	}

	l_Status = l_Status && ( GetImageInput() != NULL );
	l_Status = l_Status && ( !l_PerformTranslation || GetTranslationXInput() != NULL );
	l_Status = l_Status && ( !l_PerformTranslation || GetTranslationYInput() != NULL );
	l_Status = l_Status && SVToolClass::OnValidate();

	if( !l_Status )
	{
		SetInvalid();
	}

	return l_Status;
}

HRESULT SVShiftTool::UpdateImageWithExtent( unsigned long p_Index )
{
	HRESULT l_Status = S_OK;

	l_Status = m_svToolExtent.UpdateImageWithExtent( p_Index, SVToolExtentClass::SVRebuidOnResizeToolExtent );

	return l_Status;
}

SVImageClass* SVShiftTool::GetImageInput() const
{
	SVImageClass* l_pImage = NULL;

	if( m_ImageInput.IsConnected() )
	{
		l_pImage = dynamic_cast< SVImageClass* >( m_ImageInput.GetInputObjectInfo().PObject );
	}

	return l_pImage;
}


SVDoubleValueObjectClass* SVShiftTool::GetTranslationXInput() const
{
	SVDoubleValueObjectClass* l_pValue = NULL;

	if( m_TranslationXInput.IsConnected() )
	{
		l_pValue = dynamic_cast< SVDoubleValueObjectClass* >( m_TranslationXInput.GetInputObjectInfo().PObject );
	}

	return l_pValue;
}

SVDoubleValueObjectClass* SVShiftTool::GetTranslationYInput() const
{
	SVDoubleValueObjectClass* l_pValue = NULL;

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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVShiftToolObjectType;

	SVObjectClass* l_pObject = NULL;

	SVObjectManagerClass::Instance().ConstructObject( SVEvaluateTranslationXClassGuid, l_pObject );

	if( l_pObject != NULL )
	{
		l_pObject->SetObjectOwner( this );

		AddFriend( l_pObject->GetUniqueObjectID() );
	}

	ToolSizeAdjustTask::AddToFriendlist(this, false,true,true);
	




	SVObjectManagerClass::Instance().ConstructObject( SVEvaluateTranslationYClassGuid, l_pObject );

	if( l_pObject != NULL )
	{
		l_pObject->SetObjectOwner( this );

		AddFriend( l_pObject->GetUniqueObjectID() );
	}

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
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemNone );
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
	extentTop.SetDefaultValue( 10, true );
	extentLeft.SetDefaultValue( 10, true );
	extentWidth.SetDefaultValue( 100, true );
	extentHeight.SetDefaultValue( 100, true );

	m_TranslationX.SetDefaultValue( 0, true );
	m_TranslationY.SetDefaultValue( 0, true );

	m_LearnedTranslationX.SetDefaultValue( 0, true );
	m_LearnedTranslationY.SetDefaultValue( 0, true );

	m_DisplacementX.SetDefaultValue( 0, true );
	m_DisplacementY.SetDefaultValue( 0, true );

	m_LeftResult.SetDefaultValue( 10, true );
	m_TopResult.SetDefaultValue( 10, true );

	m_EnableSourceImageExtents.SetDefaultValue(true,true);

	m_OutputImage.InitializeImage( SVImageTypePhysical );

	m_evoShiftMode.SetEnumTypes(g_strShiftToolEnum);
	m_evoShiftMode.SetDefaultValue(SV_SHIFT_NONE,TRUE);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

void SVShiftTool::LocalDestroy()
{
}

void SVShiftTool::SetAttributeData()
{
	long l_Mode = 0;
	HRESULT hrRes = S_FALSE;
	SVExtentPropertyInfoStruct info;

	hrRes = m_evoShiftMode.GetValue(l_Mode);
	if ( hrRes == S_OK )
	{
		if (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE)
		{
			//Turn Extent data on
			//Set Extent Property Info so items show up in the Adjust Tool Position Dlg

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX,info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY,info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX,info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointX,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY,info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointY,info);
		}
		else if (l_Mode == SV_SHIFT_ENUM::SV_SHIFT_NONE) 
		{
			//Set Extent Property Info so items don't show up in the Adjust Tool Position Dlg
			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX,info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY,info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY,info);

			//Set Extent Property Info so items show up in the Adjust Tool Position Dlg
			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX,info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointX,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY,info);
			info.bHidden = false;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointY,info);
		}
		else
		{
			//Set Extent Property Info so items don't show up in the Adjust Tool Position Dlg
			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX,info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetX,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY,info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyTranslationOffsetY,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX,info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointX,info);

			m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY,info);
			info.bHidden = true;
			m_svToolExtent.SetExtentPropertyInfo(SVExtentPropertyPositionPointY,info);

			//set default value to be 0,0
			extentLeft.SetDefaultValue(0,true);
			extentTop.SetDefaultValue(0,true);
		}
	}
}


BOOL SVShiftTool::IsValid()
{
	BOOL bValid = TRUE;

	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask(this);
	if(nullptr != pToolSizeAdjustTask)
	{
			bValid = pToolSizeAdjustTask->OnValidate();
	}

	return SVToolClass::IsValid() & bValid ;
}

EAutoSize SVShiftTool::GetAutoSizeEnabled()
{
	long shiftMode;
	m_evoShiftMode.GetValue(shiftMode);
	if (shiftMode == SV_SHIFT_ENUM::SV_SHIFT_ABSOLUTE)
	{
		return EnableSize ;
	}
	else
	{
		return (EnableSizeAndPosition);
	}
}


#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVShiftTool.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:03:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   11 Jan 2013 15:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new extent functionality that does not rebuild the image buffers on a positional change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Jan 2013 12:45:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   disabled the EnableSourceImageExtents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jan 2013 16:42:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new value object for Enable Source Image Extents
 * Changed onRun to check if Enable Source Image Extents to update extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Jan 2013 15:00:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetAttributeData method. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jan 2013 15:12:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated code for the different modes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2012 11:22:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/