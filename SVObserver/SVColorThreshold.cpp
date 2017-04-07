//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorThreshold
//* .File Name       : $Workfile:   SVColorThreshold.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:10:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVColorThreshold.h"

#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVOCore/SVDataBuffer.h"
#include "ObjectInterfaces/GlobalConst.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVTool.h"
#pragma endregion Includes

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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVColorThresholdObjectType;

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
	RegisterEmbeddedObject( &band0UpperThreshold, SVBand0UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND0_UPPERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &band0LowerThreshold, SVBand0LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND0_LOWERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &band0ThresholdExclude, SVBand0ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND0_THRESHOLDEXCLUDE, false, SvOi::SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Band 1 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &band1UpperThreshold, SVBand1UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND1_UPPERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &band1LowerThreshold, SVBand1LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND1_LOWERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &band1ThresholdExclude, SVBand1ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND1_THRESHOLDEXCLUDE, false, SvOi::SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Band 2 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &band2UpperThreshold, SVBand2UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND2_UPPERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &band2LowerThreshold, SVBand2LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND2_LOWERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &band2ThresholdExclude, SVBand2ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND2_THRESHOLDEXCLUDE, false, SvOi::SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Train Color ROI extents
	/////////////////////////////////////////////////////////////////////////
	//Special type names for extents
	extentWidth.SetTypeName( _T("Extent Width") );
	extentHeight.SetTypeName( _T("Extent Height") );
	extentLeft.SetTypeName( _T("Extent X") );
	extentTop.SetTypeName( _T("Extent Y") );
	RegisterEmbeddedObject( &extentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &extentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &extentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &extentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemOwner );

	// Set Embedded defaults
	band0UpperThreshold.SetDefaultValue( SvOi::cDefaultToolUpperThreshold, TRUE );
	band0LowerThreshold.SetDefaultValue( SvOi::cDefaultToolLowerThreshold, TRUE );
	band0ThresholdExclude.SetDefaultValue( FALSE, TRUE );

	band1UpperThreshold.SetDefaultValue( SvOi::cDefaultToolUpperThreshold, TRUE );
	band1LowerThreshold.SetDefaultValue( SvOi::cDefaultToolLowerThreshold, TRUE );
	band1ThresholdExclude.SetDefaultValue( FALSE, TRUE );

	band2UpperThreshold.SetDefaultValue( SvOi::cDefaultToolUpperThreshold, TRUE );
	band2LowerThreshold.SetDefaultValue( SvOi::cDefaultToolLowerThreshold, TRUE );
	band2ThresholdExclude.SetDefaultValue( FALSE, TRUE );
		
	extentLeft.SetDefaultValue( SvOi::cDefaultWindowToolLeft, TRUE );
	extentTop.SetDefaultValue( SvOi::cDefaultWindowToolTop, TRUE );
	extentWidth.SetDefaultValue( SvOi::cDefaultWindowToolWidth, TRUE );
	extentHeight.SetDefaultValue( SvOi::cDefaultWindowToolHeight, TRUE );
	
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
		  if ( S_OK == SVImageProcessingClass::CreateDataBuffer( &svData )  )
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
				  
				  CreateChildObject(&band0HistogramImage);
				  CreateChildObject(&band1HistogramImage);
				  CreateChildObject(&band2HistogramImage);
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
	const UINT cAttributes = SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	// Set / Reset Printable Flags 
	band0UpperThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band0LowerThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band0ThresholdExclude.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band1UpperThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band1LowerThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band1ThresholdExclude.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band2UpperThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band2LowerThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	band2ThresholdExclude.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	extentLeft.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	extentTop.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	extentWidth.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	extentHeight.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	m_isCreated = bOk;

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

	if( nullptr != p_pInputImage )
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

	l_bOk = ( S_OK == p_rOutputImage.UpdateImage( l_InputID, ImageInfo ) );

	return l_bOk;
}

BOOL SVColorThresholdClass::createHistogramImage( SVImageClass* p_pInputImage, SVImageClass& p_rOutputImage )
{
	BOOL l_bOk = FALSE;

	SVGUID l_InputID;
	SVImageInfoClass ImageInfo;

	if( nullptr != p_pInputImage )
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
  l_bOk = ( S_OK == p_rOutputImage.UpdateImage( l_InputID, ImageInfo ) );

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


bool SVColorThresholdClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
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

	bool Result = ValidateLocal();
	if (!Result)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return SVOperatorClass::ResetObject(pErrorMessages) && Result;
}

