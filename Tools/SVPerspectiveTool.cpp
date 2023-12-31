//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Overlay.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* PERSPECTIVE_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
constexpr const char* PERSPECTIVE_WARP_TYPE_VERTICAL = _T("Vertical Warp");
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVPerspectiveToolClass, SvPb::PerspectiveToolClassId)
SVPerspectiveToolClass::SVPerspectiveToolClass(SVObjectClass* p_pOwner, int p_iStringResourceID)
	:SVToolClass(ToolExtType::All,p_pOwner, p_iStringResourceID)
	
{
	LocalInitialize();
	
	
}

SVPerspectiveToolClass::~SVPerspectiveToolClass()
{
	DestroyLUT();
}

bool SVPerspectiveToolClass::allowExtensionCopy() const 
{
	return false;
}

bool SVPerspectiveToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = SVToolClass::CreateObject(rCreateStructure);

	l_bOk &= ( S_OK == m_OutputImage.InitializeImage(m_InputImage.getInput<SvIe::SVImageClass>()));

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = l_bOk;

	return l_bOk;
}

bool SVPerspectiveToolClass::CloseObject()
{
	DestroyLUT();

	return SVToolClass::CloseObject();
}

HRESULT SVPerspectiveToolClass::UpdateOutputImageExtents()
{
	SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
	// Get Input Width and Height put in output Image Extent.
	const SVImageExtentClass& rInputExtents = (nullptr != pInputImage) ? pInputImage->GetImageExtents() : SVImageExtentClass();
	SVImageExtentClass OutputExtents = m_OutputImage.GetImageExtents();
	double l_dValue;
	/*HRESULT l_hrOk = */rInputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, l_dValue );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyWidth, l_dValue );

	/*l_hrOk = */rInputExtents.GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, l_dValue );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyHeight, l_dValue );

	// Set Translation
	bool l_bHideX{false};
	long l_lType{0L};
	m_svWarpType.GetValue( l_lType );
	WarpType l_eType = ( WarpType ) l_lType;
	if( l_eType == WarpTypeHorizontal )
	{
		OutputExtents.SetTranslation(SvPb::SVExtentTranslationHorizontalPerspective);
		l_bHideX = false;
	}
	else
	if( l_eType == WarpTypeVertical )
	{
		OutputExtents.SetTranslation(SvPb::SVExtentTranslationVerticalPerspective);
		l_bHideX = true;
	}
	/*l_hrOk = */OutputExtents.UpdateDataRecalculateOutput();


	// Set Position Points to zero for output image.
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyPositionPointX, 0 );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyPositionPointY, 0 );

	const SVImageExtentClass& rToolExtents = GetImageExtent();
	// Set tool Extent PositionPoints to Output Extent.
	/*l_hrOk = */rToolExtents.GetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, l_dValue );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, l_dValue );
	/*l_hrOk = */rToolExtents.GetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, l_dValue );
	/*l_hrOk = */OutputExtents.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, l_dValue );
	/*l_hrOk = */SetImageExtent(OutputExtents);


	/*l_hrOk = */OutputExtents.UpdateDataRecalculateOutput();

	uint32_t inputID = SvDef::InvalidObjectId;
	if(nullptr != pInputImage)
	{
		inputID = pInputImage->getObjectId();
	}

	SVImageInfoClass l_ImageInfo = m_OutputImage.GetImageInfo();

	l_ImageInfo.SetExtents( OutputExtents );

	HRESULT l_hrOk = m_OutputImage.UpdateImageSetParentAndImageInfo(inputID, l_ImageInfo);

	// Enable / Disable Extent Properties..
	SvIe::SVExtentPropertyInfoStruct info;
	if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyWidth, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyWidth, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}
	if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyHeight, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyHeight, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}
	if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyPositionPointX, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyPositionPointX, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyPositionPointY, info ) )
	{
		info.bHidden = true;

		if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyPositionPointY, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyTranslationOffsetX, info ) )
	{
		info.bHidden = l_bHideX;

		if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyTranslationOffsetX, info ) )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyTranslationOffsetY, info ) )
	{
		info.bHidden = !l_bHideX;

		if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyTranslationOffsetY, info ) )
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
	
	m_InputImage.validateInput();

	// Now the input image is valid!
	if( m_OutputImage.ResetObject(pErrorMessages) )
	{
		if( S_OK != CreateLUT() )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateLutFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
	else
	{
		Result = false;
	}

	SvIe::SVImageClass *pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();

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

SvPb::SVExtentLocationPropertyEnum SVPerspectiveToolClass::getLocationPropertyAtPoint( const SVPoint<double>& rPoint ) const
{
	return m_toolExtent.GetLocationPropertyAt(rPoint);
}

bool SVPerspectiveToolClass::DoesObjectHaveExtents() const
{
	return true;
}

bool SVPerspectiveToolClass::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	SvIe::SVImageClass *pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
	if ( nullptr != pInputImage && imageId == pInputImage->getObjectId() )
	{
		Result = true;
	}

	return Result;
}


