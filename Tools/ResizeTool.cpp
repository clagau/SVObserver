//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// Contains the function definitions for the Resize Tool class 
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ResizeTool.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVStatusLibrary/MessageContainer.h"

#pragma endregion Includes


namespace SvTo
{
	static bool isValidScaleFactorLV(SvVol::LinkedValue& scaleFactorValue);
	static void reportGeneralError(SvStl::MessageTextEnum AdditionalTextId, SvStl::MessageContainerVector* pErrorMessages, bool displayIfNotAddedToErrorList);


	constexpr long MinScaleFactorThreshold = 0; // Scale Factor may not 
												// be less than or equal 
												// to 0.
	constexpr long MaxScaleFactor = 1000;		// Maximum allowed Scale Factor. 
												// Arbitrary limit of 1000.


#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

SV_IMPLEMENT_CLASS(ResizeTool, SvPb::ResizeToolId);

constexpr double defaultScalefactor = 1.0;

ResizeTool::ResizeTool(SVObjectClass* pOwner, int stringResourceID)
	: SVToolClass(pOwner, stringResourceID)
{
	LocalInitialize();
}

SvVol::LinkedValue m_FilenameIndex1;

void ResizeTool::LocalInitialize()
{
	m_canResizeToParent = true;

	BuildInputObjectList();

	// The output image is referenced in the embedded list.
	m_OutputImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypePhysical);
	// This logical ROI image is also referenced in the embedded list, but 
	//  will be hidden from most exposure (within ResizeTool::Create).
	m_LogicalROIImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypeLogical);

	BuildEmbeddedObjectList();

	SvOi::NameValueVector interpolationEnumVector;

	std::copy(c_interpolationNamesAndModes.begin(), c_interpolationNamesAndModes.end(),
		std::back_inserter(interpolationEnumVector));

	m_ResizeInterpolationMode.SetEnumTypes(interpolationEnumVector);
	m_ResizeInterpolationMode.SetDefaultValue(InterpolationMode::Default, true);

	SvOi::NameValueVector overscanEnumVector;

	std::copy(c_overscanNamesAndModes.begin(), c_overscanNamesAndModes.end(),
		std::back_inserter(overscanEnumVector));

	m_ResizeOverscanMode.SetEnumTypes(overscanEnumVector);
	m_ResizeOverscanMode.SetDefaultValue(OverscanMode::OverscanEnable, true);

	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, true);

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVResizeToolObjectType;
}

void ResizeTool::BuildInputObjectList()
{
	// Source Image
	m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_InputImage, SvDef::SourceImageInputName, SvPb::ImageInputEId);
}

void ResizeTool::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(
		&m_SourceImageNames, 
		SvPb::SourceImageNamesEId, 
		IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, 
		false, 
		SvOi::SVResetItemTool);

	RegisterEmbeddedObject(
		&m_ResizeInterpolationMode, 
		SvPb::ResizeInterpolationModeEId, 
		IDS_OBJECTNAME_RESIZE_INTERPOLATIONMODE, 
		false, 
		SvOi::SVResetItemTool);

	RegisterEmbeddedObject(
		&m_ResizeOverscanMode,
		SvPb::ResizeOverscanEId, 
		IDS_OBJECTNAME_RESIZE_OVERSCAN, 
		false, 
		SvOi::SVResetItemTool);

	RegisterEmbeddedObject(
		&m_OutputImage,
		SvPb::OutputImageEId,
		IDS_OBJECTNAME_IMAGE1);

	RegisterEmbeddedObject(
		&m_LogicalROIImage,
		SvPb::LogicalROIImageEId,
		IDS_OBJECTNAME_ROIIMAGE);
}


ResizeTool::~ResizeTool(void)
{
	ResizeTool::CloseObject();
}


