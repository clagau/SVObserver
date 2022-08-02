//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* CYLINDRICAL_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
constexpr const char* CYLINDRICAL_WARP_TYPE_VERTICAL = _T("Vertical Warp");
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVCylindricalWarpToolClass, SvPb::CylindricalWarpToolClassId)

SVCylindricalWarpToolClass::SVCylindricalWarpToolClass( SVObjectClass* p_pOwner, int p_iStringResourceID )
	: SVToolClass(ToolExtType::None,p_pOwner, p_iStringResourceID)
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
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVToolCylindricalObjectType;

	// Identify our input image...
	m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_InputImage, SvDef::SourceImageInputName, SvPb::ImageInputEId);
	m_InputImage.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);

	// Register Embedded Objects
	RegisterEmbeddedImage( &m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1 );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	m_OutputImage.InitializeImage( SvPb::SVImageTypeEnum::SVImageTypePhysical );

	// Set Default Warp Method to Use Horizontal
	RegisterEmbeddedObject( &m_svWarpType, SvPb::WarpTypeEId, IDS_OBJECTNAME_WARPTYPE, false, SvOi::SVResetItemTool );
	std::string EnumTypes = SvUl::Format( _T("%s=%d,%s=%d"), CYLINDRICAL_WARP_TYPE_HORIZONTAL, WarpTypeHorizontal,
	                                   CYLINDRICAL_WARP_TYPE_VERTICAL, WarpTypeVertical);
	m_svWarpType.SetEnumTypes( EnumTypes.c_str() );
	m_svWarpType.SetDefaultValue( CYLINDRICAL_WARP_TYPE_HORIZONTAL, true);
	m_svWarpType.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

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
	RegisterEmbeddedObject( &m_svInterpolationMode, SvPb::OutputInterpolationModeEId, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone );



	// Set Default Warp Angle to the maximum 180 degrees.
	// Using start angle of 90. In the future we will use end angle to 
	// allow an unequal warp tool.
	// Currently we double the start angle and make it symetrical
	// the work is done in SVImageExtentClass.
	RegisterEmbeddedObject( &m_svWarpAngle, SvPb::WarpAngleEId, IDS_OBJECTNAME_WARPANGLE, false, SvOi::SVResetItemTool );
	m_svWarpAngle.SetDefaultValue( 180.0, true);
	m_svWarpAngle.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_toolExtent.SetExtentObject( SvPb::SVExtentPropertyStartAngle, &m_svWarpAngle );

}

HRESULT SVCylindricalWarpToolClass::LocalCreate()
{
	HRESULT l_hr= S_OK;

	HRESULT l_hrOk = S_OK;
	if ( S_OK == LocalDestroy() )
	{
		SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
		uint32_t inputID = nullptr != pInputImage ? pInputImage->getObjectId() : m_OutputImage.getObjectId();
		SVImageInfoClass l_ImageInfo = nullptr != pInputImage ? pInputImage->GetImageInfo() : m_OutputImage.GetImageInfo();
		SVImageExtentClass imageExtents = nullptr != pInputImage ? pInputImage->GetImageExtents() : m_OutputImage.GetImageExtents();

		// Warp Type..
		long lWarpType = SvPb::SVExtentTranslationCylindricalWarpH;	// default
		m_svWarpType.GetValue( lWarpType );
		SvPb::SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ? SvPb::SVExtentTranslationCylindricalWarpV : SvPb::SVExtentTranslationCylindricalWarpH;
		imageExtents.SetTranslation( eTranslation );

		// Warp Angle
		double l_dAngle = 180;
		l_hrOk = m_svWarpAngle.GetValue( l_dAngle );
		if( S_OK != (l_hr = imageExtents.SetExtentProperty( SvPb::SVExtentPropertyStartAngle, l_dAngle )) )
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
		l_ImageInfo.SetOwner( getObjectId() );
		l_ImageInfo.SetExtents(imageExtents);

		m_OutputImage.SetObjectOwner( this );

		l_hrOk = m_OutputImage.UpdateImage( inputID, l_ImageInfo );

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
	SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
	// Get Input Width and Height put in output Image Extent.
	const SVImageExtentClass rInputExtents = (nullptr != pInputImage) ? pInputImage->GetImageExtents() : SVImageExtentClass();
	SVImageExtentClass OutputExtents = m_OutputImage.GetImageExtents();
	
	// Set Translation
	long lWarpType = SvPb::SVExtentTranslationCylindricalWarpH;	// default
	/*HRESULT l_hrOk =*/ m_svWarpType.GetValue( lWarpType );
	SvPb::SVExtentTranslationEnum eTranslation = lWarpType == WarpTypeVertical ? SvPb::SVExtentTranslationCylindricalWarpV : SvPb::SVExtentTranslationCylindricalWarpH;
	OutputExtents.SetTranslation( eTranslation );

	// Set Warp Angle
	double l_dStartAngle;
	m_svWarpAngle.GetValue( l_dStartAngle );
	ValidateAngle( l_dStartAngle );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyStartAngle, l_dStartAngle );

	double l_dValue;
	/*l_hrOk = */rInputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, l_dValue );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyWidth, l_dValue);

	/*l_hrOk = */rInputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, l_dValue );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyHeight, l_dValue );

	/*l_hrOk = */OutputExtents.UpdateData();

	/*l_hrOk = */SetImageExtent( OutputExtents );

	uint32_t inputID = SvDef::InvalidObjectId;

	if(nullptr != pInputImage)
	{
		inputID = pInputImage->getObjectId();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( OutputExtents );

	return m_OutputImage.UpdateImage(inputID, l_ImageInfo );
}

