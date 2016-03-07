//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpToolClass
//* .File Name       : $Workfile:   SVCylindricalWarpTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:05:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVCylindricalWarpTool.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageExtentClass.h"

#include "SVImageProcessingClass.h"
#include "SVValueObject.h"

SV_IMPLEMENT_CLASS( SVCylindricalWarpToolClass, SVCylindricalWarpToolClassGuid )

SVCylindricalWarpToolClass::SVCylindricalWarpToolClass( BOOL p_bCreateDefaultTaskList, SVObjectClass* p_pOwner, int p_iStringResourceID )
	: SVToolClass(p_bCreateDefaultTaskList, p_pOwner, p_iStringResourceID)
{
	LocalInitialize();
}

SVCylindricalWarpToolClass::~SVCylindricalWarpToolClass()
{
	LocalDestroy();
}

BOOL SVCylindricalWarpToolClass::CreateObject( SVObjectLevelCreateStruct *p_pCreateStructure )
{
	BOOL l_bOk = SVToolClass::CreateObject( p_pCreateStructure );

	if ( l_bOk )
	{
		l_bOk = LocalCreate() == S_OK;
	}

	m_svSourceImageName.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = l_bOk;

	return l_bOk;
}

BOOL SVCylindricalWarpToolClass::CloseObject()
{
	BOOL l_bOk = LocalDestroy() == S_OK;

	l_bOk = SVToolClass::CloseObject() && l_bOk;

	return l_bOk;
}

void SVCylindricalWarpToolClass::LocalInitialize()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolCylindricalObjectType;

	// Identify our input image...
	m_InputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "CylindricalWarpImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageName, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	m_OutputImage.InitializeImage( SVImageTypePhysical );

	// Set Default Warp Method to Use Horizontal
	RegisterEmbeddedObject( &m_svWarpType, SVWarpTypeObjectGuid, IDS_OBJECTNAME_WARPTYPE, false, SVResetItemTool );
	CString strEnumTypes;
	strEnumTypes.Format("%s=%d,%s=%d", CYLINDRICAL_WARP_TYPE_HORIZONTAL, WarpTypeHorizontal,
	                                   CYLINDRICAL_WARP_TYPE_VERTICAL, WarpTypeVertical);
	m_svWarpType.SetEnumTypes( strEnumTypes );
	m_svWarpType.SetDefaultValue( CYLINDRICAL_WARP_TYPE_HORIZONTAL, TRUE );
	m_svWarpType.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Set Default Interpolation Mode to use Nearest Neighbor
	CString strMode;
	CString strPrepare;
	strEnumTypes.Empty();

	// M_NEAREST_NEIGHBOR 
	strMode.LoadString( IDS_NEAREST_NEIGHBOR_STRING );
	strPrepare.Format( _T( "%s=%d," ), strMode, SVNearestNeighbor); // M_NEAREST_NEIGHBOR);
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
	m_svInterpolationMode.SetEnumTypes( strEnumTypes );
	m_svInterpolationMode.SetDefaultValue( SVNearestNeighbor, TRUE );	// Refer to MIL...
	RegisterEmbeddedObject( &m_svInterpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SVResetItemNone );



	// Set Default Warp Angle to the maximum 180 degrees.
	// Using start angle of 90. In the future we will use end angle to 
	// allow an unequal warp tool.
	// Currently we double the start angle and make it symetrical
	// the work is done in SVImageExtentClass.
	RegisterEmbeddedObject( &m_svWarpAngle, SVWarpAngleObjectGuid, IDS_OBJECTNAME_WARPANGLE, false, SVResetItemTool );
	m_svWarpAngle.SetDefaultValue( 180.0, TRUE );
	m_svWarpAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_svToolExtent.SetExtentObject( SVExtentPropertyStartAngle, &m_svWarpAngle );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

