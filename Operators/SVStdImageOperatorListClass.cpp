//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for the StdImageOperator List
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVStdImageOperatorListClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/ITool.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "TriggerRecordController/ITriggerRecordControllerRW.h"
#pragma endregion Includes

namespace SvOp
{
#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVStdImageOperatorListClass, SvPb::StdImageOperatorListClassId)

SVStdImageOperatorListClass::SVStdImageOperatorListClass(SVObjectClass* POwner, int StringResourceID)
	:SVUnaryImageOperatorList(POwner, StringResourceID)
{
	init();
}

void SVStdImageOperatorListClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVStdImageOperatorListObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1);
	RegisterEmbeddedObject(&m_LogicalROIImage, SvPb::LogicalROIImageEId, IDS_OBJECTNAME_ROIIMAGE);

	// This logical ROI image is referenced in the embedded list, but 
	//  will be hidden from most exposure.
	m_LogicalROIImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypeLogical);
	// Set Embedded defaults
	m_OutputImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypePhysical);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVStdImageOperatorListClass::~SVStdImageOperatorListClass()
{
	SVStdImageOperatorListClass::CloseObject();
}

bool SVStdImageOperatorListClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	// Image input must already exist, and must be created!!!

	// Embedded Image output must already exist!!!
	SvIe::SVImageClass* pInputImage = getInputImage();
	m_isCreated = __super::CreateObject(rCreateStructure) && (nullptr != pInputImage);
	m_isCreated &= (S_OK == m_OutputImage.InitializeImage(pInputImage)) && (S_OK == m_LogicalROIImage.InitializeImage(pInputImage));

	// We do not want the ROI image showing up as an output image.
	m_LogicalROIImage.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	return m_isCreated;
}

bool SVStdImageOperatorListClass::CloseObject()
{
	bool bRetVal = m_LogicalROIImage.CloseObject();
	bRetVal = m_OutputImage.CloseObject() && bRetVal;

	if (m_isCreated)
	{
		m_isCreated = false;

		bRetVal = __super::CloseObject() && bRetVal;
	}

	return bRetVal;
}

bool SVStdImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	auto* pInputImage = getInputImage();
	if ((S_OK != m_OutputImage.InitializeImage(pInputImage)) || (S_OK != m_LogicalROIImage.InitializeImage(pInputImage)))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	CollectInputImageNames();

	SVImageInfoClass imageInfo = m_OutputImage.GetImageInfo();
	SvIe::SVImageProcessingClass::CreateImageBuffer(imageInfo, m_milTmpImageObjectInfo1);
	SvIe::SVImageProcessingClass::CreateImageBuffer(imageInfo, m_milTmpImageObjectInfo2);

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Special routing here.
////////////////////////////////////////////////////////////////////////////////
bool SVStdImageOperatorListClass::Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	clearRunErrorMessages();

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	SvIe::SVImageClass* pInputImage = getInputImage(true);

	if (nullptr == pInputImage)
	{
		bRetVal = false;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		m_RunErrorMessages.push_back(Msg);
	}

	if (bRetVal)
	{
		SvTrc::IImagePtr pInputImageBuffer = m_LogicalROIImage.getImageReadOnly(rRunStatus.m_triggerRecord.get());
		bRetVal = RunLocal(rRunStatus, pInputImageBuffer, m_OutputImage);
	}

	if (!bRetVal)
	{
		// Something was wrong...
		rRunStatus.SetInvalid();
	}

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

// Set String value object for Source Image Names
HRESULT SVStdImageOperatorListClass::CollectInputImageNames()
{
	HRESULT hr = S_FALSE;
	SvIe::SVImageClass* pInputImage = getInputImage();
	SvOi::ITool* pTool = GetToolInterface();
	if (nullptr != pInputImage && nullptr != pTool)
	{
		std::string Name = pInputImage->GetCompleteName();

		if (pTool->SetFirstInputImageName(Name.c_str()))
		{
			hr = S_OK;
		}
	}
	return hr;
}