bool SVCylindricalWarpToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	m_InputImage.validateInput();

	HRESULT l_hrOk = UpdateOutputImageExtents();
	if (S_OK != l_hrOk)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateOutputImageExtentsFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
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
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	UpdateImageWithExtent();

	return Result;
}

std::vector<std::string> SVCylindricalWarpToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 5> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Warp Parameters"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

bool SVCylindricalWarpToolClass::DoesObjectHaveExtents() const
{
	return false;
}

bool SVCylindricalWarpToolClass::onRun( SvIe::RunStatus& p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool l_bOk = __super::onRun( p_rRunStatus, pErrorMessages );

	if (l_bOk)
	{
		SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>(true);

		const SVImageExtentClass& rToolExtents = GetImageExtent();
		
		SVImageExtentClass InputExtents = (nullptr != pInputImage) ? pInputImage->GetImageExtents() : SVImageExtentClass();
		long l_dInputWidth, l_dToolWidth, l_dInputHeight, l_dToolHeight;
		long Interpolation;
		m_svInterpolationMode.GetValue(Interpolation);

		l_bOk = (S_OK == InputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, l_dInputWidth )) && l_bOk;
		l_bOk = (S_OK == rToolExtents.GetExtentProperty( SvPb::SVExtentPropertyWidth, l_dToolWidth )) && l_bOk;
		l_bOk = (S_OK == InputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, l_dInputHeight )) && l_bOk;
		l_bOk = (S_OK == rToolExtents.GetExtentProperty( SvPb::SVExtentPropertyHeight, l_dToolHeight )) && l_bOk;

		if (!l_bOk)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}

		if( (l_dInputWidth != l_dToolWidth) || (l_dInputHeight != l_dToolHeight) )
		{
			l_bOk = ResetObject(pErrorMessages) && l_bOk;
		}

		if ( nullptr != pInputImage )
		{
			SvOi::ITRCImagePtr pInputImageBuffer = pInputImage->getImageReadOnly(p_rRunStatus.m_triggerRecord.get());
			SvOi::ITRCImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(p_rRunStatus.m_triggerRecord);

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
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunWarpFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
	rOutputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, lOutputWidth );
	rOutputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, lOutputHeight );

	SVMatroxBufferCreateStruct l_Create;
	l_Create.m_eAttribute = SVBufAttLut;
	l_Create.m_eType = SV32BitSigned;
	l_Create.m_lSizeX = lOutputWidth;
	l_Create.m_lSizeY = lOutputHeight;
	SVMatroxBufferInterface::Create( m_LutX, l_Create );
	SVMatroxBufferInterface::Create( m_LutY, l_Create );
	assert( !m_LutX.empty() );
	assert( !m_LutY.empty() );

	if ( !m_LutX.empty() && !m_LutY.empty() )
	{
		long pitchX;
		long pitchY;
		SVMatroxBufferInterface::Get( m_LutX, SVPitch, pitchX );
		SVMatroxBufferInterface::Get( m_LutY, SVPitch, pitchY );

		long* plLutXData = nullptr;
		long* plLutYData = nullptr;
		SVMatroxBufferInterface::GetHostAddress( &plLutXData, m_LutX );
		SVMatroxBufferInterface::GetHostAddress( &plLutYData, m_LutY );

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

SvVol::SVStringValueObjectClass* SVCylindricalWarpToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

} //namespace SvTo