bool SVPerspectiveToolClass::onRun( SvIe::RunStatus &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool l_bOk = SVToolClass::onRun( p_rRunStatus, pErrorMessages );

	if ( l_bOk )
	{
		SvIe::SVImageClass *pInputImage = m_InputImage.getInput<SvIe::SVImageClass>(true);

		if ( nullptr != pInputImage )
		{
			long Interpolation;
			m_svInterpolationMode.GetValue(Interpolation);
			SvOi::ITRCImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(p_rRunStatus.m_triggerRecord);
			SvOi::ITRCImagePtr pInputImageBuffer = pInputImage->getImageReadOnly(p_rRunStatus.m_triggerRecord.get());
			if (nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty() &&
				nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty())
			{
				SVMatroxImageInterface::Warp(pOutputImageBuffer->getHandle()->GetBuffer(),
				pInputImageBuffer->getHandle()->GetBuffer(), m_LutX, m_LutY, static_cast<SVImageOperationTypeEnum>(Interpolation) );

			}
			else
			{
				l_bOk = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVPerspectiveToolObjectType;

	// Identify our input image...
	m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_InputImage, SvDef::SourceImageInputName, SvPb::ImageInputEId);
	m_InputImage.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);

	// Register Embedded Objects
	RegisterEmbeddedImage( &m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_svXOffset, SvPb::TranslationXOffsetEId, IDS_X_OFFSET, false, SvOi::SVResetItemToolAndDependent, false);
	RegisterEmbeddedObject( &m_svYOffset, SvPb::TranslationYOffsetEId, IDS_Y_OFFSET, false, SvOi::SVResetItemToolAndDependent, false);

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemToolAndDependent, false);

	SetImageExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, &m_svXOffset );
	SetImageExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, &m_svYOffset );

	m_OutputImage.InitializeImage( SvPb::SVImageTypeEnum::SVImageTypePhysical );

	// Set Default Warp Method to Use Horizontal
	RegisterEmbeddedObject( &m_svWarpType, SvPb::WarpTypeEId, IDS_OBJECTNAME_WARPTYPE, false, SvOi::SVResetItemToolAndDependent, true);
	std::string EnumTypes = std::format("{}={},{}={}", PERSPECTIVE_WARP_TYPE_HORIZONTAL, static_cast<int>(WarpTypeHorizontal),
	                                   PERSPECTIVE_WARP_TYPE_VERTICAL, static_cast<int>(WarpTypeVertical));
	m_svWarpType.SetEnumTypes( EnumTypes.c_str() );
	m_svWarpType.SetDefaultValue( PERSPECTIVE_WARP_TYPE_VERTICAL );
	m_svWarpType.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	// Set Default Interpolation Mode to use Nearest Neighbor
	std::string Mode;
	std::string Text;
	EnumTypes.clear();

	// M_NEAREST_NEIGHBOR 
	Mode = SvUl::LoadStdString( IDS_NEAREST_NEIGHBOR_STRING );
	Text = std::format( _T( "{}={}," ), Mode, static_cast<int>(SVNearestNeighbor));
	EnumTypes += Text;
	// M_BILINEAR
	Mode = SvUl::LoadStdString( IDS_BILINEAR_STRING );
	Text = std::format( _T( "{}={}," ), Mode, static_cast<int>(SVBilinear)); // M_BILINEAR );
	EnumTypes += Text;
	// M_BICUBIC
	Mode = SvUl::LoadStdString( IDS_BICUBIC_STRING );
	Text = std::format( _T( "{}={}," ), Mode, static_cast<int>(SVBiCubic)); // M_BICUBIC );
	EnumTypes += Text;

	// And now set enum types...
	m_svInterpolationMode.SetEnumTypes( EnumTypes.c_str() );
	m_svInterpolationMode.SetDefaultValue( SVNearestNeighbor );	// Refer to MIL...
	RegisterEmbeddedObject( &m_svInterpolationMode, SvPb::OutputInterpolationModeEId, IDS_OBJECTNAME_INTERPOLATION_MODE, false, SvOi::SVResetItemNone, true);
}

