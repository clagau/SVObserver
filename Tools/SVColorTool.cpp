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
#include "SVColorTool.h"
#include "Definitions/SVImageFormatEnum.h"
#include "Definitions/TextDefineSvDef.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVColorThreshold.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVColorToolClass, SVColorToolClassGuid );

#pragma region Constructor
SVColorToolClass::SVColorToolClass( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
{
	LocalInitialize();
}

SVColorToolClass::~SVColorToolClass()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVColorToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = true;

	if( SVToolClass::CreateObject(rCreateStructure) )
	{
		SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
		if (nullptr != pInspection && nullptr != pInspection->GetToolSetInterface())
		{
			SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
			if (nullptr != pInputImage)
			{
				//! We do not want the Logical ROI image showing up as an output image.
				m_LogicalROIImage.InitializeImage(pInputImage);
				m_LogicalROIImage.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
				m_OutputImage.InitializeImage(pInputImage);

				BOOL hasROI(false);
				m_hasROI.GetValue(hasROI);

				if (!hasROI && SVSVIMStateClass::CheckState(SV_STATE_LOADING))
				{
					m_ConvertTool = true;
				}
				//! If this is true then we need to convert it
				if (m_ConvertTool)
				{
					//! Converting configuration without ROI has to set the image to the full parent extents
					SetImageExtentToParent();
					// Converting configuration without ROI has to set all the thresholds to enabled
					SvDef::SVObjectTypeInfoStruct objectInfo;
					objectInfo.ObjectType = SvPb::SVOperatorObjectType;
					objectInfo.SubType = SvPb::SVColorThresholdObjectType;
					SvOp::SVColorThresholdClass* pColorThreshold = dynamic_cast<SvOp::SVColorThresholdClass*> (getFirstObject(objectInfo));
					if (nullptr != pColorThreshold)
					{
						for (SvDef::BandEnum Band : SvDef::BandList)
						{
							SvOp::BandThreshold* pBandThreshold = pColorThreshold->GetBandThreshold(Band);
							if (nullptr != pBandThreshold)
							{
								pBandThreshold->m_ThresholdEnabled.SetValue(BOOL(true));
							}
						}
						SvVol::SVBoolValueObjectClass* pOutputThresholdEnabled = pColorThreshold->GetOutputThresholdEnabled();
						if (nullptr != pOutputThresholdEnabled)
						{
							pOutputThresholdEnabled->SetValue(BOOL(true));
						}
					}
				}
			}
			//! Now set the value to ROI so no further conversion takes place
			m_hasROI.SetValue(BOOL(true));
		}

		// Create 3 output images, one for each band...
		for (SvDef::BandEnum Band : SvDef::BandList)
		{
			bOk &= createBandChildLayer(Band);
		}
	}
	
	SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true);

	// Set / Reset Printable Flag
	m_convertToHSI.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_hasROI.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVColorToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result{ true };

	SvOl::ValidateInput(m_InputImageObjectInfo);

	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		//Create band already calls reset for the image!
		createBandChildLayer(Band);
	}
	
	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
	if (m_LogicalROIImage.InitializeImage(pInputImage))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	if (m_OutputImage.InitializeImage(pInputImage))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	Result = SVToolClass::ResetObject(pErrorMessages) && Result;

	if (nullptr == pInputImage)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	CollectInputImageNames();

	UpdateImageWithExtent();

	return Result;
}

HRESULT SVColorToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = m_toolExtent.ValidExtentAgainstParentImage(rImageExtent);

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}

	return l_hrOk;
}

HRESULT SVColorToolClass::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_toolExtent.UpdateExtentToParentExtents(NewExtent);

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

HRESULT SVColorToolClass::SetImageExtentToFit(const SVImageExtentClass& rImageExtent)
{
	return m_toolExtent.UpdateExtentAgainstParentImage(rImageExtent);;
}

SVToolClass* SVColorToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool SVColorToolClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SvIe::SVImageClass* pImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
	if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}
	return Result;
}

