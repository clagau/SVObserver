//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveToolClass
//* .File Name       : $Workfile:   SVPerspectiveTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:21:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVPerspectiveTool.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVImageProcessingClass.h"

SV_IMPLEMENT_CLASS( SVPerspectiveToolClass, SVPerspectiveToolClassGuid )

SVPerspectiveToolClass::SVPerspectiveToolClass( BOOL p_bCreateDefaultTaskList, SVObjectClass *p_pOwner, int p_iStringResourceID )
	:SVToolClass(p_bCreateDefaultTaskList, p_pOwner, p_iStringResourceID)
{
	LocalInitialize();
}

SVPerspectiveToolClass::~SVPerspectiveToolClass()
{
	LocalDestroy();
}

BOOL SVPerspectiveToolClass::CreateObject( SVObjectLevelCreateStruct *p_pCreateStructure )
{
	BOOL l_bOk = SVToolClass::CreateObject( p_pCreateStructure );

	l_bOk &= ( S_OK == m_OutputImage.InitializeImage( GetInputImage() ) );

	m_svSourceImageName.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	m_isCreated = l_bOk;

	return l_bOk;
}

BOOL SVPerspectiveToolClass::CloseObject()
{
	BOOL l_bOk = S_OK == LocalDestroy();

	l_bOk = SVToolClass::CloseObject() && l_bOk;

	return l_bOk;
}

