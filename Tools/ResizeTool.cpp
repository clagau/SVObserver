//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ResizeTool.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const long ResizeTool::MinScaleFactorThreshold = 0; // Scale Factor may not be less than or equal to 0.
const long ResizeTool::MaxScaleFactor = 1000;       // Maximum allowed Scale Factor. 
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(ResizeTool, SvPb::ResizeToolId);



ResizeTool::ResizeTool(SVObjectClass* pOwner, int stringResourceID)
	: SVToolClass(pOwner, stringResourceID)
{
	LocalInitialize();
}

void ResizeTool::LocalInitialize()
{
	m_canResizeToParent = true;
	//  Start of initialize class members.
	InitializeInterpolationModeMember();
	InitializeOverscanMember();
	InitializePerformanceMember();
	//  End of initialize class members.

	BuildInputObjectList();

	// The output image is referenced in the embedded list.
	m_OutputImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);
	// This logical ROI image is also referenced in the embedded list, but 
	//  will be hidden from most exposure (within ResizeTool::Create).
	m_LogicalROIImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeLogical);

	BuildEmbeddedObjectList();

	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, true);

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResizeToolObjectType;
}

HRESULT	ResizeTool::InitializeInterpolationModeMember()
{
	HRESULT	hr = S_OK;

	SvOi::NameValueVector interpolationModeOptions
	{
		{SvDef::Auto, SVInterpolationModeOptions::InterpolationModeAuto},
		{SvDef::Bicubic, SVInterpolationModeOptions::InterpolationModeBicubic},
		{SvDef::Bilinear, SVInterpolationModeOptions::InterpolationModeBilinear},
		{SvDef::NearestNeighbor, SVInterpolationModeOptions::InterpolationModeNearestNeighbor}
	};

	m_ResizeInterpolationMode.SetEnumTypes(interpolationModeOptions);
	m_ResizeInterpolationMode.SetDefaultValue(SVInterpolationModeOptions::InterpolationModeAuto, true);

	return hr;
}

HRESULT	ResizeTool::InitializeOverscanMember()
{
	HRESULT	hr = S_OK;

	SvOi::NameValueVector EnumVector
	{
		{SvDef::Enabled, SVOverscanOptions::OverscanEnable},
		{SvDef::Disabled, SVOverscanOptions::OverscanDisable},
	};

	m_ResizeOverscan.SetEnumTypes(EnumVector);
	m_ResizeOverscan.SetDefaultValue(SVOverscanOptions::OverscanEnable, true);

	return hr;
}

HRESULT	ResizeTool::InitializePerformanceMember()
{
	HRESULT	hr = S_OK;

	SvOi::NameValueVector performanceOptions;
	performanceOptions.push_back(SvOi::NameValuePair {SvDef::Fast, SVPerformanceOptions::PerformanceFast});
	performanceOptions.push_back(SvOi::NameValuePair {SvDef::Precise, SVPerformanceOptions::PerformancePresice});

	m_ResizePerformance.SetEnumTypes(performanceOptions);
	m_ResizePerformance.SetDefaultValue(SVPerformanceOptions::PerformancePresice, TRUE);

	return hr;
}

void ResizeTool::BuildInputObjectList()
{
	// Source Image
	m_InputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_InputImageObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_InputImageObjectInfo, SvDef::ResizeImage);

	addDefaultInputObjects();
}

void ResizeTool::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ResizeInterpolationMode, SvPb::ResizeInterpolationModeEId, IDS_OBJECTNAME_RESIZE_INTERPOLATIONMODE, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ResizeOverscan, SvPb::ResizeOverscanEId, IDS_OBJECTNAME_RESIZE_OVERSCAN, false, SvOi::SVResetItemTool);
	RegisterEmbeddedObject(&m_ResizePerformance, SvPb::ResizePerformanceEId, IDS_OBJECTNAME_RESIZE_PERFORMANCE, false, SvOi::SVResetItemTool);

	RegisterEmbeddedObject(&m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1);
	RegisterEmbeddedObject(&m_LogicalROIImage, SvPb::LogicalROIImageEId, IDS_OBJECTNAME_ROIIMAGE);
}


ResizeTool::~ResizeTool(void)
{
	ResizeTool::CloseObject();
}


bool ResizeTool::CloseObject()
{
	bool bRetVal = true;

	if (m_isCreated)
	{
		//		This statement should not be necessary because it should be set within 
		//		the base class.  
		//		isCreated = FALSE;

		bRetVal = m_LogicalROIImage.CloseObject();

		bRetVal = m_OutputImage.CloseObject() && bRetVal;

		// This should end up setting isCreated to FALSE within 
		// SVObjectClass::CloseObject().
		bRetVal = SVToolClass::CloseObject() && bRetVal;
	}

	return bRetVal;
}


