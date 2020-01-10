//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorThreshold
//* .File Name       : $Workfile:   SVColorThreshold.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:10:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVColorThreshold.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVColorThresholdClass, SVColorThresholdClassGuid)

#pragma region Constructor
SVColorThresholdClass::SVColorThresholdClass(SVObjectClass* POwner, int StringResourceID)
	:SVOperatorClass(POwner, StringResourceID)
{
	LocalInitialize();
}

SVColorThresholdClass::~SVColorThresholdClass()
{
	SVColorThresholdClass::CloseObject();
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVColorThresholdClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVOperatorClass::CreateObject(rCreateStructure);

	// Owner can only be : SVOperatorClass !!!
	if (bOk)
	{
		try
		{
			// Create 4 Output Images
			bOk = createImages();
		}
		catch (...)
		{
		}
	}
	const UINT cAttributes = SvPb::printable | SvPb::remotelySetable | SvPb::setableOnline;
	// Set / Reset Printable Flags 
	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		m_BandThreshold[Band].m_UpperThreshold.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
		m_BandThreshold[Band].m_LowerThreshold.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
		m_BandThreshold[Band].m_ThresholdExclude.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
	}

	m_ExtentLeft.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentTop.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentWidth.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeight.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool SVColorThresholdClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	for (auto& rEntry : m_BandThreshold)
	{
		SvOl::ValidateInput(rEntry.m_InputImage);
	}


	double l_dWidth = 0.0;
	double l_dHeight = 0.0;

	m_ExtentWidth.GetValue(l_dWidth);
	m_ExtentHeight.GetValue(l_dHeight);

	// Recalculate pixel number...
	m_PixelNumber = ((__int64)l_dWidth) * ((__int64)l_dHeight);

	createImages();

	//If no enabled Threshold Bands then disable Output Threshold Image as well
	int EnabledThresholdCount(0);
	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		BOOL Enabled(false);
		m_BandThreshold[Band].m_ThresholdEnabled.GetValue(Enabled);
		if (Enabled)
		{
			EnabledThresholdCount++;
		}
	}
	if (0 == EnabledThresholdCount)
	{
		m_OutputThresholdEnabled.SetValue(BOOL(false));
	}

	bool Result = ValidateLocal();
	if (!Result)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	return SVOperatorClass::ResetObject(pErrorMessages) && Result;
}

