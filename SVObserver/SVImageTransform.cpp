//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTransform
//* .File Name       : $Workfile:   SVImageTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:44:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageTransform.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVGlobal.h"
#include "SVTool.h"
#include "SVTransformationTool.h"


SV_IMPLEMENT_CLASS( SVImageTransformClass, SVImageTransformClassGuid )

#pragma region Constructor
SVImageTransformClass::SVImageTransformClass( SVObjectClass* POwner, int StringResourceID )
	:SVTransformClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVImageTransformObjectType;

	// Identify our input type needs...
	// Image
	m_inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputImageObjectInfo, _T( "ImageTransformImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_useExtentsOnly, SVUseExtentsOnlyObjectGuid, IDS_OBJECTNAME_USE_EXTENTS_ONLY, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_extentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SVResetItemNone, _T("Extent Width") );
	RegisterEmbeddedObject( &m_extentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SVResetItemNone, _T("Extent Height") );

	//Register new value objects for displacementX and displacementY
	RegisterEmbeddedObject( &m_extentDisplacementX, SVImageTransformDisplacementXGuid, IDS_TRANSFORM_DISPLACEMENTX, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_extentDisplacementY, SVImageTransformDisplacementYGuid, IDS_TRANSFORM_DISPLACEMENTY, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_extentSourceX, SVImageTransformSourceXGuid, IDS_TRANSFORM_SOURCE_X, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_extentSourceY, SVImageTransformSourceYGuid, IDS_TRANSFORM_SOURCE_Y, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_extentRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SVResetItemNone, _T("Extent Angle") );

	// Interpolation mode object
	// Set Default Interpolation Mode to use Nearest Neighbor
	CString strMode("");
	CString strPrepare("");
	CString strEnumTypes("");
	// M_NEAREST_NEIGHBOR 
	strMode.LoadString( IDS_NEAREST_NEIGHBOR_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVNearestNeighOverScanClear); // M_NEAREST_NEIGHBOR);
	strEnumTypes += strPrepare;
	// M_BILINEAR
	strMode.LoadString( IDS_BILINEAR_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVBilinear);		// M_BILINEAR );
	strEnumTypes += strPrepare;
	// M_BICUBIC
	strMode.LoadString( IDS_BICUBIC_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVBiCubic);			// M_BICUBIC );
	strEnumTypes += strPrepare;
	// And now set enum types...
	m_interpolationMode.SetEnumTypes( strEnumTypes );
	m_interpolationMode.SetDefaultValue( SVNearestNeighOverScanClear, TRUE );	// Refer to MIL...
	RegisterEmbeddedObject( &m_interpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SVResetItemNone );

	// Set Embedded defaults
	m_useExtentsOnly.SetDefaultValue( FALSE, TRUE );

	m_extentWidth.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_WIDTH, TRUE );
	m_extentHeight.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_HEIGHT, TRUE );

	m_outputImageObject.InitializeImage( SVImageTypePhysical );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImageTransformClass::~SVImageTransformClass()
{
	CloseObject();
}
#pragma endregion

#pragma region Public Methods
#pragma region virtual
BOOL SVImageTransformClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = SVTransformClass::CreateObject( PCreateStructure );

	l_bOk = l_bOk && nullptr != GetTool();

	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyWidth, &m_extentWidth );
	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyHeight, &m_extentHeight );
	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyTranslationOffsetX, &m_extentDisplacementX );
	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyTranslationOffsetY, &m_extentDisplacementY );
	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyPositionPointX, &m_extentSourceX );
	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyPositionPointY, &m_extentSourceY );
	l_bOk = l_bOk && S_OK == GetTool()->SetImageExtentProperty( SVExtentPropertyRotationAngle, &m_extentRotationAngle );

	l_bOk = l_bOk && S_OK == UpdateTransformData( 1 );

	m_isCreated = l_bOk;

	// Set / Reset Printable Flags
	m_useExtentsOnly.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_extentWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_extentHeight.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_extentDisplacementX.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_extentDisplacementY.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_extentSourceX.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_extentSourceY.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_extentRotationAngle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return l_bOk;
}

