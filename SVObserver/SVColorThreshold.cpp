//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorThreshold
//* .File Name       : $Workfile:   SVColorThreshold.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:23:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVColorThreshold.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVDataBuffer.h"
#include "SVGlobal.h"
#include "SVImageProcessingClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVColorThresholdClass, SVColorThresholdClassGuid )

SVColorThresholdClass::SVColorThresholdClass( SVObjectClass* POwner, int StringResourceID )
				 :SVOperatorClass( POwner, StringResourceID ) 
{
	init();
}

void SVColorThresholdClass::init()
{
	bShowHistogram = FALSE;

	// Set up draw objects...
	graphFigures[0].SetDrawPen( TRUE, PS_SOLID, 1, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );
	graphFigures[1].SetDrawPen( TRUE, PS_SOLID, 1, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );
	graphFigures[2].SetDrawPen( TRUE, PS_SOLID, 1, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );

	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVOperatorObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVColorThresholdObjectType;

	// Register Embedded Object(s)

	/////////////////////////////////////////////////////////////////////////
	// Image Outputs
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &band0OutputImage, SVBand0ImageObjectGuid, IDS_OBJECTNAME_BAND0_IMAGE );
	RegisterEmbeddedObject( &band1OutputImage, SVBand1ImageObjectGuid, IDS_OBJECTNAME_BAND1_IMAGE );
	RegisterEmbeddedObject( &band2OutputImage, SVBand2ImageObjectGuid, IDS_OBJECTNAME_BAND2_IMAGE );
	RegisterEmbeddedObject( &outputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	/////////////////////////////////////////////////////////////////////////
	// Band 0 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &band0UpperThreshold, SVBand0UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND0_UPPERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &band0LowerThreshold, SVBand0LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND0_LOWERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &band0ThresholdExclude, SVBand0ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND0_THRESHOLDEXCLUDE, false, SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Band 1 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &band1UpperThreshold, SVBand1UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND1_UPPERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &band1LowerThreshold, SVBand1LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND1_LOWERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &band1ThresholdExclude, SVBand1ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND1_THRESHOLDEXCLUDE, false, SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Band 2 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &band2UpperThreshold, SVBand2UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND2_UPPERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &band2LowerThreshold, SVBand2LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND2_LOWERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &band2ThresholdExclude, SVBand2ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND2_THRESHOLDEXCLUDE, false, SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Train Color ROI extents
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &extentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SVResetItemOwner, _T("Extent X") );
	RegisterEmbeddedObject( &extentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SVResetItemOwner, _T("Extent Y") );
	RegisterEmbeddedObject( &extentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SVResetItemOwner, _T("Extent Width") );
	RegisterEmbeddedObject( &extentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SVResetItemOwner, _T("Extent Height") );

	// Set Embedded defaults
	band0UpperThreshold.SetDefaultValue( SV_DEFAULT_TOOL_UPPER_TRESH, TRUE );
	band0LowerThreshold.SetDefaultValue( SV_DEFAULT_TOOL_LOWER_TRESH, TRUE );
	band0ThresholdExclude.SetDefaultValue( FALSE, TRUE );

	band1UpperThreshold.SetDefaultValue( SV_DEFAULT_TOOL_UPPER_TRESH, TRUE );
	band1LowerThreshold.SetDefaultValue( SV_DEFAULT_TOOL_LOWER_TRESH, TRUE );
	band1ThresholdExclude.SetDefaultValue( FALSE, TRUE );

	band2UpperThreshold.SetDefaultValue( SV_DEFAULT_TOOL_UPPER_TRESH, TRUE );
	band2LowerThreshold.SetDefaultValue( SV_DEFAULT_TOOL_LOWER_TRESH, TRUE );
	band2ThresholdExclude.SetDefaultValue( FALSE, TRUE );
		
	extentLeft.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_LEFT, TRUE );
	extentTop.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_TOP, TRUE );
	extentWidth.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_WIDTH, TRUE );
	extentHeight.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_HEIGHT, TRUE );

	
	histValueArraySize = 0;
	pixelNumber		   = 0;

	band0HistogramImage.InitializeImage( SVImageTypeIndependent );

	band1HistogramImage.InitializeImage( SVImageTypeIndependent );

	band2HistogramImage.InitializeImage( SVImageTypeIndependent );

	band0OutputImage.InitializeImage( SVImageTypeIndependent );

	band1OutputImage.InitializeImage( SVImageTypeIndependent );

	band2OutputImage.InitializeImage( SVImageTypeIndependent );

	outputImage.InitializeImage( SVImageTypeIndependent );

	// Identify our input type needs...
	inputBand0Image.SetInputObjectType( SVBand0ImageObjectGuid, SVImageObjectType );
	inputBand0Image.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputBand0Image, _T( "ColorThresholdBand0Image" ) );

	inputBand1Image.SetInputObjectType( SVBand1ImageObjectGuid, SVImageObjectType );
	inputBand1Image.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputBand1Image, _T( "ColorThresholdBand1Image" ) );

	inputBand2Image.SetInputObjectType( SVBand2ImageObjectGuid, SVImageObjectType );
	inputBand2Image.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputBand2Image, _T( "ColorThresholdBand2Image" ) );

	addDefaultInputObjects();
}