BandThreshold* SVColorThresholdClass::GetBandThreshold(SvDef::BandEnum Band)
{
	if (SvDef::BandEnum::Band0 <= Band && SvDef::BandEnum::Band2 >= Band)
	{
		return &m_BandThreshold[Band];
	}
	return nullptr;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool SVColorThresholdClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	// Binarizing: lowerThresh <= x <= upperTresh		--> 255 
	//	 		   otherwise							--> 0

	long LowerThreshold[SvDef::BandEnum::BandNumber] = {0L, 0L, 0L};
	long UpperThreshold[SvDef::BandEnum::BandNumber] = {0L, 0L, 0L};
	BOOL ThresholdExclude[SvDef::BandEnum::BandNumber] = {FALSE, FALSE, FALSE};
	BOOL ThresholdEnabled[SvDef::BandEnum::BandNumber] = {FALSE, FALSE, FALSE};
	std::vector<SvDef::BandEnum> ThresholdEnabledVector;

	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		// Get current Threshold Flags and Values for each Band
		m_BandThreshold[Band].m_LowerThreshold.GetValue(LowerThreshold[Band]);
		m_BandThreshold[Band].m_UpperThreshold.GetValue(UpperThreshold[Band]);
		m_BandThreshold[Band].m_ThresholdExclude.GetValue(ThresholdExclude[Band]);
		m_BandThreshold[Band].m_ThresholdEnabled.GetValue(ThresholdEnabled[Band]);
		if (ThresholdEnabled[Band])
		{
			ThresholdEnabledVector.push_back(Band);
		}
	}
	SvTrc::IImagePtr pBandImageArray[SvDef::BandEnum::BandNumber];
	SvTrc::IImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
	SvTrc::IImagePtr pInputImageBufferArray[SvDef::BandEnum::BandNumber];

	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		pBandImageArray[Band] = GetBandOutputImage(Band).getImageToWrite(rRunStatus.m_triggerRecord);
		SvIe::SVImageClass* pInputImage = GetBandInputImage(Band, true);
		if (nullptr != pInputImage)
		{
			pInputImageBufferArray[Band] = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		}
		Result &= (nullptr != pBandImageArray[Band]) && !pBandImageArray[Band]->isEmpty() && nullptr != pInputImageBufferArray[Band] && !pInputImageBufferArray[Band]->isEmpty();
	}

	Result &= (nullptr != pOutputImageBuffer) && !pOutputImageBuffer->isEmpty();

	if (!Result)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	if (Result)
	{
		bool isBinarize(true);

		for (SvDef::BandEnum Band : SvDef::BandList)
		{
			//! Binarize only if band is enabled
			if (ThresholdEnabled[Band])
			{
				isBinarize &= Binarize(LowerThreshold[Band], UpperThreshold[Band], ThresholdExclude[Band], pInputImageBufferArray[Band], pBandImageArray[Band]);
			}
		}

		if (!isBinarize)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_BinarizeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (Result)
	{
		BOOL OutputThresholdEnabled;
		m_OutputThresholdEnabled.GetValue(OutputThresholdEnabled);
		if (OutputThresholdEnabled)
		{
			switch (ThresholdEnabledVector.size())
			{
				case 1LL:
					//If only one Threshold image enabled then just copy it to Output image
					SVMatroxBufferInterface::CopyBuffer(pOutputImageBuffer->getHandle()->GetBuffer(), pBandImageArray[ThresholdEnabledVector[0]]->getHandle()->GetBuffer());
					break;
				case 2LL:
					//! The Output image is a bitwise AND of 2 Threshold images
					SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pBandImageArray[ThresholdEnabledVector[0]]->getHandle()->GetBuffer(), pBandImageArray[ThresholdEnabledVector[1]]->getHandle()->GetBuffer(), SVImageAnd);
					break;
				case 3LL:
					//! The Output image is a bitwise AND of all 3 Threshold images
					SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pBandImageArray[SvDef::BandEnum::Band0]->getHandle()->GetBuffer(), pBandImageArray[SvDef::BandEnum::Band1]->getHandle()->GetBuffer(), SVImageAnd);
					SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pBandImageArray[SvDef::BandEnum::Band2]->getHandle()->GetBuffer(), pOutputImageBuffer->getHandle()->GetBuffer(), SVImageAnd);
					break;
				default:
					assert(false);
					break;
			}
		}
	}

	if (!Result)
	{
		// Signal that something was wrong...
		rRunStatus.SetInvalid();
	}

	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVColorThresholdClass::LocalInitialize()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVColorThresholdObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject(&m_OutputThresholdEnabled, SVColorThresholdEnabledObjectGuid, IDS_OBJECTNAME_OUTPUT_THRESHOLDENABLED, false, SvOi::SVResetItemNone);

	/////////////////////////////////////////////////////////////////////////
	// Image Outputs
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band0].m_OutputImage, SVBand0ImageObjectGuid, IDS_OBJECTNAME_BAND0_IMAGE);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band1].m_OutputImage, SVBand1ImageObjectGuid, IDS_OBJECTNAME_BAND1_IMAGE);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band2].m_OutputImage, SVBand2ImageObjectGuid, IDS_OBJECTNAME_BAND2_IMAGE);
	RegisterEmbeddedObject(&m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1);

	/////////////////////////////////////////////////////////////////////////
	// Band 0 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band0].m_UpperThreshold, SVBand0UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND0_UPPERTHRESHOLD, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band0].m_LowerThreshold, SVBand0LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND0_LOWERTHRESHOLD, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band0].m_ThresholdExclude, SVBand0ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND0_THRESHOLDEXCLUDE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band0].m_ThresholdEnabled, SVBand0ThresholdEnabledObjectGuid, IDS_OBJECTNAME_BAND0_THRESHOLDENABLED, false, SvOi::SVResetItemNone);

	/////////////////////////////////////////////////////////////////////////
	// Band 1 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band1].m_UpperThreshold, SVBand1UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND1_UPPERTHRESHOLD, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band1].m_LowerThreshold, SVBand1LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND1_LOWERTHRESHOLD, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band1].m_ThresholdExclude, SVBand1ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND1_THRESHOLDEXCLUDE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band1].m_ThresholdEnabled, SVBand1ThresholdEnabledObjectGuid, IDS_OBJECTNAME_BAND1_THRESHOLDENABLED, false, SvOi::SVResetItemNone);

	/////////////////////////////////////////////////////////////////////////
	// Band 2 Threshold
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band2].m_UpperThreshold, SVBand2UpperThresholdObjectGuid, IDS_OBJECTNAME_BAND2_UPPERTHRESHOLD, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band2].m_LowerThreshold, SVBand2LowerThresholdObjectGuid, IDS_OBJECTNAME_BAND2_LOWERTHRESHOLD, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band2].m_ThresholdExclude, SVBand2ThresholdExcludeObjectGuid, IDS_OBJECTNAME_BAND2_THRESHOLDEXCLUDE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_BandThreshold[SvDef::BandEnum::Band2].m_ThresholdEnabled, SVBand2ThresholdEnabledObjectGuid, IDS_OBJECTNAME_BAND2_THRESHOLDENABLED, false, SvOi::SVResetItemNone);

	/////////////////////////////////////////////////////////////////////////
	// Train Color ROI extents
	/////////////////////////////////////////////////////////////////////////
	//Special type names for extents
	m_ExtentWidth.SetTypeName(_T("Extent Width"));
	m_ExtentHeight.SetTypeName(_T("Extent Height"));
	m_ExtentLeft.SetTypeName(_T("Extent X"));
	m_ExtentTop.SetTypeName(_T("Extent Y"));
	RegisterEmbeddedObject(&m_ExtentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_ExtentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_ExtentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_ExtentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemOwner);

	m_ExtentLeft.SetDefaultValue(SvDef::cDefaultWindowToolLeft, true);
	m_ExtentTop.SetDefaultValue(SvDef::cDefaultWindowToolTop, true);
	m_ExtentWidth.SetDefaultValue(SvDef::cDefaultWindowToolWidth, true);
	m_ExtentHeight.SetDefaultValue(SvDef::cDefaultWindowToolHeight, true);

	m_PixelNumber = 0;

	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		m_BandThreshold[Band].m_UpperThreshold.SetDefaultValue(SvDef::cDefaultToolUpperThreshold, true);
		m_BandThreshold[Band].m_LowerThreshold.SetDefaultValue(SvDef::cDefaultToolLowerThreshold, true);
		m_BandThreshold[Band].m_ThresholdExclude.SetDefaultValue(BOOL(false), true);
		GetBandOutputImage(Band).InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeIndependent);
	}

	m_OutputImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeIndependent);

	// Identify our input type needs...
	m_BandThreshold[SvDef::BandEnum::Band0].m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType, SVBand0ImageObjectGuid);
	m_BandThreshold[SvDef::BandEnum::Band0].m_InputImage.SetObject(GetObjectInfo());
	RegisterInputObject(&m_BandThreshold[SvDef::BandEnum::Band0].m_InputImage, _T("ColorThresholdBand0Image"));

	m_BandThreshold[SvDef::BandEnum::Band1].m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType, SVBand1ImageObjectGuid);
	m_BandThreshold[SvDef::BandEnum::Band1].m_InputImage.SetObject(GetObjectInfo());
	RegisterInputObject(&m_BandThreshold[SvDef::BandEnum::Band1].m_InputImage, _T("ColorThresholdBand1Image"));

	m_BandThreshold[SvDef::BandEnum::Band2].m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType, SVBand2ImageObjectGuid);
	m_BandThreshold[SvDef::BandEnum::Band2].m_InputImage.SetObject(GetObjectInfo());
	RegisterInputObject(&m_BandThreshold[SvDef::BandEnum::Band2].m_InputImage, _T("ColorThresholdBand2Image"));

	addDefaultInputObjects();
}

