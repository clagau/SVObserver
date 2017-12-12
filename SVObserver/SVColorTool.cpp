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
#include "Definitions/SVImageFormatEnum.h"
#include "SVColorTool.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "SVColorThreshold.h"
#include "SVGlobal.h"

#include "ToolSizeAdjustTask.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const BandEnum BandList[] = { BandEnum::Band0, BandEnum::Band1, BandEnum::Band2 };

SV_IMPLEMENT_CLASS( SVColorToolClass, SVColorToolClassGuid );
#pragma endregion Declarations

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
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (GetInspection());
		if (nullptr != pInspection && nullptr != pInspection->GetToolSet())
		{
			SVImageClass* pInputImage = getInputImage();
			if (nullptr != pInputImage)
			{
				//! We do not want the Logical ROI image showing up as an output image.
				m_LogicalROIImage.InitializeImage(pInputImage);
				m_LogicalROIImage.SetObjectAttributesAllowed(SvDef::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute);
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
					//! This is required to receive the correct extents when the camera image has an ROI set
					pInputImage->ResetObject();
					//! Converting configuration without ROI has to set the image to the full parent extents
					SetImageExtentToParent();
					// Converting configuration without ROI has to set all the thresholds to enabled
					SvDef::SVObjectTypeInfoStruct objectInfo;
					objectInfo.ObjectType = SvDef::SVOperatorObjectType;
					objectInfo.SubType = SvDef::SVColorThresholdObjectType;
					SVColorThresholdClass* pColorThreshold = dynamic_cast<SVColorThresholdClass*> (getFirstObject(objectInfo));
					if (nullptr != pColorThreshold)
					{
						for (BandEnum Band : BandList)
						{
							BandThreshold* pBandThreshold = pColorThreshold->GetBandThreshold(Band);
							if (nullptr != pBandThreshold)
							{
								pBandThreshold->m_ThresholdEnabled.SetValue(BOOL(true));
							}
						}
						SVBoolValueObjectClass* pOutputThresholdEnabled = pColorThreshold->GetOutputThresholdEnabled();
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
		for (BandEnum Band : BandList)
		{
			bOk &= createBandChildLayer(Band);
		}
	}
	
	ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true);

	// Set / Reset Printable Flag
	m_convertToHSI.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_hasROI.SetObjectAttributesAllowed(SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute);

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVColorToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result{ true };

	for (BandEnum Band : BandList)
	{
		createBandChildLayer(Band);
		m_bandImage[Band].ResetObject();
	}
	
	if (m_LogicalROIImage.InitializeImage(getInputImage()))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	if (m_OutputImage.InitializeImage(getInputImage()))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	Result = SVToolClass::ResetObject(pErrorMessages) && Result;

	if (!m_InputImageObjectInfo.IsConnected() || nullptr == m_InputImageObjectInfo.GetInputObjectInfo().m_pObject)
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
	HRESULT l_hrOk = m_svToolExtent.ValidExtentAgainstParentImage(rImageExtent);

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}

	return l_hrOk;
}

HRESULT SVColorToolClass::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_svToolExtent.UpdateExtentToParentExtents(NewExtent);

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

HRESULT SVColorToolClass::SetImageExtentToFit(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.UpdateExtentAgainstParentImage(rImageExtent);

	return l_hrOk;
}