SVColorThresholdClass::~SVColorThresholdClass()
{
	CloseObject();
}

BOOL SVColorThresholdClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	// Owner can only be : SVOperatorClass !!!
	if( SVOperatorClass::CreateObject( PCreateStructure ) )
	{
		SVImageClass* pImage = GetBand0InputImage();

		if( pImage )
		{
			SVImageInfoClass ImageInfo = pImage->GetImageInfo();
      
			long l_lPixelDepth = 0;

			ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, l_lPixelDepth );

		  histValueArraySize = 1 << ( l_lPixelDepth & SVBufferSize );

		  // &&&
		  SVDataBufferInfoClass svData;

		  svData.Length = histValueArraySize;
		  svData.Type = SVDataBufferInfoClass::SVHistResult;
		  svData.HBuffer.milResult = histResultID;
		  if ( SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) == S_OK )
		  {
			  histResultID = svData.HBuffer.milResult;
		  }

		  if( !histResultID.empty() )
		  {
			  // Create 3 Histograms
			  try
			  {
				  aHistValueArray[0].resize( histValueArraySize );
				  aHistValueArray[1].resize( histValueArraySize );
				  aHistValueArray[2].resize( histValueArraySize );
				  
				  // Create 4 Output Images and 3 Histogram Images
				  bOk = createImages();
				  
				  ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT,reinterpret_cast<LONG_PTR>(&band0HistogramImage), NULL );
				  ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT,reinterpret_cast<LONG_PTR>(&band1HistogramImage), NULL );
				  ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT,reinterpret_cast<LONG_PTR>(&band2HistogramImage), NULL );
			  }
			  catch(...)
			  {
			  }

			  if ( ! bOk )
			  {
				  
				  SVMatroxImageInterface::Destroy( histResultID );
			  }
      }
		}
		
		if ( ! bOk )
		{
			histValueArraySize = 0;
		}
	}

	// Set / Reset Printable Flags 
	band0UpperThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band0LowerThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band0ThresholdExclude.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band1UpperThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band1LowerThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band1ThresholdExclude.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band2UpperThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band2LowerThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	band2ThresholdExclude.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;

	extentLeft.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentTop.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentWidth.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVColorThresholdClass::createImages()
{
	createHistogramImage( GetBand0InputImage(), band0HistogramImage );
	createHistogramImage( GetBand1InputImage(), band1HistogramImage );
	createHistogramImage( GetBand2InputImage(), band2HistogramImage );
	
	createOutputImage( GetBand0InputImage(), band0OutputImage );
	createOutputImage( GetBand1InputImage(), band1OutputImage );
	createOutputImage( GetBand2InputImage(), band2OutputImage );

	createOutputImage( &band0OutputImage, outputImage );

	outputImage.InitializeImage( &band0OutputImage );

	return TRUE;
}

BOOL SVColorThresholdClass::createOutputImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage )
{
	BOOL l_bOk = FALSE;

	SVGUID l_InputID;
	SVImageInfoClass ImageInfo;

	if( p_pInputImage != NULL )
	{
		l_InputID = p_pInputImage->GetUniqueObjectID();
		ImageInfo = p_pInputImage->GetImageInfo();
	}
	else
	{
		ImageInfo = p_rOutputImage.GetImageInfo();
	}

  // Setup...
  ImageInfo.SetOwner( GetOwnerID() );

	ImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
	ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );

	l_bOk = ( p_rOutputImage.UpdateImage( l_InputID, ImageInfo ) == S_OK );

	return l_bOk;
}

