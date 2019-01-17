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

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVToolCylindricalObjectType;

	// Identify our input image...
	m_InputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
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
	m_svWarpType.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

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
	m_svWarpAngle.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_toolExtent.SetExtentObject( SvDef::SVExtentPropertyStartAngle, &m_svWarpAngle );

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
		SVImageExtentClass imageExtents;

		SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_InputImageObjectInfo);

		if( nullptr != pInputImage )
		{
			// Image input must already exist, and must be created!!!
			// Embedded Image output must already exist!!!

			// Get reference to new blank image info...
			l_ImageInfo = pInputImage->GetImageInfo();

			// Size of Output is same as the Input when rotation is not enabled
			// Copy source image info into blank image info...
			l_InputID = pInputImage->GetUniqueObjectID();

			// Input Extents..
			imageExtents = pInputImage->GetImageExtents();
		}
		else
		{
			l_ImageInfo = m_OutputImage.GetImageInfo();

			l_InputID = m_OutputImage.GetUniqueObjectID();

			imageExtents = m_OutputImage.GetImageExtents();
		}

		long l_lDiameter = 0;

		// Warp Type..
		long lWarpType = SvDef::SVExtentTranslationCylindricalWarpH;	// default
		m_svWarpType.GetValue( lWarpType );
		SvDef::SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ?
		                                       SvDef::SVExtentTranslationCylindricalWarpV :
		                                       SvDef::SVExtentTranslationCylindricalWarpH;
		imageExtents.SetTranslation( eTranslation );

		// Warp Angle
		double l_dAngle = 180;
		l_hrOk = m_svWarpAngle.GetValue( l_dAngle );
		if( S_OK != (l_hr = imageExtents.SetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dAngle )) )
		{
			l_hrOk = l_hr;
		}

		// Build Output Extents...
		if( S_OK != (l_hr = imageExtents.UpdateData()) )
		{
			l_hrOk = l_hr;
		}

		// Tool Extents.....
		if( S_OK != (l_hr = SetImageExtent(imageExtents)) )
		{
			l_hrOk = l_hr;
		}

		// Output Image..
		l_ImageInfo.SetOwner( GetUniqueObjectID() );
		l_ImageInfo.SetExtents(imageExtents);

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

	SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_InputImageObjectInfo);
	// Get Input Width and Height put in output Image Extent.
	const SVImageExtentClass rInputExtents = (nullptr != pInputImage) ? pInputImage->GetImageExtents() : SVImageExtentClass();
	SVImageExtentClass OutputExtents = m_OutputImage.GetImageExtents();
	
	// Set Translation
	long lWarpType = SvDef::SVExtentTranslationCylindricalWarpH;	// default
	l_hrOk = m_svWarpType.GetValue( lWarpType );
	SvDef::SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ?
				                           SvDef::SVExtentTranslationCylindricalWarpV :
				                           SvDef::SVExtentTranslationCylindricalWarpH;
	OutputExtents.SetTranslation( eTranslation );

	// Set Warp Angle
	double l_dStartAngle;
	m_svWarpAngle.GetValue( l_dStartAngle );
	ValidateAngle( l_dStartAngle );
	l_hrOk = OutputExtents.SetExtentProperty( SvDef::SVExtentPropertyStartAngle, l_dStartAngle );

	double l_dValue;
	l_hrOk = rInputExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dValue );
	l_hrOk = OutputExtents.SetExtentProperty( SvDef::SVExtentPropertyWidth, l_dValue);

	l_hrOk = rInputExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dValue );
	l_hrOk = OutputExtents.SetExtentProperty( SvDef::SVExtentPropertyHeight, l_dValue );

	l_hrOk = OutputExtents.UpdateData();

	l_hrOk = SetImageExtent( OutputExtents );

	SVGUID l_InputID;

	if(nullptr != pInputImage)
	{
		l_InputID = pInputImage->GetUniqueObjectID();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( OutputExtents );

	l_hrOk = m_OutputImage.UpdateImage( l_InputID, l_ImageInfo );

	return l_hrOk;
}

