//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorTool
//* .File Name       : $Workfile:   SVColorTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVColorTool.h"
#include "SVInspectionProcess.h"
#include "SVColorThreshold.h"
#include "SVGlobal.h"
#include "SVRGBMainImage.h"
#pragma endregion Includes

enum BandNUmberEnum
{
	Band0 = 0,
	Band1 = 1,
	Band2 = 2
};

SV_IMPLEMENT_CLASS( SVColorToolClass, SVColorToolClassGuid );

SVColorToolClass::SVColorToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVColorToolClass::init()
{
 	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVColorToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &band0Image, SVBand0ImageObjectGuid, IDS_OBJECTNAME_BAND0_IMAGE );
	RegisterEmbeddedObject( &band1Image, SVBand1ImageObjectGuid, IDS_OBJECTNAME_BAND1_IMAGE );
	RegisterEmbeddedObject( &band2Image, SVBand2ImageObjectGuid, IDS_OBJECTNAME_BAND2_IMAGE );

	RegisterEmbeddedObject( &convertToHSI, SVConvertToHSIObjectGuid, IDS_OBJECTNAME_CONVERT_TO_HSI, true, SVResetItemIP );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	band0Image.InitializeImage( SVImageTypeDependent );
	band1Image.InitializeImage( SVImageTypeDependent );
	band2Image.InitializeImage( SVImageTypeDependent );

	extentTop.SetDefaultValue( 0.0, TRUE );
	extentLeft.SetDefaultValue( 0.0, TRUE );

	m_svToolExtent.SetTranslation( SVExtentTranslationNone );

	// Add the Color Threshold class object
	SVColorThresholdClass* pColorThreshold = new SVColorThresholdClass;
	if( pColorThreshold )
	{
		Add( pColorThreshold );
	}
}

SVColorToolClass::~SVColorToolClass()
{
}

BOOL SVColorToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		// Create 3 output images, one for each band...
		SVImageClass* pInputImage = nullptr;

		BOOL useHSI = TRUE;
		// Create HSI Image if required
		convertToHSI.GetValue( useHSI );

		if( useHSI )
		{
			pInputImage = GetHSIImage();
		}
		else
		{
			pInputImage = GetRGBImage();
		}

		if( pInputImage )
		{
			band0Image.setDibBufferFlag(false);
			band1Image.setDibBufferFlag(false);
			band2Image.setDibBufferFlag(false);

			// Create 3 child layers...
			
			// Create band 0 Layer
			bOk = createBandChildLayer( band0Image, pInputImage, 0 );

			// Create Band 1 Layer
			bOk &= createBandChildLayer( band1Image, pInputImage, 1 );

			// Create Band 2 Layer...
			bOk &= createBandChildLayer( band2Image, pInputImage, 2 );
		}
	}
	
	extentTop.SetDefaultValue( 0.0, TRUE );
	extentLeft.SetDefaultValue( 0.0, TRUE );

	// Set / Reset Printable Flag
	convertToHSI.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Color Tool.
	drawToolFlag.ObjectAttributesAllowedRef() = SV_HIDDEN;

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Color Tool.
	m_svUpdateAuxilliaryExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxilliarySourceX.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxilliarySourceY.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxilliarySourceAngle.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxilliarySourceImageName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxilliaryDrawType.ObjectAttributesAllowedRef() = SV_HIDDEN;

	isCreated = bOk;

	return bOk;
}

BOOL SVColorToolClass::CloseObject()
{
	return SVToolClass::CloseObject();
}