HRESULT SVImageTransformClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage && p_psvImage == getInputImage() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

BOOL SVImageTransformClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputImage() )
	{
		bRetVal = TRUE;
		bRetVal = SVTransformClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

HRESULT SVImageTransformClass::ResetObject( )
{
	HRESULT l_hrOk = UpdateTransformData( 1 );

	::KeepPrevError( l_hrOk, SVTransformClass::ResetObject() );

	CollectInputImageNames();

	return l_hrOk;
}

bool SVImageTransformClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );
	return( __super::resetAllObjects( shouldNotifyFriends, silentReset ) && Result );
}
#pragma endregion

SVImageClass* SVImageTransformClass::getInputImage()
{
	if( m_inputImageObjectInfo.IsConnected() && m_inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) m_inputImageObjectInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVImageClass* SVImageTransformClass::getOutputImage()
{
	return &m_outputImageObject;
}
#pragma endregion

#pragma region Protected Methods
BOOL SVImageTransformClass::onRun( SVRunStatusClass& runStatus )
{
	BOOL bRetVal = SVTransformClass::onRun( runStatus );

	BOOL l_bUseExtentsOnly = FALSE;
	BOOL l_bTranslationEnabled = FALSE;
	BOOL l_bRotationEnabled = FALSE;

	double xDisplacement = 0.0;
	double yDisplacement = 0.0;
	double srcX = 0.0;
	double srcY = 0.0;
	double angle = 0.0;
	double dstX = 0.0;
	double dstY = 0.0;
	double width = 0.0;
	double height = 0.0;

	SVImageClass* l_psvInputImage = getInputImage();

	bRetVal = bRetVal && nullptr != l_psvInputImage;

	bRetVal = bRetVal && S_OK == UpdateTransformData( runStatus.m_lResultDataIndex );

	bRetVal = bRetVal && S_OK == m_useExtentsOnly.GetValue( l_bUseExtentsOnly );
	bRetVal = bRetVal && S_OK == performTranslation.GetValue( l_bTranslationEnabled );
	bRetVal = bRetVal && S_OK == performRotation.GetValue( l_bRotationEnabled );

	if( bRetVal )
	{
		SVImageExtentClass l_svExtents = m_outputImageObject.GetImageExtents();

		if( l_bTranslationEnabled )
		{
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetX, xDisplacement );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetY, yDisplacement );
		}

		if( l_bRotationEnabled )
		{
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, srcX );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, srcY );
			bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyRotationAngle, angle );
		}

		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyWidth, width );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyHeight, height );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputPositionPointX, dstX );
		bRetVal = bRetVal && S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputPositionPointY, dstY );
	}

	if( bRetVal )
	{
		SVSmartHandlePointer InImageHandle;
		SVSmartHandlePointer OutImageHandle;

		bRetVal = bRetVal && l_psvInputImage->GetImageHandle( InImageHandle ) && !( InImageHandle.empty() );

		long intpolType = 0;
		m_interpolationMode.GetValue(runStatus.m_lResultDataIndex, intpolType);
		SVImageOperationTypeEnum interpolationType = static_cast<SVImageOperationTypeEnum>(intpolType);

		// Check if using Source Image for Extents only
		if( l_bUseExtentsOnly )
		{
			POINT l_oPoint;
			SVImageInfoClass InImageInfo = l_psvInputImage->GetImageInfo();

			if( S_OK == InImageInfo.GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint ) )
			{
				// Get Root Image from our Input Image
				SVImageClass* l_psvRootImage = dynamic_cast<SVImageClass*>( l_psvInputImage->GetRootImage() );
				if( nullptr != l_psvRootImage )
				{
					interpolationType = SVImageDefault; // M_DEFAULT;
				}
			}
		}

		bRetVal = bRetVal && m_outputImageObject.SetImageHandleIndex( runStatus.Images );
		bRetVal = bRetVal && m_outputImageObject.GetImageHandle( OutImageHandle ) && !( OutImageHandle.empty() );

		if( bRetVal && nullptr != OutImageHandle->GetBufferAddress() )
		{
			memset( OutImageHandle->GetBufferAddress(), 0, static_cast<size_t>(width * height) );
		}

		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		if( bRetVal )
		{
			bRetVal = ( S_OK == InImageHandle->GetData( l_InMilHandle ) );
			bRetVal &= !( l_InMilHandle.empty() );
		}

		if( bRetVal )
		{
			bRetVal = ( S_OK == OutImageHandle->GetData( l_OutMilHandle ) );
			bRetVal &= !( l_OutMilHandle.empty() );
		}

		if( bRetVal )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;

			SVMatroxImageRotateStruct l_Rotate(l_InMilHandle.GetBuffer());
			l_Rotate.m_dAngle = angle;
			l_Rotate.m_dSrcCenX = srcX + xDisplacement;
			l_Rotate.m_dSrcCenY = srcY + yDisplacement;
			l_Rotate.m_dDstCenX = dstX;
			l_Rotate.m_dDstCenY = dstY;
			l_Rotate.m_eInterpolation = interpolationType;

			// Use MimRotate to Rotate, Translate or Copy buffer
			// Rotate...( and translate image )
			l_Code = SVMatroxImageInterface::Rotate( l_OutMilHandle.GetBuffer(), l_Rotate );

			if( l_Code != SVMEE_STATUS_OK )
			{
				bRetVal = FALSE;
			}
		}
	}

	if( bRetVal )
	{
		runStatus.SetPassed();
	}
	else
	{
		runStatus.SetFailed();
		runStatus.SetInvalid();

		SetInvalid();
	}

	return bRetVal;
}