HRESULT SVCylindricalWarpToolClass::LocalCreate()
{
	HRESULT l_hrOk = S_FALSE;
	HRESULT l_hr= S_OK;

	l_hrOk = S_OK;
	if ( LocalDestroy() == S_OK )
	{
		SVGUID l_InputID;
		SVImageInfoClass l_ImageInfo;
		SVImageExtentClass l_svExtents;

		SVImageClass* l_pInputImage = GetInputImage();

		if( l_pInputImage != NULL )
		{
			// Image input must already exist, and must be created!!!
			// Embedded Image output must already exist!!!

			// Get reference to new blank image info...
			l_ImageInfo = l_pInputImage->GetImageInfo();

			// Size of Output is same as the Input when rotation is not enabled
			// Copy source image info into blank image info...
			l_InputID = l_pInputImage->GetUniqueObjectID();

			// Input Extents..
			l_svExtents = l_pInputImage->GetImageExtents(); //l_ImageInfo.GetExtent();
		}
		else
		{
			l_ImageInfo = m_OutputImage.GetImageInfo();

			l_InputID = m_OutputImage.GetUniqueObjectID();

			l_svExtents = m_OutputImage.GetImageExtents();
		}

		long l_lDiameter = 0;

		// Warp Type..
		long lWarpType = SVExtentTranslationCylindricalWarpH;	// default
		m_svWarpType.GetValue( lWarpType );
		SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ?
		                                       SVExtentTranslationCylindricalWarpV :
		                                       SVExtentTranslationCylindricalWarpH;
		l_svExtents.SetTranslation( eTranslation );

		// Warp Angle
		double l_dAngle = 180;
		l_hrOk = m_svWarpAngle.GetValue( l_dAngle );
		if( (l_hr = l_svExtents.SetExtentProperty( SVExtentPropertyStartAngle, l_dAngle )) != S_OK )
			l_hrOk = l_hr;

		// Build Output Extents...
		if( (l_hr = l_svExtents.UpdateData()) != S_OK )
			l_hrOk = l_hr;

		// Tool Extents.....
		if( (l_hr = SetImageExtent( 1, l_svExtents )) != S_OK )
			l_hrOk = l_hr;

		// Output Image..
		l_ImageInfo.SetOwner( GetUniqueObjectID() );
		l_ImageInfo.SetExtents( l_svExtents );

		m_OutputImage.SetObjectOwner( this );

		l_hrOk = m_OutputImage.UpdateImage( SVImageTypePhysical, l_InputID, l_ImageInfo );

		if( l_hrOk != S_OK )
		{
			l_hrOk = CreateLUT();
		}

		// Return code for UpdateOffsetData not being checked because it may not be valid the first time.
		UpdateOffsetData( &m_OutputImage );
	}

	return l_hrOk;
}

HRESULT SVCylindricalWarpToolClass::LocalDestroy()
{
	HRESULT l_hrOk = S_OK;

	if ( DestroyLUT() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVCylindricalWarpToolClass::UpdateOutputImageExtents()
{
	HRESULT l_hrOk = S_OK;

	// Get Input Width and Height put in output Image Extent.
	SVImageExtentClass l_svInputExtents = GetInputImage()->GetImageExtents();
	SVImageExtentClass l_svOutputExtents = m_OutputImage.GetImageExtents();
	
	SVImageExtentClass l_svToolExtents;
	l_hrOk = GetImageExtent( l_svToolExtents );

	// Set Translation
	long lWarpType = SVExtentTranslationCylindricalWarpH;	// default
	l_hrOk = m_svWarpType.GetValue( lWarpType );
	SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ?
				                           SVExtentTranslationCylindricalWarpV :
				                           SVExtentTranslationCylindricalWarpH;
	l_hrOk = l_svOutputExtents.SetTranslation( eTranslation );

	// Set Warp Angle
	double l_dStartAngle;
	m_svWarpAngle.GetValue( l_dStartAngle );
	ValidateAngle( l_dStartAngle );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle );

	double l_dValue;
	l_hrOk = l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyWidth, l_dValue);

	l_hrOk = l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );
	l_hrOk = l_svOutputExtents.SetExtentProperty( SVExtentPropertyHeight, l_dValue );

	l_hrOk = l_svOutputExtents.UpdateData();

	l_hrOk = SetImageExtent( 1, l_svOutputExtents );

	SVGUID l_InputID;

	if( GetInputImage() != NULL )
	{
		l_InputID = GetInputImage()->GetUniqueObjectID();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( l_svOutputExtents );

	l_hrOk = m_OutputImage.UpdateImage( l_InputID, l_ImageInfo );

	return l_hrOk;
}

