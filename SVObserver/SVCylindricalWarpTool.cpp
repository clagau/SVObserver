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
#pragma region Includes
#include "stdafx.h"
#include "SVCylindricalWarpTool.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

static const TCHAR* const CYLINDRICAL_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
static const TCHAR* const CYLINDRICAL_WARP_TYPE_VERTICAL = _T("Vertical Warp");

SV_IMPLEMENT_CLASS( SVCylindricalWarpToolClass, SVCylindricalWarpToolClassGuid )

SVCylindricalWarpToolClass::SVCylindricalWarpToolClass( SVObjectClass* p_pOwner, int p_iStringResourceID )
	: SVToolClass(p_pOwner, p_iStringResourceID)
{
	LocalInitialize();
}

SVCylindricalWarpToolClass::~SVCylindricalWarpToolClass()
{
	LocalDestroy();
}

bool SVCylindricalWarpToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = SVToolClass::CreateObject(rCreateStructure);

	if ( l_bOk )
	{
		l_bOk = S_OK == LocalCreate();
	}

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = l_bOk;

	return l_bOk;
}

bool SVCylindricalWarpToolClass::CloseObject()
{
	bool l_bOk = S_OK == LocalDestroy();

	l_bOk = SVToolClass::CloseObject() && l_bOk;

	return l_bOk;
}

