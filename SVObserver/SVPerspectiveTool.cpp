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

#pragma region Includes
#include "stdafx.h"
#include "SVPerspectiveTool.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS( SVPerspectiveToolClass, SVPerspectiveToolClassGuid )

static const TCHAR* const PERSPECTIVE_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
static const TCHAR* const PERSPECTIVE_WARP_TYPE_VERTICAL = _T("Vertical Warp");

SVPerspectiveToolClass::SVPerspectiveToolClass( BOOL p_bCreateDefaultTaskList, SVObjectClass *p_pOwner, int p_iStringResourceID )
	:SVToolClass(p_bCreateDefaultTaskList, p_pOwner, p_iStringResourceID)
{
	LocalInitialize();
}

SVPerspectiveToolClass::~SVPerspectiveToolClass()
{
	DestroyLUT();
}

BOOL SVPerspectiveToolClass::CreateObject( SVObjectLevelCreateStruct *p_pCreateStructure )
{
	BOOL l_bOk = SVToolClass::CreateObject( p_pCreateStructure );

	l_bOk &= ( S_OK == m_OutputImage.InitializeImage( GetInputImage() ) );

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = l_bOk;

	return l_bOk;
}

BOOL SVPerspectiveToolClass::CloseObject()
{
	DestroyLUT();

	return SVToolClass::CloseObject();
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

bool SVPerspectiveToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	UpdateOutputImageExtents();

	bool Result = SVToolClass::ResetObject(pErrorMessages);
	
	// Now the input image is valid!
	if( m_OutputImage.ResetObject(pErrorMessages) )
	{
		if( S_OK != CreateLUT() )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateLutFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
	else
	{
		Result = false;
	}

	SVImageClass *inputImage = GetInputImage();

	if (nullptr != inputImage)
	{
		//Set input name to source image name to display it in result picker
		m_SourceImageNames.SetValue( inputImage->GetCompleteName() );
	}

	UpdateImageWithExtent( 1 );

	return Result;
}

SVImageClass* SVPerspectiveToolClass::GetInputImage()
{
	SVImageClass *l_pImage = nullptr;

	if( m_InputImageObjectInfo.IsConnected() && 
		nullptr != m_InputImageObjectInfo.GetInputObjectInfo().m_pObject )
	{
		l_pImage = (SVImageClass *)m_InputImageObjectInfo.GetInputObjectInfo().m_pObject;
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


bool SVPerspectiveToolClass::onRun( SVRunStatusClass &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool l_bOk = SVToolClass::onRun( p_rRunStatus, pErrorMessages );

	if ( l_bOk )
	{
		SVImageClass *l_pInputImage = GetInputImage();

		SVImageExtentClass l_svToolExtents;
		l_bOk = S_OK == GetImageExtent(l_svToolExtents);

		SVImageExtentClass l_svInputExtents = l_pInputImage->GetImageExtents();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;

		long Interpolation;
		m_svInterpolationMode.GetValue(Interpolation, p_rRunStatus.m_lResultDataIndex );

		l_bOk = ( S_OK ==  l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dInputWidth ) ) && l_bOk;
		l_bOk = ( S_OK == l_svToolExtents.GetExtentProperty( SVExtentPropertyWidth, l_dToolWidth ) ) && l_bOk;
		l_bOk = ( S_OK == l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dInputHeight ) ) && l_bOk;
		l_bOk = ( S_OK == l_svToolExtents.GetExtentProperty( SVExtentPropertyHeight, l_dToolHeight ) ) && l_bOk;

		if (!l_bOk && nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}

		if( (l_dInputWidth != l_dToolWidth) || (l_dInputHeight != l_dToolHeight) )
		{
			l_bOk = ResetObject(pErrorMessages) && l_bOk;
		}

		if ( nullptr != l_pInputImage && m_OutputImage.SetImageHandleIndex( p_rRunStatus.Images ) )
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

				HRESULT l_Code;
				l_Code = SVMatroxImageInterface::Warp( l_OutMilHandle.GetBuffer(), 
					l_InMilHandle.GetBuffer(), m_LutX, m_LutY, static_cast<SVImageOperationTypeEnum>(Interpolation) );

			}
			else
			{
				l_bOk = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVPerspectiveToolObjectType;

	// Identify our input image...
	m_InputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "PerspectiveToolImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_svXOffset, SVTranslationXOffsetObjectGuid, IDS_X_OFFSET, false, SvOi::SVResetItemTool ); 
	RegisterEmbeddedObject( &m_svYOffset, SVTranslationYOffsetObjectGuid, IDS_Y_OFFSET, false, SvOi::SVResetItemTool ); 

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	HRESULT l_hr = SetImageExtentProperty( SVExtentPropertyTranslationOffsetX, &m_svXOffset );
	l_hr = SetImageExtentProperty( SVExtentPropertyTranslationOffsetY, &m_svYOffset );

	m_OutputImage.InitializeImage( SvOi::SVImageTypeEnum::SVImageTypePhysical );

	// Set Default Warp Method to Use Horizontal
	RegisterEmbeddedObject( &m_svWarpType, SVWarpTypeObjectGuid, IDS_OBJECTNAME_WARPTYPE, false, SvOi::SVResetItemTool );
	SVString EnumTypes = SvUl_SF::Format("%s=%d,%s=%d", PERSPECTIVE_WARP_TYPE_HORIZONTAL, WarpTypeHorizontal,
	                                   PERSPECTIVE_WARP_TYPE_VERTICAL, WarpTypeVertical);
	m_svWarpType.SetEnumTypes( EnumTypes.c_str() );
	m_svWarpType.SetDefaultValue( PERSPECTIVE_WARP_TYPE_VERTICAL, true );
	m_svWarpType.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	// Set Default Interpolation Mode to use Nearest Neighbor
	SVString Mode;
	SVString Text;
	EnumTypes.clear();

	// M_NEAREST_NEIGHBOR 
	Mode = SvUl_SF::LoadSVString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = SvUl_SF::Format( _T( "%s=%d," ), Mode.c_str(), SVNearestNeighbor);
	EnumTypes += Text;
	// M_BILINEAR
	Mode = SvUl_SF::LoadSVString( IDS_BILINEAR_STRING );
	Text = SvUl_SF::Format( _T( "%s=%d," ), Mode.c_str(), SVBilinear ); // M_BILINEAR );
	EnumTypes += Text;
	// M_BICUBIC
	Mode = SvUl_SF::LoadSVString( IDS_BICUBIC_STRING );
	Text = SvUl_SF::Format( _T( "%s=%d," ), Mode.c_str(), SVBiCubic ); // M_BICUBIC );
	EnumTypes += Text;

	// And now set enum types...
	m_svInterpolationMode.SetEnumTypes( EnumTypes.c_str() );
	m_svInterpolationMode.SetDefaultValue( SVNearestNeighbor, TRUE );	// Refer to MIL...
	RegisterEmbeddedObject( &m_svInterpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );

	// Initialize MIL Look up tables.

	// Add Default Inputs and Outputs
	addDefaultInputObjects();


}

HRESULT SVPerspectiveToolClass::CreateLUT()
{
	HRESULT l_hrOk = S_FALSE;

	DestroyLUT();
	
	SVImageExtentClass l_svOutputExtents;

	HRESULT l_Code;

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

	return l_hrOk;
}

void SVPerspectiveToolClass::DestroyLUT()
{
	m_LutX.clear();
	m_LutY.clear();
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

SVStringValueObjectClass* SVPerspectiveToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