HRESULT SVPerspectiveToolClass::UpdateOutputImageExtents()
{
	HRESULT l_hrOk;

	// Get Input Width and Height put in output Image Extent.
	SVImageExtentClass l_svInputExtents = GetInputImage()->GetImageExtents();
	SVImageExtentClass l_svOutputExtents = m_OutputImage.GetImageExtents();
	SVImageExtentClass l_svToolExtents;
	double l_dValue;
	l_hrOk = l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyWidth, l_dValue );

	l_hrOk = l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyHeight, l_dValue );

	l_hrOk = GetImageExtent( l_svToolExtents );

	// Set Translation
	bool l_bHideX;
	long l_lType;
	m_svWarpType.GetValue( l_lType );
	WarpType l_eType = ( WarpType ) l_lType;
	if( l_eType == WarpTypeHorizontal )
	{
		l_hrOk = l_svOutputExtents.SetTranslation( SVExtentTranslationHorizontalPerspective );
		l_bHideX = false;
	}
	else
	if( l_eType == WarpTypeVertical )
	{
		l_hrOk = l_svOutputExtents.SetTranslation( SVExtentTranslationVerticalPerspective );
		l_bHideX = true;
	}
	l_hrOk = l_svOutputExtents.UpdateData();




	// Set Position Points to zero for output image.
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyPositionPointX, 0 );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyPositionPointY, 0 );

	// Set tool Extent PositionPoints to Output Extent.
	l_hrOk = l_svToolExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dValue );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dValue );
	l_hrOk = l_svToolExtents.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dValue );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dValue );
	l_hrOk = SetImageExtent( 1, l_svOutputExtents );


	l_hrOk = l_svOutputExtents.UpdateData();

	SVGUID l_InputID;

	if( nullptr != GetInputImage() )
	{
		l_InputID = GetInputImage()->GetUniqueObjectID();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( l_svOutputExtents );

	l_hrOk = m_OutputImage.UpdateImage( l_InputID, l_ImageInfo);

	// Enable / Disable Extent Properties..
	SVExtentPropertyInfoStruct info;
	if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyWidth, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyWidth, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}
	if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyHeight, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyHeight, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}
	if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyPositionPointX, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyPositionPointX, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyPositionPointY, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyPositionPointY, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyTranslationOffsetX, info ) )
	{
		info.bHidden = l_bHideX;

		if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyTranslationOffsetX, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyTranslationOffsetY, info ) )
	{
		info.bHidden = !l_bHideX;

		if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyTranslationOffsetY, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVPerspectiveToolClass::ResetObject()
{
	HRESULT l_hrOk = UpdateOutputImageExtents();

	 l_hrOk = SVToolClass::ResetObject();
	
	// Now the input image is valid!
	if( S_OK == m_OutputImage.ResetObject() )
	{
		if( S_OK != CreateLUT() )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	SVImageClass *inputImage = GetInputImage();

	if (nullptr != inputImage)
	{
		//Set input name to source image name to display it in result picker
		m_svSourceImageName.SetValue( 0, inputImage->GetCompleteObjectName() );
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

BOOL SVPerspectiveToolClass::OnValidate()
{
	BOOL l_bOk = SVToolClass::OnValidate();

	if ( l_bOk )
	{
		l_bOk = !m_LutX.empty() && !m_LutY.empty();
	}


	return l_bOk;
}


SVImageClass* SVPerspectiveToolClass::GetInputImage()
{
	SVImageClass *l_pImage = nullptr;

	if( m_InputImageObjectInfo.IsConnected() && 
		nullptr != m_InputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		l_pImage = (SVImageClass *)m_InputImageObjectInfo.GetInputObjectInfo().PObject;
	}

	return l_pImage;
}

SVTaskObjectClass* SVPerspectiveToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVTaskObjectClass *l_psvObject = nullptr;

	SVImageExtentClass l_svExtents;

	if( S_OK == GetImageExtent( l_svExtents ) &&
	    l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVPerspectiveToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVPerspectiveToolClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage && p_psvImage == GetInputImage() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}


BOOL SVPerspectiveToolClass::onRun( SVRunStatusClass &p_rRunStatus )
{
	BOOL l_bOk = FALSE;

	if ( SVToolClass::onRun( p_rRunStatus ) )
	{
		SVImageClass *l_pInputImage = GetInputImage();

		SVImageExtentClass l_svToolExtents;
		l_bOk = S_OK == GetImageExtent(l_svToolExtents);

		SVImageExtentClass l_svInputExtents = l_pInputImage->GetImageExtents();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;

		long l_lInterpolation;
		m_svInterpolationMode.GetValue(p_rRunStatus.m_lResultDataIndex, l_lInterpolation);

		l_bOk = ( S_OK ==  l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dInputWidth ) ) && l_bOk;
		l_bOk = ( S_OK == l_svToolExtents.GetExtentProperty( SVExtentPropertyWidth, l_dToolWidth ) ) && l_bOk;
		l_bOk = ( S_OK == l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dInputHeight ) ) && l_bOk;
		l_bOk = ( S_OK == l_svToolExtents.GetExtentProperty( SVExtentPropertyHeight, l_dToolHeight ) ) && l_bOk;

		if( (l_dInputWidth != l_dToolWidth) || (l_dInputHeight != l_dToolHeight) )
		{
			ResetObject();
		}

		if ( nullptr != l_pInputImage &&
			   m_OutputImage.SetImageHandleIndex( p_rRunStatus.Images ) )
		{
			SVSmartHandlePointer l_InputHandle;
			SVSmartHandlePointer l_OutputHandle;

			if ( l_pInputImage->GetImageHandle( l_InputHandle ) && !( l_InputHandle.empty() ) &&
				   m_OutputImage.GetImageHandle( l_OutputHandle ) && !( l_OutputHandle.empty() ) )
			{
				SVImageBufferHandleImage l_InMilHandle;
				SVImageBufferHandleImage l_OutMilHandle;

				l_InputHandle->GetData( l_InMilHandle );
				l_OutputHandle->GetData( l_OutMilHandle );

				SVMatroxImageInterface::SVStatusCode l_Code;
				l_Code = SVMatroxImageInterface::Warp( l_OutMilHandle.GetBuffer(),
					l_InMilHandle.GetBuffer(), m_LutX, m_LutY, static_cast<SVImageOperationTypeEnum>(l_lInterpolation) );

			}
			else
				l_bOk = FALSE;
		}
	}


	if ( ! l_bOk )
	{
		p_rRunStatus.SetInvalid();
	}

	return l_bOk;
}

void SVPerspectiveToolClass::LocalInitialize()
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVPerspectiveToolObjectType;

	// Identify our input image...
	m_InputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "PerspectiveToolImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_svXOffset, SVTranslationXOffsetObjectGuid, IDS_X_OFFSET, false, SVResetItemTool ); 
	RegisterEmbeddedObject( &m_svYOffset, SVTranslationYOffsetObjectGuid, IDS_Y_OFFSET, false, SVResetItemTool ); 

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageName, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	HRESULT l_hr = SetImageExtentProperty( SVExtentPropertyTranslationOffsetX, &m_svXOffset );
	l_hr = SetImageExtentProperty( SVExtentPropertyTranslationOffsetY, &m_svYOffset );

	m_OutputImage.InitializeImage( SVImageTypePhysical );

	// Set Default Warp Method to Use Horizontal
	RegisterEmbeddedObject( &m_svWarpType, SVWarpTypeObjectGuid, IDS_OBJECTNAME_WARPTYPE, false, SVResetItemTool );
	CString strEnumTypes;
	strEnumTypes.Format("%s=%d,%s=%d", PERSPECTIVE_WARP_TYPE_HORIZONTAL, WarpTypeHorizontal,
	                                   PERSPECTIVE_WARP_TYPE_VERTICAL, WarpTypeVertical);
	m_svWarpType.SetEnumTypes( strEnumTypes );
	m_svWarpType.SetDefaultValue( PERSPECTIVE_WARP_TYPE_VERTICAL, TRUE );
	m_svWarpType.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Set Default Interpolation Mode to use Nearest Neighbor
	CString strMode;
	CString strPrepare;
	strEnumTypes.Empty();

	// M_NEAREST_NEIGHBOR 
	strMode.LoadString( IDS_NEAREST_NEIGHBOR_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVNearestNeighbor);
	strEnumTypes += strPrepare;
	// M_BILINEAR
	strMode.LoadString( IDS_BILINEAR_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVBilinear ); // M_BILINEAR );
	strEnumTypes += strPrepare;
	// M_BICUBIC
	strMode.LoadString( IDS_BICUBIC_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVBiCubic ); // M_BICUBIC );
	strEnumTypes += strPrepare;

	// And now set enum types...
	m_svInterpolationMode.SetEnumTypes( strEnumTypes );
	m_svInterpolationMode.SetDefaultValue( SVNearestNeighbor, TRUE );	// Refer to MIL...
	RegisterEmbeddedObject( &m_svInterpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SVResetItemNone );

	// Initialize MIL Look up tables.

	// Add Default Inputs and Outputs
	addDefaultInputObjects();


}

HRESULT SVPerspectiveToolClass::LocalDestroy()
{
	HRESULT l_hrOk = S_OK;

	if ( S_OK != DestroyLUT() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVPerspectiveToolClass::CreateLUT()
{
	HRESULT l_hrOk = S_FALSE;

	if ( S_OK == DestroyLUT() )
	{
		SVImageExtentClass l_svOutputExtents;

		SVMatroxImageInterface::SVStatusCode l_Code;

		long l_lWidth = 100;
		long l_lHeight = 100;
		if ( nullptr != GetInputImage() )
		{
			HRESULT l_hr = GetImageExtent(l_svOutputExtents);

			l_svOutputExtents.GetExtentProperty( SVExtentPropertyOutputWidth , l_lWidth );
			l_svOutputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_lHeight );
			SVMatroxBufferCreateStruct l_Create;
			l_Create.m_eAttribute = SVBufAttLut;
			l_Create.m_eType = SV32BitSigned;
			l_Create.m_lSizeBand = 1;
			l_Create.m_lSizeX = l_lWidth;
			l_Create.m_lSizeY = l_lHeight;
			l_Code = SVMatroxBufferInterface::Create( m_LutX, l_Create );
			l_Code = SVMatroxBufferInterface::Create( m_LutY, l_Create );
		}
		SVExtentPointStruct l_svPoint;

		if ( !m_LutX.empty() && !m_LutY.empty() )
		{

			long l_lPitchX = 0; //MbufInquire( m_LutX, M_PITCH, M_NULL );
			long l_lPitchY = 0; //MbufInquire( m_LutY, M_PITCH, M_NULL );
			l_Code = SVMatroxBufferInterface::Get( m_LutX, SVPitch, l_lPitchX );
			l_Code = SVMatroxBufferInterface::Get( m_LutY, SVPitch, l_lPitchY );

			long *l_plLutXData = nullptr; //(long *)MbufInquire( m_LutX, M_HOST_ADDRESS, M_NULL );
			long *l_plLutYData = nullptr; // (long *)MbufInquire( m_LutY, M_HOST_ADDRESS, M_NULL );
			l_Code = SVMatroxBufferInterface::GetHostAddress( &l_plLutXData, m_LutX );
			l_Code = SVMatroxBufferInterface::GetHostAddress( &l_plLutYData,m_LutY );

			for ( long j = 0; j < l_lHeight; j++ )
			{
				for ( long i = 0; i < l_lWidth; i++ )
				{
					l_svPoint.m_dPositionX = i;
					l_svPoint.m_dPositionY = j;
					l_svOutputExtents.TranslateFromOutputSpace( l_svPoint, l_svPoint );
					l_plLutXData[ i + ( j * l_lPitchX ) ] = (long)( l_svPoint.m_dPositionX * 256L );
					l_plLutYData[ i + ( j * l_lPitchY ) ] = (long)( l_svPoint.m_dPositionY * 256L );
				}
			}

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVPerspectiveToolClass::DestroyLUT()
{
	HRESULT l_Status = S_OK;	

	m_LutX.clear();
	m_LutY.clear();

	return l_Status;
}

HRESULT SVPerspectiveToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	double l_dOffset;
	double l_dValue;
	SVExtentTranslationEnum l_eTrans = p_svImageExtent.GetTranslation( );

	if( l_eTrans == SVExtentTranslationVerticalPerspective )
	{
		l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dOffset );
		if( S_OK != p_svImageExtent.GetExtentProperty( SVExtentPropertyOutputHeight, l_dValue) )
		{
			l_hrOk = S_FALSE;
		}
		if( S_OK == l_hrOk && l_dOffset >= l_dValue )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dValue - 1 );
		}
		if( S_OK == l_hrOk &&  l_dOffset < 0 )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 0 );
		}
		// X is always default with a Horizontal
		p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 0);
	}
	if( l_eTrans == SVExtentTranslationHorizontalPerspective )
	{
		l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dOffset );
		if( S_OK != p_svImageExtent.GetExtentProperty( SVExtentPropertyOutputWidth, l_dValue) )
		{
			l_hrOk = S_FALSE;
		}
		if( S_OK == l_hrOk && l_dOffset >= l_dValue )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dValue - 1 );
		}
		if( S_OK == l_hrOk &&  l_dOffset < 0 )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 0 );
		}
		// Y is always default with a Vertical
		p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 0);
	}

	l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );

	return l_hrOk;
}

SVStaticStringValueObjectClass* SVPerspectiveToolClass::GetInputImageNames()
{
	return &m_svSourceImageName;
}