void SVCylindricalWarpToolClass::LocalInitialize()
{
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvDef::SVToolCylindricalObjectType;

	// Identify our input image...
	m_InputImageObjectInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "CylindricalWarpImage" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	m_OutputImage.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	// Set Default Warp Method to Use Horizontal
	RegisterEmbeddedObject( &m_svWarpType, SVWarpTypeObjectGuid, IDS_OBJECTNAME_WARPTYPE, false, SvOi::SVResetItemTool );
	std::string EnumTypes = SvUl::Format( _T("%s=%d,%s=%d"), CYLINDRICAL_WARP_TYPE_HORIZONTAL, WarpTypeHorizontal,
	                                   CYLINDRICAL_WARP_TYPE_VERTICAL, WarpTypeVertical);
	m_svWarpType.SetEnumTypes( EnumTypes.c_str() );
	m_svWarpType.SetDefaultValue( CYLINDRICAL_WARP_TYPE_HORIZONTAL, true);
	m_svWarpType.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	// Set Default Interpolation Mode to use Nearest Neighbor
	std::string Mode;
	std::string Text;
	EnumTypes.clear();

	// M_NEAREST_NEIGHBOR 
	Mode = SvUl::LoadStdString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = SvUl::Format( _T( "%s=%d," ), Mode.c_str(), SVNearestNeighbor); // M_NEAREST_NEIGHBOR);
	EnumTypes += Text;
	// M_BILINEAR
	Mode = SvUl::LoadStdString( IDS_BILINEAR_STRING );
	Text = SvUl::Format( _T( "%s=%d," ), Mode.c_str(), SVBilinear);		// M_BILINEAR );
	EnumTypes += Text;
	// M_BICUBIC
	Mode = SvUl::LoadStdString( IDS_BICUBIC_STRING );
	Text = SvUl::Format( _T( "%s=%d," ), Mode.c_str(), SVBiCubic);			// M_BICUBIC );
	EnumTypes += Text;

	// And now set enum types...
	m_svInterpolationMode.SetEnumTypes( EnumTypes.c_str() );
	m_svInterpolationMode.SetDefaultValue( SVNearestNeighbor, true);	// Refer to MIL...
	RegisterEmbeddedObject( &m_svInterpolationMode, SVOutputInterpolationModeObjectGuid, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );



	// Set Default Warp Angle to the maximum 180 degrees.
	// Using start angle of 90. In the future we will use end angle to 
	// allow an unequal warp tool.
	// Currently we double the start angle and make it symetrical
	// the work is done in SVImageExtentClass.
	RegisterEmbeddedObject( &m_svWarpAngle, SVWarpAngleObjectGuid, IDS_OBJECTNAME_WARPANGLE, false, SvOi::SVResetItemTool );
	m_svWarpAngle.SetDefaultValue( 180.0, true);
	m_svWarpAngle.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_svToolExtent.SetExtentObject( SVExtentPropertyStartAngle, &m_svWarpAngle );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

HRESULT SVCylindricalWarpToolClass::LocalCreate()
{
	HRESULT l_hrOk = S_FALSE;
	HRESULT l_hr= S_OK;

	l_hrOk = S_OK;
	if ( S_OK == LocalDestroy() )
	{
		SVGUID l_InputID;
		SVImageInfoClass l_ImageInfo;
		SVImageExtentClass Extents;

		SVImageClass* l_pInputImage = GetInputImage();

		if( nullptr != l_pInputImage )
		{
			// Image input must already exist, and must be created!!!
			// Embedded Image output must already exist!!!

			// Get reference to new blank image info...
			l_ImageInfo = l_pInputImage->GetImageInfo();

			// Size of Output is same as the Input when rotation is not enabled
			// Copy source image info into blank image info...
			l_InputID = l_pInputImage->GetUniqueObjectID();

			// Input Extents..
			Extents = l_pInputImage->GetImageExtents(); //l_ImageInfo.GetExtent();
		}
		else
		{
			l_ImageInfo = m_OutputImage.GetImageInfo();

			l_InputID = m_OutputImage.GetUniqueObjectID();

			Extents = m_OutputImage.GetImageExtents();
		}

		long l_lDiameter = 0;

		// Warp Type..
		long lWarpType = SVExtentTranslationCylindricalWarpH;	// default
		m_svWarpType.GetValue( lWarpType );
		SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ?
		                                       SVExtentTranslationCylindricalWarpV :
		                                       SVExtentTranslationCylindricalWarpH;
		Extents.SetTranslation( eTranslation );

		// Warp Angle
		double l_dAngle = 180;
		l_hrOk = m_svWarpAngle.GetValue( l_dAngle );
		if( S_OK != (l_hr = Extents.SetExtentProperty( SVExtentPropertyStartAngle, l_dAngle )) )
			l_hrOk = l_hr;

		// Build Output Extents...
		if( S_OK != (l_hr = Extents.UpdateData()) )
			l_hrOk = l_hr;

		// Tool Extents.....
		if( S_OK != (l_hr = SetImageExtent( Extents )) )
			l_hrOk = l_hr;

		// Output Image..
		l_ImageInfo.SetOwner( GetUniqueObjectID() );
		l_ImageInfo.SetExtents( Extents );

		m_OutputImage.SetObjectOwner( this );

		l_hrOk = m_OutputImage.UpdateImage( l_InputID, l_ImageInfo );

		if( S_OK != l_hrOk )
		{
			CreateLUT();
		}

		// Return code for UpdateOffsetData not being checked because it may not be valid the first time.
		UpdateOffsetData( &m_OutputImage );
	}

	return l_hrOk;
}

HRESULT SVCylindricalWarpToolClass::LocalDestroy()
{
	HRESULT l_hrOk = S_OK;

	if ( S_OK != DestroyLUT() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVCylindricalWarpToolClass::UpdateOutputImageExtents()
{
	HRESULT l_hrOk = S_OK;

	// Get Input Width and Height put in output Image Extent.
	SVImageExtentClass InputExtents = GetInputImage()->GetImageExtents();
	SVImageExtentClass OutputExtents = m_OutputImage.GetImageExtents();
	
	SVImageExtentClass l_svToolExtents;
	l_hrOk = GetImageExtent( l_svToolExtents );

	// Set Translation
	long lWarpType = SVExtentTranslationCylindricalWarpH;	// default
	l_hrOk = m_svWarpType.GetValue( lWarpType );
	SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ?
				                           SVExtentTranslationCylindricalWarpV :
				                           SVExtentTranslationCylindricalWarpH;
	l_hrOk = OutputExtents.SetTranslation( eTranslation );

	// Set Warp Angle
	double l_dStartAngle;
	m_svWarpAngle.GetValue( l_dStartAngle );
	ValidateAngle( l_dStartAngle );
	l_hrOk = OutputExtents.SetExtentProperty( SVExtentPropertyStartAngle, l_dStartAngle );

	double l_dValue;
	l_hrOk = InputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dValue );
	l_hrOk = OutputExtents.SetExtentProperty( SVExtentPropertyWidth, l_dValue);

	l_hrOk = InputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dValue );
	l_hrOk = OutputExtents.SetExtentProperty( SVExtentPropertyHeight, l_dValue );

	l_hrOk = OutputExtents.UpdateData();

	l_hrOk = SetImageExtent( OutputExtents );

	SVGUID l_InputID;

	if( nullptr != GetInputImage() )
	{
		l_InputID = GetInputImage()->GetUniqueObjectID();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( OutputExtents );

	l_hrOk = m_OutputImage.UpdateImage( l_InputID, l_ImageInfo );

	return l_hrOk;
}

bool SVCylindricalWarpToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	HRESULT l_hrOk = UpdateOutputImageExtents();
	if (S_OK != l_hrOk)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateOutputImageExtentsFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	// Now the input image is valid!
	Result = m_OutputImage.ResetObject(pErrorMessages) && Result;

	if (Result)
	{
		if (!CreateLUT())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	SVImageClass *inputImage = GetInputImage();
	if (nullptr != inputImage)
	{
		//Set input name to source image name to display it in result picker
		m_SourceImageNames.SetValue( inputImage->GetCompleteName() );
	}

	UpdateImageWithExtent();

	return Result;
}

SVImageClass* SVCylindricalWarpToolClass::GetInputImage()
{
	if( m_InputImageObjectInfo.IsConnected() && nullptr != m_InputImageObjectInfo.GetInputObjectInfo().getObject() )
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVImageClass*> (m_InputImageObjectInfo.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

bool SVCylindricalWarpToolClass::DoesObjectHaveExtents() const
{
	return false;
}

bool SVCylindricalWarpToolClass::onRun( SVRunStatusClass& p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool l_bOk = __super::onRun( p_rRunStatus, pErrorMessages );

	if (l_bOk)
	{
		SVImageClass* l_pInputImage = GetInputImage();

		SVImageExtentClass l_svToolExtents;
		l_bOk = S_OK == GetImageExtent(l_svToolExtents);
		
		HRESULT MatroxCode;

		SVImageExtentClass l_svInputExtents = l_pInputImage->GetImageExtents();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;
		long Interpolation;
		m_svInterpolationMode.GetValue(Interpolation);

		l_bOk = (S_OK == l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dInputWidth )) && l_bOk;
		l_bOk = (S_OK == l_svToolExtents.GetExtentProperty( SVExtentPropertyWidth, l_dToolWidth )) && l_bOk;
		l_bOk = (S_OK == l_svInputExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dInputHeight )) && l_bOk;
		l_bOk = (S_OK == l_svToolExtents.GetExtentProperty( SVExtentPropertyHeight, l_dToolHeight )) && l_bOk;

		if (!l_bOk)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		if( (l_dInputWidth != l_dToolWidth) || (l_dInputHeight != l_dToolHeight) )
		{
			l_bOk = ResetObject(pErrorMessages) && l_bOk;
		}

		if ( nullptr != l_pInputImage &&
			   m_OutputImage.SetImageHandleIndex( p_rRunStatus.Images ) )
		{
			SvOi::SVImageBufferHandlePtr l_InputHandle;
			SvOi::SVImageBufferHandlePtr l_OutputHandle;

			SVImageBufferHandleImage l_InMilHandle;
			SVImageBufferHandleImage l_OutMilHandle;

			if ( l_pInputImage->GetImageHandle( l_InputHandle ) && nullptr != l_InputHandle &&
				m_OutputImage.GetImageHandle( l_OutputHandle ) && nullptr != l_OutputHandle &&
				!(l_InputHandle->empty() ) && !(l_OutputHandle->empty() ) )
			{
				MatroxCode = SVMatroxImageInterface::Warp(l_OutputHandle->GetBuffer(),
					l_InputHandle->GetBuffer(),
					m_LutX, 
					m_LutY,
					static_cast<SVImageOperationTypeEnum>(Interpolation));
					

				if (S_OK != MatroxCode)
				{
					l_bOk = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunWarpFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
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

bool SVCylindricalWarpToolClass::CreateLUT()
{
	SVImageExtentClass l_OutputExtents;
	HRESULT l_Code;
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

		long* l_plLutXData = nullptr;
		long* l_plLutYData = nullptr;
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
		return true;
	}
	else //if ( !m_LutX.empty() && !m_LutY.empty() )
	{
		return false;
	}
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

SVStringValueObjectClass* SVCylindricalWarpToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