bool ResizeTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	// Override base class hiding of Scale Factors.  These values will be 
	// exposed for the Resize Tool.
	// Since this value object is already 
	// exposed as an extent, we do not want 
	// it to be embeddable.
	const UINT cAttributes = (SvDef::defaultValueObjectAttributes | SvPb::remotelySetable | SvPb::extentObject | SvPb::setableOnline) & ~SvPb::embedable;

	m_ExtentWidthScaleFactor.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	m_ExtentHeightScaleFactor.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	SvIe::SVImageClass* pInputImage = getInputImage();
	bOk &= (nullptr != pInputImage);

	bOk &= (S_OK == m_LogicalROIImage.InitializeImage(pInputImage));

	// We do not want the ROI image showing up as an output image.
	m_LogicalROIImage.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationResize);

	bOk &= (S_OK == m_OutputImage.InitializeImage(pInputImage));

	bOk &= (nullptr != GetTool());

	bOk &= (nullptr != GetInspection());

	bOk &= (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true));

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	return bOk;
}

SvIe::SVImageClass* ResizeTool::getInputImage(bool bRunMode /*= false*/) const
{
	return SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo, bRunMode);
}

HRESULT ResizeTool::SetImageExtentToParent()
{
	SVImageExtentClass l_NewExtent;
	HRESULT Result = m_toolExtent.UpdateExtentToParentExtents(l_NewExtent);

	if (S_OK == Result)
	{
		//@WARNING [Jim][8 July 2015] - Research this.  All tools are calling 
		// base class which does not validate.  Should probably call the 
		// derived tool classes explicit version.
		Result = SVToolClass::SetImageExtent(l_NewExtent);
	}
	return Result;
}

HRESULT ResizeTool::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = m_toolExtent.ValidExtentAgainstParentImage(rImageExtent);

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}

	return l_hrOk;
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

SvVol::SVStringValueObjectClass* ResizeTool::GetInputImageNames()
{
	return &m_SourceImageNames;
}