HRESULT SVImageTransformClass::UpdateTransformData( long p_lIndex )
{
	HRESULT l_hrOk = S_OK;

	SVImageClass* l_psvInputImage = getInputImage();
	SVToolClass* l_psvTool = GetTool();

	if( nullptr != l_psvInputImage && nullptr != l_psvTool )
	{
		POINT l_oUseExtentsOnlyPoint;

		SVImageExtentClass l_svExtents = l_psvInputImage->GetImageExtents();

		SVDoubleValueObjectClass* l_pTranslationXResult = getInputTranslationXResult();
		SVDoubleValueObjectClass* l_pTranslationYResult = getInputTranslationYResult();
		SVDoubleValueObjectClass* l_pRotationXResult = getInputRotationXResult();
		SVDoubleValueObjectClass* l_pRotationYResult = getInputRotationYResult();
		SVDoubleValueObjectClass* l_pRotationAngleResult = getInputRotationAngleResult();

		BOOL l_bTranslationEnabled = false;
		BOOL l_bRotationEnabled = false;
		BOOL l_bUseExtentsOnly = false;

		double l_dTranslationXResult = 0.0;
		double l_dTranslationYResult = 0.0;

		double l_dRotationXResult = 0.0;
		double l_dRotationYResult = 0.0;
		double l_dRotationAngleResult = 0.0;

		double l_dLearnedTranslationXValue = 0.0;
		double l_dLearnedTranslationYValue = 0.0;
		double l_dLearnedRotationXValue = 0.0;
		double l_dLearnedRotationYValue = 0.0;
		double l_dLearnedRotationAngleValue = 0.0;

		double l_dWidth = 0.0;
		double l_dHeight = 0.0;

		::KeepPrevError( l_hrOk, performTranslation.GetValue( l_bTranslationEnabled ) );
		::KeepPrevError( l_hrOk, performRotation.GetValue( l_bRotationEnabled ) );
		::KeepPrevError( l_hrOk, m_useExtentsOnly.GetValue( l_bUseExtentsOnly ) );

		::KeepPrevError( l_hrOk, l_svExtents.GetExtentProperty( SVExtentPropertyWidth, l_dWidth ) );
		::KeepPrevError( l_hrOk, l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_dHeight ) );

		bool l_bAlwaysUpdate = false;

		if( S_OK == l_hrOk && l_bTranslationEnabled )
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, l_pTranslationXResult->GetValue( l_dTranslationXResult ) );
			::KeepPrevError( l_hrOk, l_pTranslationYResult->GetValue( l_dTranslationYResult ) );

			::KeepPrevError( l_hrOk, learnedTranslationX.GetValue( l_dLearnedTranslationXValue ) );
			::KeepPrevError( l_hrOk, learnedTranslationY.GetValue( l_dLearnedTranslationYValue ) );
		}

		if( S_OK == l_hrOk && l_bRotationEnabled )
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, l_pRotationXResult->GetValue( l_dRotationXResult ) );
			::KeepPrevError( l_hrOk, l_pRotationYResult->GetValue( l_dRotationYResult ) );
			::KeepPrevError( l_hrOk, l_pRotationAngleResult->GetValue( l_dRotationAngleResult ) );

			::KeepPrevError( l_hrOk, learnedRotationX.GetValue( l_dLearnedRotationXValue ) );
			::KeepPrevError( l_hrOk, learnedRotationY.GetValue( l_dLearnedRotationYValue ) );
			::KeepPrevError( l_hrOk, learnedRotationAngle.GetValue( l_dLearnedRotationAngleValue ) );
		}

		l_psvTool->SetAlwaysUpdate( l_bAlwaysUpdate );

		if( S_OK == l_hrOk && l_bUseExtentsOnly )
		{
			::KeepPrevError( l_hrOk, l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_oUseExtentsOnlyPoint ) );
		}

		if( S_OK == l_hrOk )
		{
			double xDisplacement = 0.0;
			double yDisplacement = 0.0;

			double srcX = 0.0;
			double srcY = 0.0;
			double angle = 0.0;

			if( l_bTranslationEnabled )
			{
				xDisplacement = ( l_dTranslationXResult - l_dLearnedTranslationXValue );
				yDisplacement = ( l_dTranslationYResult - l_dLearnedTranslationYValue );
			}

			if( l_bRotationEnabled )
			{
				srcX = ( l_dRotationXResult - l_dLearnedRotationXValue );
				srcY = ( l_dRotationYResult - l_dLearnedRotationYValue );
				angle = ( l_dRotationAngleResult - l_dLearnedRotationAngleValue );

				l_hrOk = l_svExtents.SetTranslation( SVExtentTranslationTransformRotate );
			}
			else
			{
				l_hrOk = l_svExtents.SetTranslation( SVExtentTranslationTransformShift );
			}

			// Check if using Source Image for Extents only
			if( l_bUseExtentsOnly )
			{
				xDisplacement += l_oUseExtentsOnlyPoint.x;
				yDisplacement += l_oUseExtentsOnlyPoint.y;
			}

			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyWidth, l_dWidth ) );
			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyHeight, l_dHeight ) );
			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, xDisplacement ) );
			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, yDisplacement ) );
			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyPositionPointX, srcX ) );
			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyPositionPointY, srcY ) );
			::KeepPrevError( l_hrOk, l_svExtents.SetExtentProperty( SVExtentPropertyRotationAngle, angle ) );
		}

		::KeepPrevError( l_hrOk, l_psvTool->SetImageExtent( p_lIndex, l_svExtents ) );

		::KeepPrevError( l_hrOk, m_outputImageObject.InitializeImage( getInputImage() ) );

		// Return code for UpdateImageWithExtend not being checked because it may not be valid the first time.
		l_psvTool->UpdateImageWithExtent( p_lIndex );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}
#pragma endregion

#pragma region Private Methods
// Set String value object for Source Image Names
HRESULT SVImageTransformClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVTransformationToolClass* l_pTool = dynamic_cast<SVTransformationToolClass*>(GetTool());
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		l_pTool->GetInputImageNames()->SetValue( 0, l_strName );

		l_hr = S_OK;
	}
	return l_hr;
}
#pragma endregion