SVImageClass* SVColorThresholdClass::GetBand0InputImage()
{
	if( inputBand0Image.IsConnected() && inputBand0Image.GetInputObjectInfo().m_pObject )
		return ( SVImageClass * )inputBand0Image.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVImageClass* SVColorThresholdClass::GetBand1InputImage()
{
	if( inputBand1Image.IsConnected() && inputBand1Image.GetInputObjectInfo().m_pObject )
		return ( SVImageClass * )inputBand1Image.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVImageClass* SVColorThresholdClass::GetBand2InputImage()
{
	if( inputBand2Image.IsConnected() && inputBand2Image.GetInputObjectInfo().m_pObject )
		return ( SVImageClass * )inputBand2Image.GetInputObjectInfo().m_pObject;

	return nullptr;
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

bool SVColorThresholdClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	bool l_bOk = true;

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

	if ( !band0OutputImage.SetImageHandleIndex( rRunStatus.Images ) || !band1OutputImage.SetImageHandleIndex( rRunStatus.Images ) ||
		  !band2OutputImage.SetImageHandleIndex( rRunStatus.Images ) || !outputImage.SetImageHandleIndex( rRunStatus.Images ) )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SetImageHandleIndexFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( l_bOk )
	{
		BOOL isBinarize = binarize( band0Lower, band0Upper, bBand0ThresholdExclude, GetBand0InputImage(), &band0OutputImage );
		isBinarize &= binarize( band1Lower, band1Upper, bBand1ThresholdExclude, GetBand1InputImage(), &band1OutputImage );
		isBinarize &= binarize( band2Lower, band2Upper, bBand2ThresholdExclude, GetBand2InputImage(), &band2OutputImage );

		if (!isBinarize)
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_BinarizeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
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

		l_bOk = l_bOk && band0OutputImage.GetImageHandle( Band0Handle );
		l_bOk = l_bOk && band1OutputImage.GetImageHandle( Band1Handle );
		l_bOk = l_bOk && band2OutputImage.GetImageHandle( Band2Handle );

		l_bOk = l_bOk && outputImage.GetImageHandle( OutputHandle );

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

		if (!l_bOk)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
		else
		{

			SVMatroxImageInterface::Arithmetic( l_OutputMilBuffer.GetBuffer(), l_Band0MilBuffer.GetBuffer(), l_Band1MilBuffer.GetBuffer(), SVImageAnd);
			SVMatroxImageInterface::Arithmetic( l_OutputMilBuffer.GetBuffer(), l_Band2MilBuffer.GetBuffer(), l_OutputMilBuffer.GetBuffer(), SVImageAnd);
			// Do Image Arithemetic - AND the 3 outputs together

			if( bShowHistogram )
			{
				long lTop = 0;
				long lLeft = 0;

				double Value( 0.0 );
				extentTop.GetValue( Value );
				lTop = static_cast<long> (Value);
				extentLeft.GetValue( Value );
				lLeft = static_cast<long> (Value);

				SVSmartHandlePointer ImageHandle;
				SVSmartHandlePointer InputImageHandle;

				band0HistogramImage.SetImageHandleIndex( rRunStatus.Images );
				band1HistogramImage.SetImageHandleIndex( rRunStatus.Images );
				band2HistogramImage.SetImageHandleIndex( rRunStatus.Images );

				if( nullptr != GetBand0InputImage() && 
					GetBand0InputImage()->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
					band0HistogramImage.GetImageHandle( ImageHandle )  && !( ImageHandle.empty() )	)
				{
					SVImageBufferHandleImage l_FromMilBuffer;
					SVImageBufferHandleImage l_ToMilBuffer;

					InputImageHandle->GetData( l_FromMilBuffer );
					ImageHandle->GetData( l_ToMilBuffer );

					if( !( l_FromMilBuffer.empty() ) && !( l_ToMilBuffer.empty() ) )
					{
						SVMatroxBufferInterface::CopyBuffer( l_ToMilBuffer.GetBuffer(), l_FromMilBuffer.GetBuffer(), -lLeft, -lTop );
					}
				}

				if( nullptr != GetBand1InputImage() && 
					GetBand1InputImage()->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
					band1HistogramImage.GetImageHandle( ImageHandle )  && !( ImageHandle.empty() )	)
				{
					SVImageBufferHandleImage l_FromMilBuffer;
					SVImageBufferHandleImage l_ToMilBuffer;

					InputImageHandle->GetData( l_FromMilBuffer );
					ImageHandle->GetData( l_ToMilBuffer );

					if( !( l_FromMilBuffer.empty() ) && !( l_ToMilBuffer.empty() ) )
					{
						SVMatroxBufferInterface::CopyBuffer( l_ToMilBuffer.GetBuffer(), l_FromMilBuffer.GetBuffer(), -lLeft, -lTop );
					}
				}

				if( nullptr != GetBand2InputImage() && 
					GetBand2InputImage()->GetImageHandle( InputImageHandle ) && !( InputImageHandle.empty() ) &&
					band2HistogramImage.GetImageHandle( ImageHandle )  && !( ImageHandle.empty() )	)
				{
					SVImageBufferHandleImage l_FromMilBuffer;
					SVImageBufferHandleImage l_ToMilBuffer;

					InputImageHandle->GetData( l_FromMilBuffer );
					ImageHandle->GetData( l_ToMilBuffer );

					if( !( l_FromMilBuffer.empty() ) && !( l_ToMilBuffer.empty() ) )
					{
						SVMatroxBufferInterface::CopyBuffer( l_ToMilBuffer.GetBuffer(), l_FromMilBuffer.GetBuffer(), -lLeft, -lTop );
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
		rRunStatus.SetInvalid();
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

	return nullptr;
}

SVDrawObjectListClass* SVColorThresholdClass::GetThresholdBarsFigure( int bandNumber )
{
	if( bandNumber >= 0 && bandNumber < 3 )
		return &thresholdBarFigures[bandNumber];

	return nullptr;
}

bool SVColorThresholdClass::ValidateLocal() const
{
	// Validate that we have inputs and outputs
	if( inputBand0Image.IsConnected() && 
		inputBand0Image.GetInputObjectInfo().m_pObject && 
		inputBand0Image.GetInputObjectInfo().m_pObject->IsValid() &&
		inputBand1Image.IsConnected() && 
		inputBand1Image.GetInputObjectInfo().m_pObject && 
		inputBand1Image.GetInputObjectInfo().m_pObject->IsValid() &&
		inputBand2Image.IsConnected() && 
		inputBand2Image.GetInputObjectInfo().m_pObject && 
		inputBand2Image.GetInputObjectInfo().m_pObject->IsValid() &&
		band0OutputImage.IsCreated() && band0OutputImage.IsValid() && 
		band1OutputImage.IsCreated() && band1OutputImage.IsValid() &&
		band2OutputImage.IsCreated() && band2OutputImage.IsValid() &&
		outputImage.IsCreated() && outputImage.IsValid() )
	{
		return true;
	}
	return false;
}