bool ResizeTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = ValidateParameters(pErrorMessages) && ValidateOfflineParameters(pErrorMessages);

	SvOl::ValidateInput(m_InputImageObjectInfo);

	SvIe::SVImageClass* pInputImage = getInputImage();
	if (Result)
	{
		if (nullptr != pInputImage)
		{
			//Set input name to source image name to display it in result picker
			m_SourceImageNames.SetValue(pInputImage->GetCompleteName());
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5047_GETINPUTIMAGEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (Result)
	{
		// required within ResetObject in order to correctly reallocate
		// buffers when source image is changed within GUI.
		if (E_FAIL == m_LogicalROIImage.InitializeImage(pInputImage))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5048_INITIALIZEROIIMAGEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (Result)
	{
		// required within ResetObject in order to correctly reallocate
		// buffers when source image is changed within GUI.
		if (E_FAIL == m_OutputImage.InitializeImage(pInputImage))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5049_INITIALIZEOUTPUTIMAGEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	Result = Result && SVToolClass::ResetObject(pErrorMessages);

	return Result;
}

bool ResizeTool::ValidateParameters(SvStl::MessageContainerVector *pErrorMessages)
{
	double newWidthScaleFactor = 0.0;
	GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyWidthScaleFactor, newWidthScaleFactor);
	bool Result = ValidateScaleFactor(newWidthScaleFactor, pErrorMessages);

	double newHeightScaleFactor = 0.0;
	GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyHeightScaleFactor, newHeightScaleFactor);
	Result = ValidateScaleFactor(newHeightScaleFactor, pErrorMessages) && Result;

	return Result;
}

bool ResizeTool::ValidateOfflineParameters(SvStl::MessageContainerVector *pErrorMessages)
{
	long Value(0L);
	m_ResizeInterpolationMode.GetValue(Value);
	SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationValue;
	interpolationValue = static_cast<SVInterpolationModeOptions::SVInterpolationModeOptionsEnum> (Value);
	bool Result = ValidateInterpolation(interpolationValue, pErrorMessages);

	m_ResizeOverscan.GetValue(Value);
	SVOverscanOptions::SVOverscanOptionsEnum overscanValue;
	overscanValue = static_cast<SVOverscanOptions::SVOverscanOptionsEnum> (Value);
	Result = ValidateOverscan(overscanValue) && Result;

	m_ResizePerformance.GetValue(Value);
	SVPerformanceOptions::SVPerformanceOptionsEnum performanceValue;
	performanceValue = static_cast<SVPerformanceOptions::SVPerformanceOptionsEnum> (Value);
	Result = ValidatePerformance(performanceValue) && Result;

	return Result;
}

bool	ResizeTool::ValidateInterpolation(SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	switch (interpolationMode)
	{
		case	SVInterpolationModeOptions::InterpolationModeAuto:
		case	SVInterpolationModeOptions::InterpolationModeBilinear:
		case	SVInterpolationModeOptions::InterpolationModeBicubic:
			// Average will only work with values less than 1.  Auto will 
			// already use Average for values less than 1.
		case	SVInterpolationModeOptions::InterpolationModeNearestNeighbor:
		{
			break;
		}
		default:
		{
			// invalid parameter
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

bool	ResizeTool::ValidateOverscan(const SVOverscanOptions::SVOverscanOptionsEnum overscan, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	switch (overscan)
	{
		case	SVOverscanOptions::OverscanEnable:
		case	SVOverscanOptions::OverscanDisable:
		{
			break;
		}
		default:
		{
			// invalid parameter
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5045_INVALIDOVERSCAN, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

bool	ResizeTool::ValidatePerformance(const SVPerformanceOptions::SVPerformanceOptionsEnum performance, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	switch (performance)
	{
		case	SVPerformanceOptions::PerformanceFast:
		case	SVPerformanceOptions::PerformancePresice:
		{
			break;
		}
		default:
		{
			// invalid parameter
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_5046_INVALIDPERFORMANCE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	return Result;
}

bool ResizeTool::ValidateScaleFactor(const double value, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	// Arbitrary high end scale factor limit of 1000.
	if ((value <= MinScaleFactorThreshold) ||
		(value > MaxScaleFactor))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), value));
			SvStl::MessageContainer Msg(SVMSG_SVO_5061_SFOUTSIDERANGE, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

void ResizeTool::BackupInspectionParameters()
{
	GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyHeightScaleFactor, m_ResizeHeightSF_Backup);
	GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyWidthScaleFactor, m_ResizeWidthSF_Backup);
	m_ResizeInterpolationMode.GetValue(m_ResizeInterpolationMode_Backup);
	m_ResizeOverscan.GetValue(m_ResizeOverscan_Backup);
	m_ResizePerformance.GetValue(m_ResizePerformance_Backup);
}

HRESULT	ResizeTool::GetBackupInspectionParameters(double*	oldHeightScaleFactor,
	double*	oldWidthScaleFactor,
	long*	oldInterpolationMode,
	long*	oldOverscan,
	long*	oldPerformance)
{
	HRESULT hr = S_OK;

	*oldHeightScaleFactor = m_ResizeHeightSF_Backup;
	*oldWidthScaleFactor = m_ResizeWidthSF_Backup;
	*oldInterpolationMode = m_ResizeInterpolationMode_Backup;
	*oldOverscan = m_ResizeOverscan_Backup;
	*oldPerformance = m_ResizePerformance_Backup;

	return hr;
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

bool ResizeTool::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::onRun(rRunStatus, pErrorMessages);

	double heightScaleFactor = 0.0;
	if (!SUCCEEDED(GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyHeightScaleFactor, heightScaleFactor)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5030_GETHEIGHTSFFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	double widthScaleFactor = 0.0;
	if (!SUCCEEDED(GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyWidthScaleFactor, widthScaleFactor)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5031_GETWIDTHSFFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
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

	long overscan = 0;
	if (!SUCCEEDED(m_ResizeOverscan.GetValue(overscan)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5033_GETOVERSCANFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	long performance = 0;
	if (!SUCCEEDED(m_ResizePerformance.GetValue(performance)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5034_GETPERFORMANCEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	SvTrc::IImagePtr pRoiImageBuffer = m_LogicalROIImage.getImageReadOnly(rRunStatus.m_triggerRecord.get());
	if (nullptr == pRoiImageBuffer || pRoiImageBuffer->isEmpty())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	SvTrc::IImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
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
		HRESULT hr = SVMatroxImageInterface::Resize(pOutputImageBuffer->getHandle()->GetBuffer(),
			pRoiImageBuffer->getHandle()->GetBuffer(),
			widthScaleFactor,
			heightScaleFactor,
			static_cast <SVInterpolationModeOptions::SVInterpolationModeOptionsEnum>(interpolationMode),
			static_cast <SVOverscanOptions::SVOverscanOptionsEnum> (overscan),
			static_cast <SVPerformanceOptions::SVPerformanceOptionsEnum> (performance));
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

} //namespace SvTo