HRESULT SVCylindricalWarpToolClass::ResetObject()
{
	HRESULT l_hrOk = SVToolClass::ResetObject();

	l_hrOk = UpdateOutputImageExtents();

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

	SVImageClass *inputImage = GetInputImage();
	if (nullptr != inputImage)
	{
		//Set input name to source image name to display it in result picker
		m_svSourceImageName.SetValue( 0/*Static value, this parameter will not used*/, inputImage->GetCompleteObjectName() );
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

BOOL SVCylindricalWarpToolClass::OnValidate()
{
	BOOL l_bOk = SVToolClass::OnValidate();

	if ( l_bOk )
	{
		l_bOk = !m_LutX.empty() && !m_LutY.empty();
	}

	return l_bOk;
}

SVImageClass* SVCylindricalWarpToolClass::GetInputImage()
{
	SVImageClass* l_pImage = NULL;

	if( m_InputImageObjectInfo.IsConnected() && 
		  m_InputImageObjectInfo.GetInputObjectInfo().PObject != NULL )
	{
		l_pImage = dynamic_cast<SVImageClass*> (m_InputImageObjectInfo.GetInputObjectInfo().PObject);
	}

	return l_pImage;
}

bool SVCylindricalWarpToolClass::DoesObjectHaveExtents() const
{
	return false;
}

BOOL SVCylindricalWarpToolClass::onRun( SVRunStatusClass& p_rRunStatus )
{
	BOOL l_bOk = TRUE;

	if ( SVToolClass::onRun( p_rRunStatus ) )
	{
		SVImageClass* l_pInputImage = GetInputImage();

		SVImageExtentClass l_svToolExtents;
		l_bOk = GetImageExtent(l_svToolExtents) == S_OK;
		
		SVMatroxImageInterface::SVStatusCode l_Code;

		SVImageExtentClass l_svInputExtents = l_pInputImage->GetImageExtents();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;
		long l_lInterpolation;
		m_svInterpolationMode.GetValue(p_rRunStatus.m_lResultDataIndex, l_lInterpolation);

		l_bOk = (l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dInputWidth ) == S_OK) && l_bOk;
		l_bOk = (l_svToolExtents.GetExtentProperty( SVExtentPropertyWidth, l_dToolWidth ) == S_OK) && l_bOk;
		l_bOk = (l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dInputHeight ) == S_OK) && l_bOk;
		l_bOk = (l_svToolExtents.GetExtentProperty( SVExtentPropertyHeight, l_dToolHeight ) == S_OK) && l_bOk;

		if( (l_dInputWidth != l_dToolWidth) || (l_dInputHeight != l_dToolHeight) )
		{
			HRESULT hrReset = ResetObject();
		}

		if ( l_pInputImage != NULL &&
			   m_OutputImage.SetImageHandleIndex( p_rRunStatus.Images ) )
		{
			SVSmartHandlePointer l_InputHandle;
			SVSmartHandlePointer l_OutputHandle;

			SVImageBufferHandleImage l_InMilHandle;
			SVImageBufferHandleImage l_OutMilHandle;

			if ( l_pInputImage->GetImageHandle( l_InputHandle ) && !( l_InputHandle.empty() ) &&
				   m_OutputImage.GetImageHandle( l_OutputHandle ) && !( l_OutputHandle.empty() ) )
			{
				l_InputHandle->GetData( l_InMilHandle );
				l_OutputHandle->GetData( l_OutMilHandle );
			}

			if( !( l_InMilHandle.empty() ) && !( l_OutMilHandle.empty() ) )
			{
				l_Code = SVMatroxImageInterface::Warp(l_OutMilHandle.GetBuffer(), 
					l_InMilHandle.GetBuffer(), 
					m_LutX, 
					m_LutY,
					static_cast<SVImageOperationTypeEnum>(l_lInterpolation));
					

				if ( l_Code != SVMEE_STATUS_OK )
				{
					
					SVMatroxStatusInformation l_info;
					CString l_sErrorStr;
					l_bOk = FALSE;
				}
			}
		}
	}

	ASSERT( l_bOk );

	if ( ! l_bOk )
	{
		p_rRunStatus.SetInvalid();
	}

	return l_bOk;
}