bool ResizeTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	// for the ResizeTool, we want to be able to see and store these scale factors
	m_ExtentWidthFactorContent.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeightFactorContent.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::AddAttribute);
	m_ExtentWidthFactorFormat.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::AddAttribute);
	m_ExtentHeightFactorFormat.SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::AddAttribute);

	// We do not want the ROI image showing up as an output image.
	m_LogicalROIImage.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationResize);

	SvIe::SVImageClass* pInputImage = getInputImage();
	if (nullptr != pInputImage)
	{
		m_LogicalROIImage.InitializeImage(pInputImage);
		m_OutputImage.InitializeImage(pInputImage);
	}

	bOk &= (nullptr != GetTool());

	bOk &= (nullptr != GetInspection());

	bOk &= (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true));

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	double widthScaleFactor = 0.0;
	m_ExtentWidthFactorFormat.getValue(widthScaleFactor);
	if (widthScaleFactor == 0.0)
	//m_ExtentWidthFactorFormat having a default value of 0.0 rather than SvDef::cDefaultScaleFactor
	// ensures proper behaviour with pre-10.10 configurations
	{
		std::string tmpStr;
		m_ExtentWidthFactorFormat.getLinkedName().getValue(tmpStr);
		if (tmpStr.empty()) //but a dotted name (even if currently having a value of zero) must not be overwritten with a numeric value!
		{
			m_ExtentWidthFactorContent.getValue(widthScaleFactor);
			m_ExtentWidthFactorFormat.setValue(widthScaleFactor);
		}
	}

	double heightScaleFactor = 0.0;
	m_ExtentHeightFactorFormat.getValue(heightScaleFactor);
	if (heightScaleFactor == 0.0)
	//m_ExtentHeightFactorFormat having a default value of 0.0 rather than SvDef::cDefaultScaleFactor
	// ensures proper behaviour with pre-10.10 configurations
	{
		std::string tmpStr;
		m_ExtentHeightFactorFormat.getLinkedName().getValue(tmpStr);
		if (tmpStr.empty()) //but a dotted name (even if currently having a value of zero) must not be overwritten with a numeric value!
		{
			m_ExtentHeightFactorContent.getValue(heightScaleFactor);
			m_ExtentHeightFactorFormat.setValue(heightScaleFactor);
		}
	}

	return bOk;
}


SvIe::SVImageClass* ResizeTool::getInputImage(bool bRunMode /*= false*/) const
{
	return m_InputImage.getInput<SvIe::SVImageClass>(bRunMode);
}

HRESULT ResizeTool::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT Result = m_toolExtent.UpdateExtentToParentExtents(NewExtent);

	if (S_OK == Result)
	{
		Result = SetImageExtent(NewExtent);
	}

	return Result;
}

HRESULT ResizeTool::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT hrOk = m_toolExtent.ValidExtentAgainstParentImage(rImageExtent);

	if (S_OK == hrOk)
	{
		hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}

	return hrOk;
}

SVToolClass* ResizeTool::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool ResizeTool::DoesObjectHaveExtents() const
{
	return true;
}

std::vector<std::string> ResizeTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 6> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Size and Position"),
		_T("Resize"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

SvVol::SVStringValueObjectClass* ResizeTool::GetInputImageNames()
{
	return &m_SourceImageNames;
}

bool ResizeTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)

	std::vector<std::reference_wrapper<const SvVol::LinkedValue>>
		allScaleFactorLinkedValues
	{
		std::cref(m_ExtentWidthFactorContent),
		std::cref(m_ExtentHeightFactorContent),
		std::cref(m_ExtentWidthFactorFormat),
		std::cref(m_ExtentHeightFactorFormat)
	};

	std::stringstream traceStream;

	traceStream << "Scale factors (wC/hC/wF/hF):";

	for (auto& rSflv : allScaleFactorLinkedValues)
	{
		auto& rSvo = rSflv.getLinkedName();

		std::string ln;
		rSvo.getValue(ln);
		traceStream << " '" << ln << "': ";
		double scaleFactor = 0.0;
		rSflv.getValue(scaleFactor);
		traceStream << scaleFactor;
	}
	traceStream << std::endl;

	OutputDebugString(traceStream.str().c_str());
	//uncomment the next line to enable error reporting to cmd window
	//SV_LOG_GLOBAL(info) << traceStream.str();
#endif

	if (!AreAllAllScaleFactorValuesValid())
	{
		reportGeneralError(SvStl::Tid_InvalidScaleFactor, pErrorMessages, true);

		return false;
	}

	if (!isInterpolationModeValueOK())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));

		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(Msg);
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Display);
			Exception.setMessage(Msg.getMessage());
		}
	}

	if (!isOverscanModeValueOK())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_5045_INVALIDOVERSCAN, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));

		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(Msg);
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Display);
			Exception.setMessage(Msg.getMessage());
		}
	}

	if (!ModifyImageExtentByScaleFactors())
	{
		reportGeneralError(SvStl::Tid_UpdateOutputImageExtentsFailed, pErrorMessages, true);

		return false;
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)
	auto extent = m_OutputImage.GetImageExtents();
#endif

	m_OutputImage.ResetObject(); // this ensures that the output image (that has already 
								 // been reset earlier) will have the right size as determined by the 
								 // extent as modified by ModifyImageExtentByScaleFactors() above.

#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)
	m_OutputImage.GetImageExtents().OutputDebugInformationOnExtent(" reset", &extent);
	m_OutputImage.GetImageExtents().OutputDebugInformationOnExtent(" reset");
	auto toolimageextent = m_toolExtent.getImageExtent();
	toolimageextent.OutputDebugInformationOnExtent("reset2");