bool SVColorThresholdClass::createImages()
{
	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		createOutputImage(GetBandInputImage(Band), &GetBandOutputImage(Band));
	}
	createOutputImage(&GetBandOutputImage(SvDef::BandEnum::Band0), &m_OutputImage);

	m_OutputImage.InitializeImage(&GetBandOutputImage(SvDef::BandEnum::Band0));

	return true;
}

bool SVColorThresholdClass::createOutputImage(SvIe::SVImageClass* pInputImage, SvIe::SVImageClass* pOutputImage)
{
	bool Result(false);

	SVGUID InputID;
	SVImageInfoClass ImageInfo;

	if (nullptr != pInputImage)
	{
		InputID = pInputImage->GetUniqueObjectID();
		ImageInfo = pInputImage->GetImageInfo();
	}
	else
	{
		ImageInfo = pOutputImage->GetImageInfo();
	}

	// Setup...
	ImageInfo.SetOwner(GetParentID());

	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0);
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);

	Result = (S_OK == pOutputImage->UpdateImage(InputID, ImageInfo));

	return Result;
}

bool SVColorThresholdClass::Binarize(long lower, long upper, BOOL bExclude, SvTrc::IImagePtr pInputImage, SvTrc::IImagePtr pOutputImage)
{
	bool Result(false);

	if (nullptr != pInputImage && !(pInputImage->isEmpty()) && nullptr != pOutputImage && !(pOutputImage->isEmpty()))
	{
		HRESULT MatroxCode;
		double minVal(0.0);
		double maxVal(0.0);

		if (lower > upper)
		{
			minVal = (double)upper;
			maxVal = (double)lower;
		}
		else
		{
			minVal = (double)lower;
			maxVal = (double)upper;
		}

		SVConditionEnum  BinirizeType = bExclude ? SVECondOutRange : SVECondInRange;
		MatroxCode = SVMatroxImageInterface::Binarize(pOutputImage->getHandle()->GetBuffer(), pInputImage->getHandle()->GetBuffer(), BinirizeType, minVal, maxVal);

		Result = (S_OK == MatroxCode) ? true : false;
	}

	return Result;
}

bool SVColorThresholdClass::ValidateLocal() const
{
	bool Result(true);

	// Validate that we have inputs and outputs
	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		Result &= m_BandThreshold[Band].m_InputImage.IsConnected();
		Result &= nullptr != m_BandThreshold[Band].m_InputImage.GetInputObjectInfo().getObject();
		Result &= Result ? m_BandThreshold[Band].m_InputImage.GetInputObjectInfo().getObject()->IsCreated() : false;
		Result &= m_BandThreshold[Band].m_OutputImage.IsCreated();
		//Result &= m_BandThreshold[Band].m_OutputImage.IsCreated();
		if (!Result)
		{
			break;
		}
	}
	if (Result)
	{
		Result &= m_OutputImage.IsCreated();
	}
	return Result;
}

SvIe::SVImageClass* SVColorThresholdClass::GetBandInputImage(SvDef::BandEnum Band, bool bRunMode /*= false*/)
{
	return SvOl::getInput<SvIe::SVImageClass>(m_BandThreshold[Band].m_InputImage, bRunMode);
}
#pragma endregion Private Methods

} //namespace SvOp