BOOL SVColorThresholdClass::createHistogramImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage )
{
	BOOL l_bOk = FALSE;

	SVGUID l_InputID;
	SVImageInfoClass ImageInfo;

	if( p_pInputImage != NULL )
	{
		l_InputID = p_pInputImage->GetUniqueObjectID();
		ImageInfo = p_pInputImage->GetImageInfo();
	}
	else
	{
		ImageInfo = p_rOutputImage.GetImageInfo();
	}

	SVExtentPointStruct l_svPoint;

	double l_dWidth = 0.0;
	double l_dHeight = 0.0;

  extentLeft.GetValue( l_svPoint.m_dPositionX );
  extentTop.GetValue( l_svPoint.m_dPositionY );
  extentWidth.GetValue( l_dWidth );
  extentHeight.GetValue( l_dHeight );

	// Setup...
  ImageInfo.SetOwner( GetOwnerID() );
	ImageInfo.SetOwnerImage( p_rOutputImage.GetUniqueObjectID() );

	ImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
	ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );

	ImageInfo.SetExtentProperty( SVExtentPropertyPositionPoint, 0 );
	ImageInfo.SetExtentProperty( SVExtentPropertyWidth, l_dWidth );
	ImageInfo.SetExtentProperty( SVExtentPropertyHeight, l_dHeight );
	ImageInfo.SetTranslation( SVExtentTranslationNone );

	// Try to create image object...
  l_bOk = ( p_rOutputImage.UpdateImage( l_InputID, ImageInfo ) == S_OK );

	return l_bOk;
}

BOOL SVColorThresholdClass::CloseObject()
{
	band0HistogramImage.CloseObject();
	band1HistogramImage.CloseObject();
	band2HistogramImage.CloseObject();

	if( !histResultID.empty() )
	{
		
		SVMatroxImageInterface::Destroy( histResultID );
	}
	histValueArraySize = 0;

	return SVOperatorClass::CloseObject();
}


HRESULT SVColorThresholdClass::ResetObject()
{
	double l_dWidth = 0.0;
	double l_dHeight = 0.0;

	extentWidth.GetValue( l_dWidth );
	extentHeight.GetValue( l_dHeight );

	// Recalculate pixel number...
	pixelNumber = ( ( __int64 ) l_dWidth ) * ( ( __int64 ) l_dHeight );

	createImages();

	band0HistogramImage.ResetObject();
	band1HistogramImage.ResetObject();
	band2HistogramImage.ResetObject();

	HRESULT l_hrOk = SVOperatorClass::ResetObject();

	return l_hrOk;
}