HRESULT SVPerspectiveToolClass::CreateLUT()
{
	HRESULT l_hrOk = S_FALSE;

	DestroyLUT();
	
	long l_lWidth = 100;
	long l_lHeight = 100;
	if (nullptr != m_InputImage.getInput<SvIe::SVImageClass>())
	{
		GetImageExtent().GetExtentProperty( SvPb::SVExtentPropertyOutputWidth , l_lWidth );
		GetImageExtent().GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, l_lHeight );
		SVMatroxBufferCreateStruct l_Create;
		l_Create.m_eAttribute = SVBufAttLut;
		l_Create.m_eType = SV32BitSigned;
		l_Create.m_lSizeBand = 1;
		l_Create.m_lSizeX = l_lWidth;
		l_Create.m_lSizeY = l_lHeight;
		SVMatroxBufferInterface::Create( m_LutX, l_Create );
		SVMatroxBufferInterface::Create( m_LutY, l_Create );
	}
	SVPoint<double> point;

	if ( !m_LutX.empty() && !m_LutY.empty() )
	{

		long l_lPitchX = 0; //MbufInquire( m_LutX, M_PITCH, M_NULL );
		long l_lPitchY = 0; //MbufInquire( m_LutY, M_PITCH, M_NULL );
		SVMatroxBufferInterface::Get( m_LutX, SVPitch, l_lPitchX );
		SVMatroxBufferInterface::Get( m_LutY, SVPitch, l_lPitchY );

		long *l_plLutXData = nullptr; //(long *)MbufInquire( m_LutX, M_HOST_ADDRESS, M_NULL );
		long *l_plLutYData = nullptr; // (long *)MbufInquire( m_LutY, M_HOST_ADDRESS, M_NULL );
		SVMatroxBufferInterface::GetHostAddress( &l_plLutXData, m_LutX );
		SVMatroxBufferInterface::GetHostAddress( &l_plLutYData,m_LutY );

		for ( long j = 0; j < l_lHeight; j++ )
		{
			for ( long i = 0; i < l_lWidth; i++ )
			{
				point.m_x = i;
				point.m_y = j;
				GetImageExtent().TranslateFromOutputSpace( point, point );
				l_plLutXData[ i + ( j * l_lPitchX ) ] = static_cast<long> (point.m_x * 256L);
				l_plLutYData[ i + ( j * l_lPitchY ) ] = static_cast<long> (point.m_y * 256L);
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

HRESULT SVPerspectiveToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;

	double l_dOffset;
	double l_dValue;
	SVImageExtentClass NewExtent = rImageExtent;
	SvPb::SVExtentTranslationEnum ExtentTranslation = rImageExtent.GetTranslation( );

	if( ExtentTranslation == SvPb::SVExtentTranslationVerticalPerspective )
	{
		l_hrOk = NewExtent.GetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, l_dOffset );
		if( S_OK != NewExtent.GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, l_dValue) )
		{
			l_hrOk = S_FALSE;
		}
		if( S_OK == l_hrOk && l_dOffset >= l_dValue )
		{
			l_hrOk = NewExtent.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, l_dValue - 1 );
		}
		if( S_OK == l_hrOk &&  l_dOffset < 0 )
		{
			l_hrOk = NewExtent.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, 0 );
		}
		// X is always default with a Horizontal
		NewExtent.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, 0);
	}
	if( ExtentTranslation == SvPb::SVExtentTranslationHorizontalPerspective )
	{
		l_hrOk = NewExtent.GetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, l_dOffset );
		if( S_OK != NewExtent.GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, l_dValue) )
		{
			l_hrOk = S_FALSE;
		}
		if( S_OK == l_hrOk && l_dOffset >= l_dValue )
		{
			l_hrOk = NewExtent.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, l_dValue - 1 );
		}
		if( S_OK == l_hrOk &&  l_dOffset < 0 )
		{
			l_hrOk = NewExtent.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetX, 0 );
		}
		// Y is always default with a Vertical
		NewExtent.SetExtentProperty( SvPb::SVExtentPropertyTranslationOffsetY, 0);
	}

	l_hrOk = SVToolClass::SetImageExtent(NewExtent);

	return l_hrOk;
}

void SVPerspectiveToolClass::addOverlays(SvPb::OverlayDesc& rOverlay) const
{
	auto* pOverlay = rOverlay.add_overlays();
	pOverlay->set_name(GetName());
	pOverlay->set_objectid(getObjectId());
	pOverlay->set_displaybounding(true);
	auto* pBoundingBox = pOverlay->mutable_boundingshape();
	auto* pPerspective = pBoundingBox->mutable_perspective();

	long l_lType;
	m_svWarpType.GetValue(l_lType);
	WarpType l_eType = (WarpType)l_lType;
	if (l_eType == WarpTypeHorizontal)
	{
		pPerspective->set_orientation(SvPb::Orientation::x);
		SvPb::setValueObject(m_svXOffset, *pPerspective->mutable_offset());
	}
	else
	{
		pPerspective->set_orientation(SvPb::Orientation::y);
		SvPb::setValueObject(m_svYOffset, *pPerspective->mutable_offset());
	}
	
	setStateValueToOverlay(*pOverlay);
}

std::vector<std::string> SVPerspectiveToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 5> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Warp Function"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

SvVol::SVStringValueObjectClass* SVPerspectiveToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

} //namespace SvTo
