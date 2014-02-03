//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTransform
//* .File Name       : $Workfile:   SVImageTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Feb 2014 10:48:52  $
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
	outObjectInfo.ObjectTypeInfo.SubType = SVImageTransformObjectType;

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

	l_bOk = l_bOk && GetTool() != NULL;

	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyWidth, &m_extentWidth ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyHeight, &m_extentHeight ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyTranslationOffsetX, &m_extentDisplacementX ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyTranslationOffsetY, &m_extentDisplacementY ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyPositionPointX, &m_extentSourceX ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyPositionPointY, &m_extentSourceY ) == S_OK;
	l_bOk = l_bOk && GetTool()->SetImageExtentProperty( SVExtentPropertyRotationAngle, &m_extentRotationAngle ) == S_OK;

	l_bOk = l_bOk && UpdateTransformData( 1 ) == S_OK;

	isCreated = l_bOk;

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

	if ( p_psvImage != NULL && p_psvImage == getInputImage() )
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

	return l_hrOk;
}
#pragma endregion

SVImageClass* SVImageTransformClass::getInputImage()
{
	if( m_inputImageObjectInfo.IsConnected() && m_inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) m_inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
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

	bRetVal = bRetVal && l_psvInputImage != NULL;

	bRetVal = bRetVal && UpdateTransformData( runStatus.m_lResultDataIndex ) == S_OK;

	bRetVal = bRetVal && m_useExtentsOnly.GetValue( l_bUseExtentsOnly ) == S_OK;
	bRetVal = bRetVal && performTranslation.GetValue( l_bTranslationEnabled ) == S_OK;
	bRetVal = bRetVal && performRotation.GetValue( l_bRotationEnabled ) == S_OK;

	if( bRetVal )
	{
		CollectInputImageNames(runStatus);

		SVImageExtentClass l_svExtents = m_outputImageObject.GetImageExtents();

		if( l_bTranslationEnabled )
		{
			bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetX, xDisplacement ) == S_OK;
			bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetY, yDisplacement ) == S_OK;
		}

		if( l_bRotationEnabled )
		{
			bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, srcX ) == S_OK;
			bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, srcY ) == S_OK;
			bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyRotationAngle, angle ) == S_OK;
		}

		bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyWidth, width ) == S_OK;
		bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyHeight, height ) == S_OK;
		bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyOutputPositionPointX, dstX ) == S_OK;
		bRetVal = bRetVal && l_svExtents.GetExtentProperty( SVExtentPropertyOutputPositionPointY, dstY ) == S_OK;
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

			if( InImageInfo.GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint ) == S_OK )
			{
				// Get Root Image from our Input Image
				SVImageClass* l_psvRootImage = dynamic_cast<SVImageClass*>( l_psvInputImage->GetRootImage() );
				if( l_psvRootImage != NULL )
				{
					interpolationType = SVImageDefault; // M_DEFAULT;
				}
			}
		}

		bRetVal = bRetVal && m_outputImageObject.SetImageHandleIndex( runStatus.Images );
		bRetVal = bRetVal && m_outputImageObject.GetImageHandle( OutImageHandle ) && !( OutImageHandle.empty() );

		if( bRetVal && OutImageHandle->GetBufferAddress() != NULL )
		{
			memset( OutImageHandle->GetBufferAddress(), 0, static_cast<size_t>(width * height) );
		}

		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		if( bRetVal )
		{
			bRetVal = ( InImageHandle->GetData( l_InMilHandle ) == S_OK );
			bRetVal &= !( l_InMilHandle.empty() );
		}

		if( bRetVal )
		{
			bRetVal = ( OutImageHandle->GetData( l_OutMilHandle ) == S_OK );
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

LONG_PTR SVImageTransformClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

HRESULT SVImageTransformClass::UpdateTransformData( long p_lIndex )
{
	HRESULT l_hrOk = S_OK;

	SVImageClass* l_psvInputImage = getInputImage();
	SVToolClass* l_psvTool = GetTool();

	if( l_psvInputImage != NULL && l_psvTool != NULL )
	{
		POINT l_oUseExtentsOnlyPoint;

		SVImageExtentClass l_svExtents = l_psvInputImage->GetImageExtents();

		SVDoubleValueObjectClass* l_pTranslationXResult = getInputTranslationXResult();
		SVDoubleValueObjectClass* l_pTranslationYResult = getInputTranslationYResult();
		SVDoubleValueObjectClass* l_pRotationXResult = getInputRotationXResult();
		SVDoubleValueObjectClass* l_pRotationYResult = getInputRotationYResult();
		SVDoubleValueObjectClass* l_pRotationAngleResult = getInputRotationAngleResult();

		BOOL l_bTranslationEnabled = FALSE;
		BOOL l_bRotationEnabled = FALSE;
		BOOL l_bUseExtentsOnly = FALSE;

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

		if( l_hrOk == S_OK && l_bTranslationEnabled )
		{
			l_bAlwaysUpdate = true;

			::KeepPrevError( l_hrOk, l_pTranslationXResult->GetValue( l_dTranslationXResult ) );
			::KeepPrevError( l_hrOk, l_pTranslationYResult->GetValue( l_dTranslationYResult ) );

			::KeepPrevError( l_hrOk, learnedTranslationX.GetValue( l_dLearnedTranslationXValue ) );
			::KeepPrevError( l_hrOk, learnedTranslationY.GetValue( l_dLearnedTranslationYValue ) );
		}

		if( l_hrOk == S_OK && l_bRotationEnabled )
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

		if( l_hrOk == S_OK && l_bUseExtentsOnly )
		{
			::KeepPrevError( l_hrOk, l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_oUseExtentsOnlyPoint ) );
		}

		if( l_hrOk == S_OK )
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
HRESULT SVImageTransformClass::CollectInputImageNames( SVRunStatusClass& runStatus )
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVTransformationToolClass* l_pTool = dynamic_cast<SVTransformationToolClass*>(GetTool());
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		l_pTool->m_svSourceImageNames.SetValue( runStatus.m_lResultDataIndex, 0, l_strName );

		l_hr = S_OK;
	}
	return l_hr;
}
#pragma endregion

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageTransform.cpp_v  $
 * 
 *    Rev 1.3   01 Feb 2014 10:48:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Jan 2014 16:40:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  879
 * SCR Title:  Add interpolation mode to transformation tool
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Changed constructor to initialize values for interpolation mode.
 * Changed onRun to set interpolationType based on the Interpolation Mode option.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   02 May 2013 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:55:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   05 Dec 2012 13:41:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed code associated with SCR 798
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   01 Nov 2012 13:38:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed onRun for the new tanslation mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   12 Jul 2012 14:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Jun 2012 12:47:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   21 Sep 2011 08:32:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified UpdateTransformData to ignore the return code from UpdateImageWithExtent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   30 Mar 2011 16:20:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Mar 2011 13:35:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   11 Mar 2011 14:22:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   25 Feb 2011 12:11:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   10 Feb 2011 14:55:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   27 Jan 2011 11:20:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   04 Nov 2010 13:39:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   01 Jun 2010 14:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   16 Dec 2009 11:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   07 Mar 2008 11:54:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for Transfer Image Definition List Source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   24 Jul 2007 13:31:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   21 Jun 2007 13:05:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   19 Feb 2007 16:36:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   15 Feb 2007 11:46:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   16 Jan 2006 17:05:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   02 Nov 2005 07:33:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   14 Oct 2005 11:21:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject method to fix print flag errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   14 Sep 2005 13:21:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed unused local variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   13 Sep 2005 09:52:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the method to correctly caculate the extent data and pass it correctly to the cached source/aux data and to the main extent information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   12 Sep 2005 13:30:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix copy paste error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   12 Sep 2005 13:02:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved the gettting and setting of the output buffer data to after the reset object for the output image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Aug 2005 11:19:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   09 Aug 2005 15:56:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new value objects to handle the translation extents and hooked then to the tool extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Jul 2005 12:36:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2005 08:11:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Feb 2005 14:27:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Feb 2005 14:33:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Apr 2003 10:19:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Mar 2003 09:19:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed MimRotate to use M_NEAREST_NEIGHBOR + M_OVERSCAN_CLEAR instead of M_DEFAULT + M_OVERSCAN_CLEAR as an interpolation type
 * This change applies to MIL 6.1 as well.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jan 2003 10:33:52   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage, onRun and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Dec 2002 19:24:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Nov 2002 13:26:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2001 15:36:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Oct 2000 15:51:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  187
 * SCR Title:  Color SVIM Problem when Adding Monochrome Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes remove the ability to add a monochrome tool to the tool set until a color tool gets added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Mar 2000 21:51:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onRun to check for image pointers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2000 13:04:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVImageInfoClass methods to get/set image extents.
 * Revised onRun to use memset instead of mbufClear to clear
 * image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Class for Image Transformations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