HRESULT SVCylindricalWarpToolClass::CreateLUT()
{
	HRESULT l_hrOk = S_OK;

	SVImageExtentClass l_OutputExtents;

	
	SVMatroxImageInterface::SVStatusCode l_Code;
	

	long l_lOutputWidth = 100;
	long l_lOutputHeight = 100;
	l_OutputExtents = m_OutputImage.GetImageExtents();
	l_OutputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_lOutputWidth );
	l_OutputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_lOutputHeight );



	SVMatroxBufferCreateStruct l_Create;
	l_Create.m_eAttribute = SVBufAttLut;
	l_Create.m_eType = SV32BitSigned;
	l_Create.m_lSizeX = l_lOutputWidth;
	l_Create.m_lSizeY = l_lOutputHeight;
	l_Code = SVMatroxBufferInterface::Create( m_LutX, l_Create );
	l_Code = SVMatroxBufferInterface::Create( m_LutY, l_Create );
	ASSERT( !m_LutX.empty() );
	ASSERT( !m_LutY.empty() );

	if ( !m_LutX.empty() && !m_LutY.empty() )
	{
		long lWarpType = WarpTypeHorizontal;
		m_svWarpType.GetValue( lWarpType );
		SVCylindricalWarpToolClass::WarpType eWarpType = (SVCylindricalWarpToolClass::WarpType) lWarpType;

		SVExtentPointStruct l_svPointValue;
		SVExtentPointStruct l_svPointResult;

		long l_lPitchX;
		long l_lPitchY;
		SVMatroxBufferInterface::Get( m_LutX, SVPitch, l_lPitchX );
		SVMatroxBufferInterface::Get( m_LutY, SVPitch, l_lPitchY );

		long* l_plLutXData = NULL; // (long*) MbufInquire( m_LutX, M_HOST_ADDRESS, M_NULL );
		long* l_plLutYData = NULL; // (long*) MbufInquire( m_LutY, M_HOST_ADDRESS, M_NULL );
		SVMatroxBufferInterface::GetHostAddress( static_cast<void*>(&l_plLutXData), m_LutX );
		SVMatroxBufferInterface::GetHostAddress( static_cast<void*>(&l_plLutYData), m_LutY );

		for ( long y = 0; y < l_lOutputHeight; y++ )
		{
			for ( long x = 0; x < l_lOutputWidth; x++ )
			{
				l_svPointValue.m_dPositionX = x;
				l_svPointValue.m_dPositionY = y;
				l_OutputExtents.TranslateFromOutputSpace( l_svPointValue, l_svPointResult );
				l_plLutXData[ x + ( y * l_lPitchY ) ] = (long)( l_svPointResult.m_dPositionX * 256L );
				l_plLutYData[ x + ( y * l_lPitchX ) ] = (long)( l_svPointResult.m_dPositionY * 256L );
			}
		}
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVCylindricalWarpToolClass::DestroyLUT()
{
	HRESULT l_hrOk = S_OK;

	m_LutX.clear();
	m_LutY.clear();

	return l_hrOk;
}


HRESULT SVCylindricalWarpToolClass::ValidateAngle(double &p_dWarpAngle )
{
	if( p_dWarpAngle < 0 )
	{
		p_dWarpAngle = 0;
	}
	if( p_dWarpAngle > 180 )
	{
		p_dWarpAngle = 180;
	}
	return S_OK;
}

SVStaticStringValueObjectClass* SVCylindricalWarpToolClass::GetInputImageNames()
{
	return &m_svSourceImageName;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCylindricalWarpTool.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:05:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   12 Jul 2012 14:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   19 Jun 2012 12:20:46   jspila
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
 *    Rev 1.25   21 Sep 2011 08:27:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified LocalCreate to ignore return code from UpdateOffsetData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   30 Mar 2011 16:20:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   30 Mar 2011 08:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues with Adjust Tool position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   21 Mar 2011 12:05:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   16 Mar 2011 13:23:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   11 Mar 2011 14:17:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   01 Mar 2011 10:13:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Feb 2011 12:07:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   10 Feb 2011 14:33:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   27 Jan 2011 10:58:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Nov 2010 13:32:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   01 Jun 2010 14:12:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   15 Dec 2009 15:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Jul 2009 11:17:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8.3.0   14 Jul 2009 12:44:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Apr 2008 07:51:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   07 Mar 2008 11:28:22   tbair
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
 *    Rev 1.9   24 Jul 2007 11:47:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8.2.0   07 Apr 2008 07:56:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jan 2006 09:55:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  544
 * SCR Title:  Fix output image size for the Cylindrical Warp Tool
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to force an extent update to update the output extent data before setting the extent data into the tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   20 Jan 2006 11:24:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateOffsetData to LocalCreate
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Nov 2005 07:33:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jul 2005 12:17:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jul 2005 08:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Cylindrical Dialog Changes
 * Added Warp Angle and Interpolation Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2005 08:04:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2005 15:30:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed rounding error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 15:12:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed orientation error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Mar 2005 12:17:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
