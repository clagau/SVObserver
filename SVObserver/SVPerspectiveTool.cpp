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
#include "SVGageTool.h"

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

	l_bOk &= ( m_OutputImage.InitializeImage( GetInputImage() ) == S_OK );

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = l_bOk;

	return l_bOk;
}

BOOL SVPerspectiveToolClass::CloseObject()
{
	BOOL l_bOk = LocalDestroy() == S_OK;

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

	if( GetInputImage() != NULL )
	{
		l_InputID = GetInputImage()->GetUniqueObjectID();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( l_svOutputExtents );

	l_hrOk = m_OutputImage.UpdateImage( l_InputID, l_ImageInfo);

	// Enable / Disable Extent Properties..
	SVExtentPropertyInfoStruct info;
	if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyWidth, info ) == S_OK )
	{
		info.bHidden = true;

		if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyWidth, info ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}
	if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyHeight, info ) == S_OK )
	{
		info.bHidden = true;

		if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyHeight, info ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}
	if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyPositionPointX, info ) == S_OK )
	{
		info.bHidden = true;

		if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyPositionPointX, info ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyPositionPointY, info ) == S_OK )
	{
		info.bHidden = true;

		if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyPositionPointY, info ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}


	if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyTranslationOffsetX, info ) == S_OK )
	{
		info.bHidden = l_bHideX;

		if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyTranslationOffsetX, info ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyTranslationOffsetY, info ) == S_OK )
	{
		info.bHidden = !l_bHideX;

		if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyTranslationOffsetY, info ) != S_OK )
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
	if( m_OutputImage.ResetObject() == S_OK )
	{
		if( CreateLUT() != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
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
	SVImageClass *l_pImage = NULL;

	if( m_InputImageObjectInfo.IsConnected() && 
		  m_InputImageObjectInfo.GetInputObjectInfo().PObject != NULL )
	{
		l_pImage = (SVImageClass *)m_InputImageObjectInfo.GetInputObjectInfo().PObject;
	}

	return l_pImage;
}

SVTaskObjectClass* SVPerspectiveToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVTaskObjectClass *l_psvObject = NULL;

	SVImageExtentClass l_svExtents;

	if( GetImageExtent( l_svExtents ) == S_OK &&
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

	if ( p_psvImage != NULL && p_psvImage == GetInputImage() )
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

		CollectInputImageNames(p_rRunStatus);

		SVImageExtentClass l_svToolExtents;
		l_bOk = GetImageExtent(l_svToolExtents) == S_OK;

		SVImageExtentClass l_svInputExtents = l_pInputImage->GetImageExtents();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;

		long l_lInterpolation;
		m_svInterpolationMode.GetValue(p_rRunStatus.m_lResultDataIndex, l_lInterpolation);

		l_bOk = ( l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dInputWidth ) == S_OK ) && l_bOk;
		l_bOk = ( l_svToolExtents.GetExtentProperty( SVExtentPropertyWidth, l_dToolWidth ) == S_OK ) && l_bOk;
		l_bOk = ( l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dInputHeight ) == S_OK ) && l_bOk;
		l_bOk = ( l_svToolExtents.GetExtentProperty( SVExtentPropertyHeight, l_dToolHeight ) == S_OK ) && l_bOk;

		if( (l_dInputWidth != l_dToolWidth) || (l_dInputHeight != l_dToolHeight) )
		{
			ResetObject();
		}

		if ( l_pInputImage != NULL &&
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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVPerspectiveToolObjectType;

	// Identify our input image...
	m_InputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "PerspectiveToolImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_svXOffset, SVTranslationXOffsetObjectGuid, IDS_X_OFFSET, false, SVResetItemTool ); 
	RegisterEmbeddedObject( &m_svYOffset, SVTranslationYOffsetObjectGuid, IDS_Y_OFFSET, false, SVResetItemTool ); 

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

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

	if ( DestroyLUT() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVPerspectiveToolClass::CreateLUT()
{
	HRESULT l_hrOk = S_FALSE;

	if ( DestroyLUT() == S_OK )
	{
		//SVImageExtentClass l_svInputExtents;
		SVImageExtentClass l_svOutputExtents;

		SVMatroxImageInterface::SVStatusCode l_Code;

		long l_lWidth = 100;
		long l_lHeight = 100;
		if ( GetInputImage() != NULL)
		{
			HRESULT l_hr = GetImageExtent(l_svOutputExtents) ;


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

			long *l_plLutXData = NULL; //(long *)MbufInquire( m_LutX, M_HOST_ADDRESS, M_NULL );
			long *l_plLutYData = NULL; // (long *)MbufInquire( m_LutY, M_HOST_ADDRESS, M_NULL );
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
		if( p_svImageExtent.GetExtentProperty( SVExtentPropertyOutputHeight, l_dValue) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
		if( l_hrOk == S_OK && l_dOffset >= l_dValue )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetY, l_dValue - 1 );
		}
		if( l_hrOk == S_OK &&  l_dOffset < 0 )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 0 );
		}
		// X is always default with a Horizontal
		p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 0);
	}
	if( l_eTrans == SVExtentTranslationHorizontalPerspective )
	{
		l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dOffset );
		if( p_svImageExtent.GetExtentProperty( SVExtentPropertyOutputWidth, l_dValue) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
		if( l_hrOk == S_OK && l_dOffset >= l_dValue )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetX, l_dValue - 1 );
		}
		if( l_hrOk == S_OK &&  l_dOffset < 0 )
		{
			l_hrOk = p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetX, 0 );
		}
		// Y is always default with a Vertical
		p_svImageExtent.SetExtentProperty( SVExtentPropertyTranslationOffsetY, 0);
	}

	l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );

	return l_hrOk;
}

// Set String value object for Source Image Names
HRESULT SVPerspectiveToolClass::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = GetInputImage();
	if( l_pInputImage )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		m_svSourceImageNames.SetValue( RRunStatus.m_lResultDataIndex, 0, l_strName );

		l_hr = S_OK;
	}
	return l_hr;
}

HRESULT SVPerspectiveToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageNames;
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPerspectiveTool.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:21:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Jul 2012 14:19:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   12 Jul 2012 15:03:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Jun 2012 14:00:56   jspila
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
 *    Rev 1.19   30 Mar 2011 16:24:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 Mar 2011 12:13:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Mar 2011 13:48:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   11 Mar 2011 14:31:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   01 Mar 2011 10:39:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   25 Feb 2011 12:22:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Feb 2011 15:09:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Jan 2011 11:50:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Nov 2010 14:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   01 Jun 2010 15:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 12:25:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2009 12:23:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Apr 2008 08:42:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Set Parent operator with Set Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Mar 2008 12:57:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functions to support source image names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Jul 2007 15:44:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4.3.0   14 Jul 2009 15:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code touse the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4.2.0   07 Apr 2008 08:40:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Nov 2005 07:33:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jul 2005 13:18:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jul 2005 15:47:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Interpolation mode to PerspectiveWarpTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 08:27:54   ebeyeler
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
 *    Rev 1.0   09 Mar 2005 13:46:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in new files for the new perspective tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