#endif

	m_InputImage.validateInput();

	SvIe::SVImageClass* pInputImage = getInputImage();
	if (nullptr != pInputImage)
	{
		//Set input name to source image name to display it in result picker
		m_SourceImageNames.SetValue(pInputImage->GetCompleteName());
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5047_GETINPUTIMAGEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	// required within ResetObject in order to correctly reallocate
	// buffers when source image is changed within GUI.
	if (E_FAIL == m_LogicalROIImage.InitializeImage(pInputImage))
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5048_INITIALIZEROIIMAGEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	// required within ResetObject in order to correctly reallocate
	// buffers when source image is changed within GUI.
	if (E_FAIL == m_OutputImage.InitializeImage(pInputImage))
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5049_INITIALIZEOUTPUTIMAGEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	auto result = SVToolClass::ResetObject(pErrorMessages);

	return result;
}

bool ResizeTool::ModifyImageExtentByScaleFactors()
{
#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)
	auto before = m_toolExtent.getImageExtent();
#endif

	double contentWidthScaleFactor = 0.0;
	HRESULT hr = m_ExtentWidthFactorContent.getValue(contentWidthScaleFactor); //this scale factor influences only the image's content, not its size.
	bool ok = (S_OK == hr);
	double contentHeightScaleFactor = 0.0;
	ok = ok && (S_OK == m_ExtentHeightFactorContent.getValue(contentHeightScaleFactor)); //this scale factor influences only the image's content, not its size.

	double formatWidthScaleFactor = 0.0;
	ok = ok && (S_OK == m_ExtentWidthFactorFormat.getValue(formatWidthScaleFactor)); //this scale factor influences only the image's size, not its content.
	double formatHeightScaleFactor = 0.0;
	ok = ok && (S_OK == m_ExtentHeightFactorFormat.getValue(formatHeightScaleFactor)); //this scale factor influences only the image's size, not its content.

	ok = ok && (S_OK == m_toolExtent.getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyWidthFactorContent, contentWidthScaleFactor));
	ok = ok && (S_OK == m_toolExtent.getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyHeightFactorContent, contentHeightScaleFactor));

	ok = ok && (S_OK == m_toolExtent.getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyWidthFactorFormat, formatWidthScaleFactor));
	ok = ok && (S_OK == m_toolExtent.getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyHeightFactorFormat, formatHeightScaleFactor));

	double imageWidthFactor = 0.0;
	ok =  ok &&(S_OK == m_ExtentWidthFactorFormat.getValue(imageWidthFactor)); //this scale factor influences only image size, not enlargement
	double imageHeightFactor = 0.0;
	ok =  ok &&(S_OK == m_ExtentHeightFactorFormat.getValue(imageHeightFactor)); //this scale factor influences only image size, not enlargement

	double currentImageWidth = 0.0;
	ok =  ok &&(S_OK == m_toolExtent.getImageExtent().GetExtentProperty(SvPb::SVExtentPropertyWidth, currentImageWidth));

	double currentImageHeight = 0.0;
	ok =  ok &&(S_OK == m_toolExtent.getImageExtent().GetExtentProperty(SvPb::SVExtentPropertyHeight, currentImageHeight));

	double outputImageWidth = currentImageWidth * imageWidthFactor;
	double outputImageHeight = currentImageHeight * imageHeightFactor;

	ok =  ok &&(S_OK == m_toolExtent.getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyOutputWidth, outputImageWidth));
	ok =  ok &&(S_OK == m_toolExtent.getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyOutputHeight, outputImageHeight));

#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)
	m_toolExtent.getImageExtent().OutputDebugInformationOnExtent(" modify", &before);
#endif

	return ok;
}

bool ResizeTool::AreAllAllScaleFactorValuesValid()
{
	return isValidScaleFactorLV(m_ExtentWidthFactorContent)
		&& isValidScaleFactorLV(m_ExtentHeightFactorContent)
		&& isValidScaleFactorLV(m_ExtentWidthFactorFormat)
		&& isValidScaleFactorLV(m_ExtentHeightFactorFormat);
}

bool ResizeTool::isInterpolationModeValueOK()
{
	long interpolationMode(0L);
	m_ResizeInterpolationMode.GetValue(interpolationMode);

	return std::any_of(c_interpolationNamesAndModes.begin(), c_interpolationNamesAndModes.end(),
		[interpolationMode](const auto& nameAndMode) {return nameAndMode.second == interpolationMode; });
}