bool SVStdImageOperatorListClass::RunLocal(SVRunStatusClass &rRunStatus, SvTrc::IImagePtr pInputImageBuffer, SvIe::SVImageClass&)
{
	bool bRetVal = true;
	SvTrc::IImagePtr pOutputImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
	if (nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
	{
		SvOi::SVImageBufferHandlePtr input = (nullptr != pInputImageBuffer) ? pInputImageBuffer->getHandle() : nullptr;
		SvOi::SVImageBufferHandlePtr output = pOutputImageBuffer->getHandle();

		if (nullptr == input || nullptr == output)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			m_RunErrorMessages.push_back(Msg);
			SvIe::SVImageProcessingClass::InitBuffer(output);
			return false;
		}

		//set tmp variable
		SvOi::SVImageBufferHandlePtr sourceImage = m_milTmpImageObjectInfo1;
		SvOi::SVImageBufferHandlePtr destinationImage = m_milTmpImageObjectInfo2;
		if (sourceImage->empty() || destinationImage->empty() || !copyBuffer(input, sourceImage))
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			m_RunErrorMessages.push_back(Msg);
			return false;
		}

		SVRunStatusClass ChildRunStatus;
		ChildRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
		ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;
		// Run children...
		for (int i = 0; i < GetSize(); i++)
		{
			ChildRunStatus.ResetRunStateAndToolSetTimes();

			SVUnaryImageOperatorClass*  pOperator = dynamic_cast<SVUnaryImageOperatorClass*>(GetAt(i));
			if (nullptr != pOperator)
			{
				if (pOperator->Run(true, sourceImage, destinationImage, ChildRunStatus))
				{
					//switch image buffer for next run
					std::swap(sourceImage, destinationImage);
				}
				// NOTE:
				// If operator returns FALSE, he was not running ( may be deactivated )
				// So, don't switch first flag off, so that a following operator knows
				// he is the first one or the 'no operator was running' check can do his job!!!
				// RO_22Mar2000

				// WARNING:
				// Do not set bRetVal automatically to FALSE, if operator was not running !!!
				// ChildRunStatus keeps information about, if an error occurred while running !!!
			}
			else
			{
				bRetVal = false;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				m_RunErrorMessages.push_back(Msg);
			}

			// Update our Run Status
			if (ChildRunStatus.IsDisabled())
			{
				rRunStatus.SetDisabled();
			}

			if (ChildRunStatus.IsDisabledByCondition())
			{
				rRunStatus.SetDisabledByCondition();
			}

			if (ChildRunStatus.IsWarned())
			{
				rRunStatus.SetWarned();
			}

			if (ChildRunStatus.IsFailed())
			{
				rRunStatus.SetFailed();
			}

			if (ChildRunStatus.IsPassed())
			{
				rRunStatus.SetPassed();
			}

			if (ChildRunStatus.IsCriticalFailure())
			{
				rRunStatus.SetCriticalFailure();
			}
		}// for( int i = 0; i < GetSize(); i++ )
		rRunStatus.m_triggerRecord = std::move(ChildRunStatus.m_triggerRecord);

		// 'no operator was running' check...
		// RO_22Mar2000

		if (bRetVal)
		{
			if (!copyBuffer(sourceImage, output))
			{
				bRetVal = false;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				m_RunErrorMessages.push_back(Msg);
			}
		} // if( bFirstFlag ) 
	}  // if ( get output image buffer )
	else
	{
		bRetVal = false;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetImageBufferToWriteFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		m_RunErrorMessages.push_back(Msg);
	}	
	return bRetVal;
}

bool SVStdImageOperatorListClass::copyBuffer(const SvOi::SVImageBufferHandlePtr input, SvOi::SVImageBufferHandlePtr output)
{
	bool bRetVal = true;

	bRetVal = bRetVal && !(input->empty());
	bRetVal = bRetVal && !(output->empty());

	if (bRetVal && input != output)
	{
		HRESULT  Code = SVMatroxBufferInterface::CopyBuffer(output->GetBuffer(), input->GetBuffer());
		bRetVal = (S_OK == Code);
	}

	return bRetVal;
}

} //namespace SvOp