HRESULT SVColorToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	// Create 3 output images, one for each band...
	SVImageClass* pInputImage = nullptr;

	BOOL useHSI = TRUE;

	// Create HSI Image if required
	convertToHSI.GetValue( useHSI );

	if( useHSI )
	{
		pInputImage = GetHSIImage();
	}
	else
	{
		pInputImage = GetRGBImage();
	}

	// From the Matrox Imaging Library Help file for MbufChildColor2d, the Band parameter specifies the index of the band to use. 
	// Valid index values are from 0 to (number of bands of the buffer - 1). 
	// Band 0 corresponds to: the red band (for RGB parent buffers), the hue band (for HSL parent buffers), and the Y band (for YUV parent buffers). 
	// Band 1 corresponds to: the green band (for RGB parent buffers), the saturation band (for HSL parent buffers), and the U band (for YUV parent buffers). 
	// Band 2 corresponds to: the blue band (for RGB parent buffers), the luminance band (for HSL parent buffers), and the V band (for YUV parent buffers). 

	createBandChildLayer( band0Image, pInputImage, Band0 );
	band0Image.ResetObject();

	// Create Band 1 Layer
	createBandChildLayer( band1Image, pInputImage, Band1 );
	band1Image.ResetObject();

	// Create Band 2 Layer...
	createBandChildLayer( band2Image, pInputImage, Band2 );
	band2Image.ResetObject();

	if( S_OK != SVToolClass::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

	CollectInputImageNames();

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

bool SVColorToolClass::DoesObjectHaveExtents() const
{
	return false;
}

HRESULT SVColorToolClass::UpdateImageWithExtent( unsigned long p_Index )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.UpdateImageWithExtent( p_Index, SVToolExtentClass::SVColorToolExtent );

	return l_hrOk;
}

BOOL SVColorToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	// Create 3 output images, one for each band...
	SVImageClass* pInputImage = nullptr;

	BOOL useHSI = TRUE;

	// Create HSI Image if required
	convertToHSI.GetValue( useHSI );

	if( useHSI )
	{
		pInputImage = GetHSIImage();
	}
	else
	{
		pInputImage = GetRGBImage();
	}

	if( nullptr != pInputImage )
	{
		if( band0Image.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
		{
			band0Image.ResetObject();
		}

		if( band1Image.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
		{
			band1Image.ResetObject();
		}

		if( band2Image.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
		{
			band2Image.ResetObject();
		}
	}

	return SVToolClass::onRun( RRunStatus );
}

BOOL SVColorToolClass::createBandChildLayer( SVImageClass& p_rOutputImage, SVImageClass* p_pInputImage, long p_BandLink )
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
	ImageInfo.SetOwner( GetUniqueObjectID() );
	ImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
	ImageInfo.SetImageProperty( SVImagePropertyBandLink, p_BandLink );
	
	// Restore Output Image Extents
	// Note: this must now be done after setting SVImageInfoClass::POwnerTool
	// So Shadowed Extents get Updated
			
	// Try to create image object...
	l_bOk = ( S_OK == p_rOutputImage.InitializeImage( l_InputID, ImageInfo ) );

	return l_bOk;
}

SVImageClass* SVColorToolClass::GetRGBImage()
{
	SVImageClass* pImage = nullptr;

	// Ask the Document for this directly...

	SVInspectionProcess* pInspection = GetInspection();
	if( nullptr != pInspection )
	{
		pImage = pInspection->GetRGBMainImage();
	}

	return pImage;
}

SVImageClass* SVColorToolClass::GetHSIImage()
{
	SVImageClass* pImage = nullptr;

	// Ask the Document for this directly...

	SVInspectionProcess* pInspection = GetInspection();
	if( nullptr != pInspection )
	{
		pImage = pInspection->GetHSIMainImage();
	}

	return pImage;
}

SVBoolValueObjectClass* SVColorToolClass::GetConvertToHSIVariable()
{
	return &convertToHSI;
}

DWORD_PTR SVColorToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = 0;
	
	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

// Set String value object for Source Image Names
HRESULT SVColorToolClass::CollectInputImageNames( )
{
	CString l_strName;
	SVImageClass* l_pImage = nullptr;
	bool l_bConvertToHSI;
	HRESULT l_hr = convertToHSI.GetValue( l_bConvertToHSI );
	if( S_OK == l_hr )
	{
		if( l_bConvertToHSI )
		{
			l_pImage = GetHSIImage();
		}
		else
		{
			l_pImage = GetRGBImage();
		}
		if( nullptr != l_pImage )
		{
			l_strName = l_pImage->GetCompleteObjectName();
			m_svSourceImageNames.SetDefaultValue( l_strName, true );
		}
	}
	return l_hr;
}

SVStaticStringValueObjectClass* SVColorToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}