SVTaskObjectClass* SVColorToolClass::GetObjectAtPoint(const SVExtentPointStruct &rPoint)
{
	SVImageExtentClass l_Extents;

	SVTaskObjectClass* pObject(nullptr);

	if (S_OK == m_svToolExtent.GetImageExtent(l_Extents) &&
		l_Extents.GetLocationPropertyAt(rPoint) != SVExtentLocationPropertyUnknown)
	{
		pObject = this;
	}
	return pObject;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool SVColorToolClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SVImageClass* pImage = getInputImage();
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

	SVImageBufferHandlePtr OutputImageHandle;
	if (m_OutputImage.SetImageHandleIndex(rRunStatus.Images) && m_OutputImage.GetImageHandle(OutputImageHandle))
	{
		SVImageBufferHandleImage milHandleTo;
		SVImageBufferHandleImage milHandleFrom;

		OutputImageHandle->GetData(milHandleTo);
		SVImageClass* pInputImage = getInputImage();
		if (m_LogicalROIImage.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp())
		{
			UpdateImageWithExtent();
		}
		SVImageBufferHandlePtr inputImageHandle;
		m_LogicalROIImage.GetParentImageHandle(inputImageHandle);
		if (nullptr != inputImageHandle)
		{
			inputImageHandle->GetData(milHandleFrom);
		}
		if (!milHandleTo.empty() && !milHandleFrom.empty())
		{
			HRESULT MatroxCode(S_OK);
			if (convertToHSI)
			{
				MatroxCode = SVMatroxImageInterface::Convert(milHandleTo.GetBuffer(), milHandleFrom.GetBuffer(), SVImageRGBToHLS);
			}
			else
			{
				MatroxCode = SVMatroxBufferInterface::CopyBuffer(milHandleTo.GetBuffer(), milHandleFrom.GetBuffer());
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

	for (BandEnum Band : BandList)
	{
		if (m_bandImage[Band].GetLastResetTimeStamp() <= m_OutputImage.GetLastResetTimeStamp())
		{
			Result &= m_bandImage[Band].ResetObject();
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVColorToolObjectType;

	m_InputImageObjectInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageColorType);
	m_InputImageObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_InputImageObjectInfo, _T("ColorToolInputImage"));

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1);
	RegisterEmbeddedObject(&m_LogicalROIImage, SVLogicalROIImageGuid, IDS_OBJECTNAME_IMAGE2);
	RegisterEmbeddedObject(&m_bandImage[Band0], SVBand0ImageObjectGuid, IDS_OBJECTNAME_BAND0_IMAGE);
	RegisterEmbeddedObject(&m_bandImage[Band1], SVBand1ImageObjectGuid, IDS_OBJECTNAME_BAND1_IMAGE);
	RegisterEmbeddedObject(&m_bandImage[Band2], SVBand2ImageObjectGuid, IDS_OBJECTNAME_BAND2_IMAGE);

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
	ImageInfo.SetTranslation(SVExtentTranslationNone);
	m_LogicalROIImage.UpdateImage(SV_GUID_NULL, ImageInfo);
	m_LogicalROIImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeLogical);
	m_OutputImage.UpdateImage(SV_GUID_NULL, ImageInfo);
	m_OutputImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);

	for (BandEnum Band : BandList)
	{
		m_bandImage[Band].InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeDependent);
	}

	ToolSizeAdjustTask::AddToFriendlist(this, true, true, true);

	//// Add the Color Threshold class object
	SVColorThresholdClass* pColorThreshold = new SVColorThresholdClass;
	if(pColorThreshold)
	{
		Add(pColorThreshold);
	}
}

SVImageClass* SVColorToolClass::getInputImage() const
{
	if (m_InputImageObjectInfo.IsConnected() && nullptr != m_InputImageObjectInfo.GetInputObjectInfo().m_pObject)
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when m_pObject is not nullptr then it is a SVImageClass
		return static_cast<SVImageClass*> (m_InputImageObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return nullptr;
}

bool SVColorToolClass::createBandChildLayer(BandEnum Band)
{
	bool l_bOk = false;

	SVGUID InputID = m_OutputImage.GetUniqueObjectID();
	SVImageInfoClass ImageInfo = m_OutputImage.GetImageInfo();

	// Setup...
	ImageInfo.SetOwner(GetUniqueObjectID());
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1L);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, static_cast<long> (Band));

	l_bOk = (S_OK == m_bandImage[Band].UpdateImage(InputID, ImageInfo));

	return l_bOk;
}

HRESULT SVColorToolClass::CollectInputImageNames()
{
	std::string Name;

	if (nullptr != getInputImage())
	{
		std::string Name = getInputImage()->GetCompleteName();
		m_SourceImageNames.SetDefaultValue(Name, true);
		return S_OK;
	}
	return E_FAIL;
}
#pragma endregion Private Methods