bool SVColorToolClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result(true);
	BOOL convertToHSI(true);

	m_convertToHSI.GetValue(convertToHSI);

	SvTrc::IImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
	if (nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
	{
		SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo, true);
		if (nullptr != pInputImage)
		{
			SvTrc::IImagePtr pInputImageBuffer = m_LogicalROIImage.getImageReadOnly(rRunStatus.m_triggerRecord.get());
			if (nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty())
			{
				HRESULT MatroxCode(S_OK);
				if (convertToHSI)
				{
					MatroxCode = SVMatroxImageInterface::Convert(pOutputImageBuffer->getHandle()->GetBuffer(), pInputImageBuffer->getHandle()->GetBuffer(), SVImageRGBToHLS);
				}
				else
				{
					MatroxCode = SVMatroxBufferInterface::CopyBuffer(pOutputImageBuffer->getHandle()->GetBuffer(), pInputImageBuffer->getHandle()->GetBuffer());
				}
				if (S_OK != MatroxCode)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
						pErrorMessages->push_back(Msg);
					}
				}
			}
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
	}

	Result &= __super::onRun(rRunStatus, pErrorMessages);
	
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVColorToolClass::LocalInitialize()
{
	m_canResizeToParent = true;
	m_ConvertTool = false;
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVColorToolObjectType;

	m_InputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageColorType);
	m_InputImageObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_InputImageObjectInfo, SvDef::cColorToolInputImage);

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1);
	RegisterEmbeddedObject(&m_LogicalROIImage, SVLogicalROIImageGuid, IDS_OBJECTNAME_ROIIMAGE);
	RegisterEmbeddedObject(&m_bandImage[SvDef::Band0], SVBand0ImageObjectGuid, IDS_OBJECTNAME_BAND0_IMAGE);
	RegisterEmbeddedObject(&m_bandImage[SvDef::Band1], SVBand1ImageObjectGuid, IDS_OBJECTNAME_BAND1_IMAGE);
	RegisterEmbeddedObject(&m_bandImage[SvDef::Band2], SVBand2ImageObjectGuid, IDS_OBJECTNAME_BAND2_IMAGE);

	RegisterEmbeddedObject(&m_convertToHSI, SVConvertToHSIObjectGuid, IDS_OBJECTNAME_CONVERT_TO_HSI, true, SvOi::SVResetItemIP);
	RegisterEmbeddedObject(&m_hasROI, SVHasROIObjectGuid, IDS_OBJECTNAME_HAS_ROI, true, SvOi::SVResetItemIP);
	m_convertToHSI.SetDefaultValue(BOOL(false), true);
	m_hasROI.SetDefaultValue(BOOL(false), true);

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject(&m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool);

	SVImageInfoClass ImageInfo = m_OutputImage.GetImageInfo();
	//! Set Output image to color
	ImageInfo.SetOwner(GetUniqueObjectID());
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatRGB8888);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3L);
	ImageInfo.SetTranslation(SvPb::SVExtentTranslationNone);
	m_LogicalROIImage.UpdateImage(GUID_NULL, ImageInfo);
	m_LogicalROIImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeLogical);
	m_OutputImage.UpdateImage(GUID_NULL, ImageInfo);
	m_OutputImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);

	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		m_bandImage[Band].InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeDependent);
	}

	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, true);

	//// Add the Color Threshold class object
	SvOp::SVColorThresholdClass* pColorThreshold = new SvOp::SVColorThresholdClass;
	if(nullptr != pColorThreshold)
	{
		Add(pColorThreshold);
	}
}

bool SVColorToolClass::createBandChildLayer(SvDef::BandEnum Band)
{
	bool l_bOk = false;

	SVGUID InputID = m_OutputImage.GetUniqueObjectID();
	SVImageInfoClass ImageInfo = m_OutputImage.GetImageInfo();

	// Setup...
	ImageInfo.SetOwner(GetUniqueObjectID());
	//The single bands do not need the offset position only width and size!
	ImageInfo.SetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPoint, {0.0, 0.0});
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1L);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, static_cast<long> (Band));

	l_bOk = (S_OK == m_bandImage[Band].UpdateImage(InputID, ImageInfo));

	return l_bOk;
}

HRESULT SVColorToolClass::CollectInputImageNames()
{
	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
	if (nullptr != pInputImage)
	{
		std::string Name = pInputImage->GetCompleteName();
		m_SourceImageNames.SetDefaultValue(Name, true);
		return S_OK;
	}
	return E_FAIL;
}
#pragma endregion Private Methods

} //namespace SvTo
