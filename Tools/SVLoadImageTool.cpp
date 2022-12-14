//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLoadImage
//* .File Name       : $Workfile:   SVToolLoadImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLoadImageTool.h"
#include "Definitions/SVImageFormatEnum.h"
#include "AnalyzerOperators/Analyzer.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

constexpr int GrayScaleBitDepth = 8;
SV_IMPLEMENT_CLASS( SVLoadImageToolClass, SvPb::LoadImageToolClassId );

SVLoadImageToolClass::SVLoadImageToolClass( SVObjectClass* POwner, int StringResourceID )
					 :SVToolClass(ToolExtType::All, POwner, StringResourceID )
{
	init();
}

void SVLoadImageToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVToolLoadImageObjectType;

	// Identify our input type needs
	
	// Register Embedded Objects
	RegisterEmbeddedImage( &m_fileImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_currentPathName, SvPb::PathNameEId, IDS_OBJECTNAME_PATHNAME, false, SvOi::SVResetItemToolAndDependent, true);
	RegisterEmbeddedObject( &m_continuousReload, SvPb::ContinuousReloadEId, IDS_OBJECTNAME_CONTINUOUS_RELOAD, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject(&m_isColorImage, SvPb::IsColorImageCheckEId, IDS_OBJECTNAME_COLOR_IMAGE, false, SvOi::SVResetItemNone, true);


	// Set Embedded defaults
	m_currentPathName.SetDefaultValue( _T( "" ), true );
	m_continuousReload.SetDefaultValue( false, true );

	m_fileImage.InitializeImage( SvPb::SVImageTypeEnum::SVImageTypePhysical );

	// Default taskObjectList items:

	// Set Translation
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationFigureShift);
	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, false);
}

SVLoadImageToolClass::~SVLoadImageToolClass()
{ 
}

bool SVLoadImageToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	if( bOk )
	{
		// Create Image...
		SVImageInfoClass ImageInfo = m_fileImage.GetImageInfo();

		// Setup...
		ImageInfo.SetOwner( getObjectId() );
		ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8 );
		ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);
		ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );
		ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned);

		bOk = (S_OK == m_fileImage.UpdateImageSetParentAndImageInfo(SvDef::InvalidObjectId, ImageInfo));
	}

	// Set / Reset Printable Flags
	m_fileImage.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute );
	m_currentPathName.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_continuousReload.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	if (bOk)
	{
		bOk = (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, false));
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVLoadImageToolClass::CloseObject()
{
	if (SVToolClass::CloseObject())
	{
		return m_fileImage.CloseObject();
	}
	return false;
}

bool SVLoadImageToolClass::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	// All inputs and outputs must be validated first
	if (ValidateLocal(pErrorMessages) && __super::onRun(rRunStatus, pErrorMessages))
	{
		BOOL bReload = false;
		std::string ImagePathName;

		if (S_OK != m_continuousReload.GetValue(bReload) || S_OK != m_currentPathName.GetValue(ImagePathName))
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}


		if (bReload || m_ReloadFileImage || nullptr == m_fileImage.getImageReadOnly(rRunStatus.m_triggerRecord.get()))
		{
			if (S_OK != m_fileImage.LoadImage(ImagePathName.c_str(), rRunStatus.m_triggerRecord))
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				rRunStatus.SetInvalid();
				return false;
			}

			m_ReloadFileImage = false;
		}
		



		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			//If not in runMode reload always, because outside of runMode it is not sure if the last image still valid.
			m_ReloadFileImage = true;
		}

		return true;
	}

	rRunStatus.SetInvalid();
	return false;
}

bool SVLoadImageToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);
	
	m_ReloadFileImage = true;

	bool correctModeForImage = true;
	BOOL bIsColorImage = false;
	m_isColorImage.GetValue(bIsColorImage);

	int imagePropertyFormat;
	SVImageInfoClass ImageInfo = m_fileImage.GetImageInfo();
	ImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, imagePropertyFormat);

	if (bIsColorImage)
	{
		SVImageFile FileImage;
		std::string ImagePathName;
		m_currentPathName.GetValue(ImagePathName);
		if (S_OK == SVImageFileLoader::LoadFirstFile(ImagePathName.c_str(), ImageFileFormat::any, FileImage))
		{
			bool selectedFileIsColor = FileImage.GetBitDepth() > GrayScaleBitDepth;
			if (selectedFileIsColor && SvDef::SVImageFormatBGR888X != imagePropertyFormat)
			{
				ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatBGR888X);
				ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3L);
				m_fileImage.UpdateImageSetParentAndImageInfo(SvDef::InvalidObjectId, ImageInfo);
				m_fileImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypePhysical);
			}
			else if (false == selectedFileIsColor && SvDef::SVImageFormatBGR888X == imagePropertyFormat)
			{
				if (S_OK != Result && nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_IsColorCheckForGrayscaleImage, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				correctModeForImage = false;
			}
		}
	}
	else
	{
		if (SvDef::SVImageFormatMono8 != imagePropertyFormat)
		{
			ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8);
			ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1L);
			m_fileImage.UpdateImageSetParentAndImageInfo(SvDef::InvalidObjectId, ImageInfo);
			m_fileImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypePhysical);
		}
	}

	UpdateImageWithExtent();

	return Result && ValidateLocal(pErrorMessages) && correctModeForImage;
}

bool SVLoadImageToolClass::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	//! Use static_cast to avoid time penalty in run mode for dynamic_cast
	//! We are sure that when m_pObject is not nullptr then it is a SVImageClass
	SvIe::SVImageClass* pImage = static_cast<SvIe::SVImageClass*> (m_pCurrentToolSet->getCurrentImageInterface());
	if (nullptr != pImage && imageId == pImage->getObjectId())
	{
		Result = true;
	}
	else
	{
		Result = SVToolClass::isInputImage(imageId);
	}

	return Result;
}

SVToolClass* SVLoadImageToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != m_toolExtent.GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool SVLoadImageToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVLoadImageToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;

	const SVExtentFigureStruct& rFigure = rImageExtent.GetFigure();

	if ( (rFigure.m_svTopLeft.m_x >= 0) && (rFigure.m_svTopLeft.m_y >= 0) )
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVLoadImageToolClass::SetImageExtentToParent()
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass NewExtent;
	std::string ImagePathName;

	if (S_OK == l_hrOk)
	{
		l_hrOk = m_currentPathName.GetValue(ImagePathName);
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = NewExtent.SetDataFromFile(ImagePathName.c_str());
	}

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

HRESULT SVLoadImageToolClass::GetParentExtent( SVImageExtentClass& rParentExtent ) const
{
	HRESULT hr = S_OK;
	std::string ImagePathName;
	if( S_OK == hr )
	{
		hr = m_currentPathName.GetValue( ImagePathName );
	}
	if( S_OK == hr )
	{
		hr = rParentExtent.SetDataFromFile(ImagePathName.c_str());
	}
	return hr;
}

std::vector<std::string> SVLoadImageToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 5> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Size and Position"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

bool SVLoadImageToolClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	std::string PathName;
	m_currentPathName.GetValue( PathName );

	if(0 == _access(PathName.c_str(), 0))
	{
		return true;
	}

	if (nullptr != pErrorMessages)
	{
		SvStl::MessageContainer message( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
		pErrorMessages->push_back(message);
	}
	return false;
}

} //namespace SvTo