bool ResizeTool::isOverscanModeValueOK()
{
	long overscanMode(0L);
	m_ResizeOverscanMode.GetValue(overscanMode);

	return std::any_of(c_overscanNamesAndModes.begin(), c_overscanNamesAndModes.end(),
		[overscanMode](const auto& nameAndMode) {return nameAndMode.second == overscanMode; });
}

bool ResizeTool::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	const SvIe::SVImageClass* pImage = getInputImage();
	if (nullptr != pImage && imageId == pImage->getObjectId())
	{
		Result = true;
	}

	return Result;
}

bool ResizeTool::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	//@TODO [Arvid][10.10][27.11.2020] the message management in this tool in general 
	//and in this function in particular is somewhat strange and should be redone!

	bool Result = __super::onRun(rRunStatus, pErrorMessages);

#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)
	m_OutputImage.GetImageExtents().OutputDebugInformationOnExtent("on run");
	auto toolimageextent = m_toolExtent.getImageExtent();
	m_OutputImage.GetImageExtents().OutputDebugInformationOnExtent("on run", &toolimageextent);
#endif

	double contentHeightScaleFactor = 0.0;
	m_ExtentHeightFactorContent.getValue(contentHeightScaleFactor); //this scale factor influences only enlargement, not image size.
	if (false == isValidScaleFactor(contentHeightScaleFactor))
	{
		Result = false;
		reportGeneralError(SvStl::Tid_InvalidScaleFactor, pErrorMessages, false);
	}

	double contentWidthScaleFactor = 0.0;
	m_ExtentWidthFactorContent.getValue(contentWidthScaleFactor); //this scale factor influences only enlargement, not image size.

	if (false == isValidScaleFactor(contentWidthScaleFactor))
	{
		Result = false;
		reportGeneralError(SvStl::Tid_InvalidScaleFactor, pErrorMessages, false);
	}

	long interpolationMode = 0;
	if (!SUCCEEDED(m_ResizeInterpolationMode.GetValue(interpolationMode)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	long overscanMode = 0;
	if (!SUCCEEDED(m_ResizeOverscanMode.GetValue(overscanMode)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5033_GETOVERSCANFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	SvOi::ITRCImagePtr pRoiImageBuffer = m_LogicalROIImage.getImageReadOnly(rRunStatus.m_triggerRecord.get());
	if (nullptr == pRoiImageBuffer || pRoiImageBuffer->isEmpty())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	SvOi::ITRCImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
	if (nullptr == pOutputImageBuffer || pOutputImageBuffer->isEmpty())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	if (Result)
	{
		// m_ExtentWidthFactorFormat and m_ExtentHeightFactorFormat are used only to determine whether the resize result is smaller
		// than the output buffer in either dimension and hence the output buffer must be cleared
		// they are not needed for the resize operation itself and therefore are not checked for validity

		double formatHeightScaleFactor = 0.0;
		m_ExtentHeightFactorFormat.getValue(formatHeightScaleFactor);

		double formatWidthScaleFactor = 0.0;
		m_ExtentWidthFactorFormat.getValue(formatWidthScaleFactor);

		if ((contentHeightScaleFactor < formatHeightScaleFactor) || (contentWidthScaleFactor < formatWidthScaleFactor))
		{
			MbufClear(pOutputImageBuffer->getHandle()->GetBuffer().GetIdentifier(), 0);
		}

		HRESULT hr = SVMatroxImageInterface::Resize(pOutputImageBuffer->getHandle()->GetBuffer(),
			pRoiImageBuffer->getHandle()->GetBuffer(),
			contentWidthScaleFactor,
			contentHeightScaleFactor,
			interpolationMode,
			overscanMode);

		if (!SUCCEEDED(hr))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(hr, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (!Result)
	{
		rRunStatus.SetInvalid();
	}

	return Result;
}


void reportGeneralError(SvStl::MessageTextEnum AdditionalTextId, SvStl::MessageContainerVector* pErrorMessages, bool displayIfNotAddedToErrorList ) //@TODO [Arvid][10.10][25.11.2020] könnte allgemeiner verwendet werden!
{
	SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, AdditionalTextId, SvStl::SourceFileParams(StdMessageParams));
	if (nullptr != pErrorMessages)
	{
		pErrorMessages->push_back(Msg);
		return;
	}

	if (displayIfNotAddedToErrorList)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Display);
		Exception.setMessage(Msg.getMessage());
	}
}


bool isValidScaleFactorLV(SvVol::LinkedValue& scaleFactorValue)
{
	double scaleFactor = 0.0;
	scaleFactorValue.getValue(scaleFactor);
	return isValidScaleFactor(scaleFactor);
}


bool isValidScaleFactor(double value)
{
	if ((value <= MinScaleFactorThreshold) || (value > MaxScaleFactor))
	{
		return false;
	}

	return true;
}

} //namespace SvTo