SVImageClass* SVColorThresholdClass::GetBand0InputImage()
{
	if( inputBand0Image.IsConnected() && inputBand0Image.GetInputObjectInfo().PObject )
		return ( SVImageClass * )inputBand0Image.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVColorThresholdClass::GetBand1InputImage()
{
	if( inputBand1Image.IsConnected() && inputBand1Image.GetInputObjectInfo().PObject )
		return ( SVImageClass * )inputBand1Image.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVColorThresholdClass::GetBand2InputImage()
{
	if( inputBand2Image.IsConnected() && inputBand2Image.GetInputObjectInfo().PObject )
		return ( SVImageClass * )inputBand2Image.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVColorThresholdClass::GetBand0HistogramImage()
{
	return &band0HistogramImage;
}

SVImageClass* SVColorThresholdClass::GetBand1HistogramImage()
{
	return &band1HistogramImage;
}

SVImageClass* SVColorThresholdClass::GetBand2HistogramImage()
{
	return &band2HistogramImage;
}

void SVColorThresholdClass::GetTrainColorROIExtent( CRect &p_roRect )
{
	double value;

	extentLeft.GetValue( value );
	p_roRect.left = (long)(value);

	extentTop.GetValue( value );
	p_roRect.top = (long)(value);

	extentWidth.GetValue( value );
	p_roRect.right = (long)(value) + p_roRect.left;

	extentHeight.GetValue( value );
	p_roRect.bottom = (long)(value) + p_roRect.top;
}

SVImageClass* SVColorThresholdClass::GetBand0OutputImage()
{
	return &band0OutputImage;
}

SVImageClass* SVColorThresholdClass::GetBand1OutputImage()
{
	return &band1OutputImage;
}

SVImageClass* SVColorThresholdClass::GetBand2OutputImage()
{
	return &band2OutputImage;
}
	
SVImageClass* SVColorThresholdClass::GetOutputImage()
{
	return &outputImage;
}

BOOL SVColorThresholdClass::OnValidate() 
{
	if ( SVOperatorClass::OnValidate() )
	{
		// Validate that we have inputs and outputs
		if( inputBand0Image.IsConnected() && 
			inputBand0Image.GetInputObjectInfo().PObject && 
			inputBand0Image.GetInputObjectInfo().PObject->IsValid() &&
			inputBand1Image.IsConnected() && 
			inputBand1Image.GetInputObjectInfo().PObject && 
			inputBand1Image.GetInputObjectInfo().PObject->IsValid() &&
			inputBand2Image.IsConnected() && 
			inputBand2Image.GetInputObjectInfo().PObject && 
			inputBand2Image.GetInputObjectInfo().PObject->IsValid() &&
			band0OutputImage.IsCreated() && band0OutputImage.IsValid() && 
			band1OutputImage.IsCreated() && band1OutputImage.IsValid() &&
			band2OutputImage.IsCreated() && band2OutputImage.IsValid() &&
			outputImage.IsCreated() && outputImage.IsValid() )
		return TRUE;
	}
	return FALSE;
}

BOOL SVColorThresholdClass::onRun( SVRunStatusClass& RRunStatus )
{ 
	BOOL l_bOk = TRUE;

	// Binarizing: lowerThresh <= x <= upperTresh		--> 255 
	//	 		   otherwise							--> 0

	long band0Lower = 0;
	long band0Upper = 0;
	BOOL bBand0ThresholdExclude = FALSE;

	long band1Lower = 0;
	long band1Upper = 0;
	BOOL bBand1ThresholdExclude = FALSE;

	long band2Lower = 0;
	long band2Upper = 0;
	BOOL bBand2ThresholdExclude = FALSE;

	// Get current Threshold Flags...
	band0ThresholdExclude.GetValue( bBand0ThresholdExclude );
	band1ThresholdExclude.GetValue( bBand1ThresholdExclude );
	band2ThresholdExclude.GetValue( bBand2ThresholdExclude );
			
	// Get current Threshold values...
	band0LowerThreshold.GetValue( band0Lower );
	band0UpperThreshold.GetValue( band0Upper );

	band1LowerThreshold.GetValue( band1Lower );
	band1UpperThreshold.GetValue( band1Upper );

	band2LowerThreshold.GetValue( band2Lower );
	band2UpperThreshold.GetValue( band2Upper );

  l_bOk &= band0OutputImage.SetImageHandleIndex( RRunStatus.Images );
  l_bOk &= band1OutputImage.SetImageHandleIndex( RRunStatus.Images );
  l_bOk &= band2OutputImage.SetImageHandleIndex( RRunStatus.Images );

  l_bOk &= outputImage.SetImageHandleIndex( RRunStatus.Images );

	if( l_bOk )
	{
		l_bOk &= binarize( band0Lower, band0Upper, bBand0ThresholdExclude, GetBand0InputImage(), &band0OutputImage );
		l_bOk &= binarize( band1Lower, band1Upper, bBand1ThresholdExclude, GetBand1InputImage(), &band1OutputImage );
		l_bOk &= binarize( band2Lower, band2Upper, bBand2ThresholdExclude, GetBand2InputImage(), &band2OutputImage );
	}

	if( l_bOk )
	{
    SVSmartHandlePointer Band0Handle;
    SVSmartHandlePointer Band1Handle;
    SVSmartHandlePointer Band2Handle;
    SVSmartHandlePointer OutputHandle;

		SVImageBufferHandleImage l_Band0MilBuffer;
		SVImageBufferHandleImage l_Band1MilBuffer;
		SVImageBufferHandleImage l_Band2MilBuffer;
		SVImageBufferHandleImage l_OutputMilBuffer;

    l_bOk &= band0OutputImage.GetImageHandle( Band0Handle );
    l_bOk &= band1OutputImage.GetImageHandle( Band1Handle );
    l_bOk &= band2OutputImage.GetImageHandle( Band2Handle );

    l_bOk &= outputImage.GetImageHandle( OutputHandle );

		l_bOk &= !( Band0Handle.empty() );
		l_bOk &= !( Band1Handle.empty() );
		l_bOk &= !( Band2Handle.empty() );
		l_bOk &= !( OutputHandle.empty() );

		if( l_bOk )
		{
			Band0Handle->GetData( l_Band0MilBuffer );
			Band1Handle->GetData( l_Band1MilBuffer );
			Band2Handle->GetData( l_Band2MilBuffer );
			OutputHandle->GetData( l_OutputMilBuffer );
		}

		l_bOk &= !( l_Band0MilBuffer.empty() );
		l_bOk &= !( l_Band1MilBuffer.empty() );
		l_bOk &= !( l_Band2MilBuffer.empty() );
		l_bOk &= !( l_OutputMilBuffer.empty() );

		if( l_bOk )
    {
			
			SVMatroxImageInterface::Arithmetic( l_OutputMilBuffer.GetBuffer(), l_Band0MilBuffer.GetBuffer(), l_Band1MilBuffer.GetBuffer(), SVImageAnd);
			SVMatroxImageInterface::Arithmetic( l_OutputMilBuffer.GetBuffer(), l_Band2MilBuffer.GetBuffer(), l_OutputMilBuffer.GetBuffer(), SVImageAnd);
		  // Do Image Arithemetic - AND the 3 outputs together

		  if( bShowHistogram )
		  {
				long l_lTop = 0;
				long l_lLeft = 0;

				extentTop.GetValue( l_lTop );
				extentLeft.GetValue( l_lLeft );

				SVSmartHandlePointer ImageHandle;
				SVSmartHandlePointer InputImageHandle;

				band0HistogramImage.SetImageHandleIndex( RRunStatus.Images );
				band1HistogramImage.SetImageHandleIndex( RRunStatus.Images );
				band2HistogramImage.SetImageHandleIndex( RRunStatus.Images );

				if( GetBand0InputImage() != NULL && 
					  GetBand0InputImage()->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
				    band0HistogramImage.GetImageHandle( ImageHandle )  && !( ImageHandle.empty() )	)
				{
					SVImageBufferHandleImage l_FromMilBuffer;
					SVImageBufferHandleImage l_ToMilBuffer;

					InputImageHandle->GetData( l_FromMilBuffer );
					ImageHandle->GetData( l_ToMilBuffer );

					if( !( l_FromMilBuffer.empty() ) && !( l_ToMilBuffer.empty() ) )
					{
						SVMatroxBufferInterface::CopyBuffer( l_ToMilBuffer.GetBuffer(), l_FromMilBuffer.GetBuffer(), -l_lLeft, -l_lTop );
					}
				}

				if( GetBand1InputImage() != NULL && 
					  GetBand1InputImage()->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
				    band1HistogramImage.GetImageHandle( ImageHandle )  && !( ImageHandle.empty() )	)
				{
					SVImageBufferHandleImage l_FromMilBuffer;
					SVImageBufferHandleImage l_ToMilBuffer;

					InputImageHandle->GetData( l_FromMilBuffer );
					ImageHandle->GetData( l_ToMilBuffer );

					if( !( l_FromMilBuffer.empty() ) && !( l_ToMilBuffer.empty() ) )
					{
						SVMatroxBufferInterface::CopyBuffer( l_ToMilBuffer.GetBuffer(), l_FromMilBuffer.GetBuffer(), -l_lLeft, -l_lTop );
					}
				}

				if( GetBand2InputImage() != NULL && 
					  GetBand2InputImage()->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
				    band2HistogramImage.GetImageHandle( ImageHandle )  && !( ImageHandle.empty() )	)
				{
					SVImageBufferHandleImage l_FromMilBuffer;
					SVImageBufferHandleImage l_ToMilBuffer;

					InputImageHandle->GetData( l_FromMilBuffer );
					ImageHandle->GetData( l_ToMilBuffer );

					if( !( l_FromMilBuffer.empty() ) && !( l_ToMilBuffer.empty() ) )
					{
						SVMatroxBufferInterface::CopyBuffer( l_ToMilBuffer.GetBuffer(), l_FromMilBuffer.GetBuffer(), -l_lLeft, -l_lTop );
					}
				}

			  // Update Histogram graph
			  getHistogram( GetBand0HistogramImage(), aHistValueArray[0], &graphFigures[0] );
			  getHistogram( GetBand1HistogramImage(), aHistValueArray[1], &graphFigures[1] );
			  getHistogram( GetBand2HistogramImage(), aHistValueArray[2], &graphFigures[2] );
		  
			  // Update threshold bars
			  updateThresholdBars( GetBand0InputImage(), &thresholdBarFigures[0], band0Lower, band0Upper );
			  updateThresholdBars( GetBand1InputImage(), &thresholdBarFigures[1], band1Lower, band1Upper );
			  updateThresholdBars( GetBand2InputImage(), &thresholdBarFigures[2], band2Lower, band2Upper );
		  }
    }
  }

	if( !l_bOk )
	{
		// Signal that something was wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}

BOOL SVColorThresholdClass::binarize( long lower, long upper, BOOL bExclude, SVImageClass* pInputImage, SVImageClass* pOutputImage )
{
	if( pInputImage && pOutputImage )
	{
		SVSmartHandlePointer InputImageHandle;
		SVSmartHandlePointer OutputImageHandle;

    SVImageBufferHandleImage l_InputMilBuffer;
    SVImageBufferHandleImage l_OutputMilBuffer;

		if ( pInputImage->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
			 pOutputImage->GetImageHandle( OutputImageHandle ) && !( OutputImageHandle.empty() ) )
		{
			InputImageHandle->GetData( l_InputMilBuffer );
			OutputImageHandle->GetData( l_OutputMilBuffer );
		}

		if( !( l_InputMilBuffer.empty() ) && !( l_OutputMilBuffer.empty() ) )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;
			double minVal;
			double maxVal;

			if( lower > upper)
			{
				minVal = (double)upper;
				maxVal = (double)lower;
			}
			else
			{
				minVal = (double)lower;
				maxVal = (double)upper;
			}

			if( bExclude )
			{
				l_Code = SVMatroxImageInterface::Binarize( l_OutputMilBuffer.GetBuffer(),  
					l_InputMilBuffer.GetBuffer(), 
					SVECondOutRange, minVal, maxVal );
				
			}
			else
			{
				l_Code = SVMatroxImageInterface::Binarize( l_OutputMilBuffer.GetBuffer(),  
					l_InputMilBuffer.GetBuffer(), 
					SVECondInRange, minVal, maxVal );
				
			}
			
			long l_lMILError( 0 );
			
			if( l_Code != SVMEE_STATUS_OK )
			{
				return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SVColorThresholdClass::getHistogram( SVImageClass* pInputImage, SVMatroxLongArray& pHistValues, SVDrawObjectClass* pGraphFigure )
{
	
	SVMatroxImageInterface::SVStatusCode l_Code;

	if( pInputImage && pGraphFigure )
	{
		SVImageBufferHandleImage l_MilBuffer;
    SVSmartHandlePointer ImageHandle;

		if ( pInputImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
		{
			ImageHandle->GetData( l_MilBuffer );
		}

		if( !( l_MilBuffer.empty() ) )
		{
			l_Code = SVMatroxImageInterface::Histogram(histResultID, l_MilBuffer.GetBuffer());
			  // Need to compute Histogram on a portion of the image...

				long l_MILError( 0 );

				if( l_Code != SVMEE_STATUS_OK )
				{
					return FALSE;
				}
				SVMatroxImageInterface::GetResult( histResultID, pHistValues );			
				
				// Calculate Graph...
				CPoint graphPoint;
				
				pGraphFigure->SetListSize( histValueArraySize );
				
				for( int i = 0; i < histValueArraySize; i++ )
				{
					// Get Pixel ...
					long dwPixel = pHistValues[ i ];
					
					// Must be weighted by color number!
					graphPoint.x = i;
					graphPoint.y = dwPixel;
					
					// Horizontal type
					//graphPoint.y = imageExtent.Height - graphPoint.y;
					//graphPoint.x += imageExtent.Left;
					//graphPoint.y += imageExtent.Top;
					
					pGraphFigure->SetPointAtGrow( i, graphPoint );
				}
				return  TRUE;
		}
	}
	return FALSE;
}

BOOL SVColorThresholdClass::updateThresholdBars( SVImageClass* pImage, SVDrawObjectListClass* pThresholdBarFigure, long lMinThresholdValue, long lMaxThresholdValue )
{
	if( pImage && pThresholdBarFigure )
	{
		pThresholdBarFigure->Flush();

		SVImageInfoClass ImageInfo = pImage->GetImageInfo();
    
		SVDrawObjectClass l_svDrawObject;

		l_svDrawObject.SetDrawPen( TRUE, PS_DOT, 1, SV_DEFAULT_GOOD_COLOR );

		POINT l_oPoint;
		CPoint graphPoint;
		
		long l_lHeight = 0;
		
    ImageInfo.GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint );
    ImageInfo.GetExtentProperty( SVExtentPropertyHeight, l_lHeight );
		
		// MaxThresholdBar
		// Max left...
		graphPoint.x = l_oPoint.x + lMaxThresholdValue;
		graphPoint.y = l_oPoint.y;

		l_svDrawObject.SetPointAtGrow( 0, graphPoint );

		// Max right...
		graphPoint.y = l_oPoint.y + l_lHeight;

		l_svDrawObject.SetPointAtGrow( 1, graphPoint );
		
		pThresholdBarFigure->Add( l_svDrawObject );

		// MinThresholdBar
		// Min left...
		graphPoint.x = l_oPoint.x + lMinThresholdValue;
		graphPoint.y = l_oPoint.y ;
		
		l_svDrawObject.SetPointAtGrow( 0, graphPoint );
		
		// Min right...
		graphPoint.y = l_oPoint.y + l_lHeight;
		
		l_svDrawObject.SetPointAtGrow( 1, graphPoint );
		
		pThresholdBarFigure->Add( l_svDrawObject );

		return TRUE;
	}
	return FALSE;
}

SVDrawObjectClass* SVColorThresholdClass::GetGraphFigure( int bandNumber )
{
	if( bandNumber >= 0 && bandNumber < 3 )
		return &graphFigures[bandNumber];

	return NULL;
}

SVDrawObjectListClass* SVColorThresholdClass::GetThresholdBarsFigure( int bandNumber )
{
	if( bandNumber >= 0 && bandNumber < 3 )
		return &thresholdBarFigures[bandNumber];

	return NULL;
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVColorThreshold.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:23:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:53:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   12 Jul 2012 14:37:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   18 Jun 2012 17:59:28   jspila
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
 *    Rev 1.37   26 Jul 2011 07:50:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  732
 * SCR Title:  Fix issue with the Color Tool's color threshold image extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with the Color Threshold Image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   13 Apr 2011 14:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Mar 2011 13:23:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   11 Mar 2011 14:17:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   01 Mar 2011 10:13:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   25 Feb 2011 12:07:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   10 Feb 2011 14:33:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   27 Jan 2011 10:58:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   08 Dec 2010 07:45:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   04 Nov 2010 13:30:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   01 Jun 2010 14:13:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   03 Sep 2009 09:59:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   30 Jul 2009 11:17:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   06 Jun 2008 09:20:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  628
 * SCR Title:  Make Color Tool Thresholds remotely settable
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Create Object to set attributes on thresholds so they are remotely settable and online settable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20.3.0   14 Jul 2009 12:27:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include new error codes and new get parent image method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   09 Apr 2008 07:37:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   07 Mar 2008 11:28:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   24 Jul 2007 11:26:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20.2.0   07 Apr 2008 07:36:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Feb 2007 16:34:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   15 Feb 2007 11:44:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.  Also removed the un-necessary string attribute to hold the last error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   24 Jan 2006 12:02:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to allow the offset extents to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Nov 2005 07:33:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   08 Aug 2005 15:31:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove unused method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   29 Jul 2005 12:17:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   21 Jun 2005 08:02:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Feb 2005 10:39:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image object to be physsical for histogram images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Feb 2005 13:24:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Nov 2003 12:22:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  395
 * SCR Title:  Color ROI does not update unless you move ROI in training
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Jul 2003 08:54:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 May 2003 17:44:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed the onRun of the color so that the Adjust light reference and adjust LUT will work and update in test mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Apr 2003 17:16:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Mar 2003 16:02:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated class to fix HSI problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jan 2003 12:44:58   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, createOutputImage, createHistogramImage, onRun, binarize, getHistogram and updateThresholdBars to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Jan 2003 13:44:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Dec 2002 19:01:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Nov 2002 11:02:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 *    Changed
 *        CreateObject
 *    Changed explicit MIL call to wrapper call
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 13:25:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Apr 17 2001 15:20:46   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:39:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