bool SVCylindricalWarpToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_InputImageObjectInfo);

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

	SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_InputImageObjectInfo);
	if (nullptr != pInputImage)
	{
		//Set input name to source image name to display it in result picker
		m_SourceImageNames.SetValue( pInputImage->GetCompleteName() );
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	UpdateImageWithExtent();

	return Result;
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
		SVImageClass* pInputImage = SvOl::getInput<SVImageClass>(m_InputImageObjectInfo, true);

		const SVImageExtentClass& rToolExtents = GetImageExtent();
		
		SVImageExtentClass InputExtents = (nullptr != pInputImage) ? pInputImage->GetImageExtents() : SVImageExtentClass();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;
		long Interpolation;
		m_svInterpolationMode.GetValue(Interpolation);

		l_bOk = (S_OK == InputExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, l_dInputWidth )) && l_bOk;
		l_bOk = (S_OK == rToolExtents.GetExtentProperty( SvDef::SVExtentPropertyWidth, l_dToolWidth )) && l_bOk;
		l_bOk = (S_OK == InputExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, l_dInputHeight )) && l_bOk;
		l_bOk = (S_OK == rToolExtents.GetExtentProperty( SvDef::SVExtentPropertyHeight, l_dToolHeight )) && l_bOk;

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

		if ( nullptr != pInputImage )
		{
			SvTrc::IImagePtr pInputImageBuffer = pInputImage->getImageReadOnly(p_rRunStatus.m_triggerRecord);
			SvTrc::IImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(p_rRunStatus.m_triggerRecord);

			if ( nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty() &&
				nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
			{
				HRESULT MatroxCode = SVMatroxImageInterface::Warp(pOutputImageBuffer->getHandle()->GetBuffer(),
					pInputImageBuffer->getHandle()->GetBuffer(),
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

	assert( l_bOk );

	if ( ! l_bOk )
	{
		p_rRunStatus.SetInvalid();
	}

	return l_bOk;
}

bool SVCylindricalWarpToolClass::CreateLUT()
{
	long lOutputWidth = 100;
	long lOutputHeight = 100;
	const SVImageExtentClass rOutputExtents = m_OutputImage.GetImageExtents();
	rOutputExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, lOutputWidth );
	rOutputExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, lOutputHeight );

	SVMatroxBufferCreateStruct l_Create;
	l_Create.m_eAttribute = SVBufAttLut;
	l_Create.m_eType = SV32BitSigned;
	l_Create.m_lSizeX = lOutputWidth;
	l_Create.m_lSizeY = lOutputHeight;
	SVMatroxBufferInterface::Create( m_LutX, l_Create );
	SVMatroxBufferInterface::Create( m_LutY, l_Create );
	ASSERT( !m_LutX.empty() );
	ASSERT( !m_LutY.empty() );

	if ( !m_LutX.empty() && !m_LutY.empty() )
	{
		long pitchX;
		long pitchY;
		SVMatroxBufferInterface::Get( m_LutX, SVPitch, pitchX );
		SVMatroxBufferInterface::Get( m_LutY, SVPitch, pitchY );

		long* plLutXData = nullptr;
		long* plLutYData = nullptr;
		SVMatroxBufferInterface::GetHostAddress( static_cast<void*>(&plLutXData), m_LutX );
		SVMatroxBufferInterface::GetHostAddress( static_cast<void*>(&plLutYData), m_LutY );

		for ( long y = 0; y < lOutputHeight; y++ )
		{
			for ( long x = 0; x < lOutputWidth; x++ )
			{
				SVPoint<double> point{static_cast<double> (x), static_cast<double> (y)};

				rOutputExtents.TranslateFromOutputSpace(point, point);
				plLutXData[ x + ( y * pitchY ) ] = static_cast<long> (point.m_x * 256L);
				plLutYData[ x + ( y * pitchX ) ] = static_cast<long> (point.m_y * 256L);
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

