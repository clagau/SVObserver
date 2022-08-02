//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageArithmetic
//* .File Name       : $Workfile:   SVImageArithmetic.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:44:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageArithmetic.h"
#include "Definitions/ImageOperatorEnums.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "InspectionEngine/SVImageClass.h"
#include "Tools/SVTool.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "InspectionEngine/RunStatus.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVValueObjectLibrary/SVValueObjectClass.h"
#if defined(TRACE_IMAGE_SIZE)
#include "SVUtilityLibrary/StringHelper.h"
#endif
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVImageArithmetic, SvPb::ImageArithmeticClassId)

SVImageArithmetic::SVImageArithmetic(SVObjectClass* POwner, int StringResourceID)
	:SVTaskObjectClass(POwner, StringResourceID)
{
	// Identify yourself
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVImageArithmeticObjectType;

	// Identify our input type needs...

	// Input A...
	// Image
	m_InputImageA.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_InputImageA, _T("ImageArithmeticAImage"), SvPb::ImageArithmeticAImageInputEId);

	// Input B...
	// Image
	m_InputImageB.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_InputImageB, _T("ImageArithmeticBImage"), SvPb::ImageArithmeticBImageInputEId);

	// Operator Input...
	m_InputArithmaticOperator.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVLongValueObjectType, SvPb::ArithmeticOperatorEId);
	registerInputObject(&m_InputArithmaticOperator, _T("ImageArithmeticOperator"), SvPb::ImageArithmeticOperatorInputEId);
	m_InputArithmaticOperator.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// offset and gain input 
	m_InputIsGainOffsetEnabled.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::ImageToolEnabledGainId);
	registerInputObject(&m_InputIsGainOffsetEnabled, _T("ImageArithmeticGainEnabled"), SvPb::ImageArithmeticGainEnabledEId);

	m_InputOffset.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVVariantValueObjectType, SvPb::ImageToolOffsetId);
	registerInputObject(&m_InputOffset, _T("ImageArithmeticOffset"), SvPb::ImageArithmeticOffsetEId);

	m_InputGain.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVVariantValueObjectType, SvPb::ImageToolGainId);
	registerInputObject(&m_InputGain, _T("ImageArithmeticGain"), SvPb::ImageArithmeticGainEId);

	m_InputUseLut.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::ImageToolRecalculateOnceId);
	registerInputObject(&m_InputUseLut, _T("ImageArithmeticUseLut"), SvPb::ImageArithmeticUseLutEId);

	// Register Embedded Objects
	RegisterEmbeddedImage(&m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1);
	m_OutputImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypePhysical);
}

SVImageArithmetic::~SVImageArithmetic()
{
	SVImageArithmetic::CloseObject();
	m_milFloatImageBuffer.clear();
}

bool SVImageArithmetic::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	bOk &= (S_OK == m_OutputImage.InitializeImage(m_InputImageA.getInput<SvIe::SVImageClass>()));

	// Reset Printable flag
	m_OutputImage.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool SVImageArithmetic::CloseObject()
{
	if (SVTaskObjectClass::CloseObject() && getOutputImage())
	{
		return getOutputImage()->CloseObject();
	}
	return false;
}

bool SVImageArithmetic::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result {true};

	m_InputImageA.validateInput();
	m_InputImageB.validateInput();
	m_InputArithmaticOperator.validateInput();
	m_InputGain.validateInput();;
	m_InputOffset.validateInput();;
	m_InputIsGainOffsetEnabled.validateInput();;

	HRESULT l_hrOk = m_OutputImage.InitializeImage(m_InputImageA.getInput<SvIe::SVImageClass>());
	if (S_OK != l_hrOk)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	CollectInputImageNames();

	Result = SVTaskObjectClass::ResetObject(pErrorMessages) && Result && ValidateLocal(pErrorMessages);
	m_milFloatImageBuffer.clear();

	BOOL enableGain {FALSE};
	if (getEnableGainAndOffset(enableGain, pErrorMessages) == false)
	{
		return false;
	}
	BOOL useLut {FALSE};
	if (getEnableLut(useLut, pErrorMessages) == false)
	{
		return false;
	}
	
	if (enableGain && Result)
	{

		auto info = m_OutputImage.GetImageInfo();
		SVMatroxBufferCreateStruct  mbCstruct;
		mbCstruct.m_eType = SV32BitFloat;
		mbCstruct.m_eAttribute = (SVMatroxBufferAttributeEnum)(SVBufAttImage | SVBufAttProc);
		mbCstruct.m_lSizeBand = info.getBandNumber();
		mbCstruct.m_lSizeX = info.getOutputWidth();
		mbCstruct.m_lSizeY = info.getOutputHeight();
		HRESULT res = SVMatroxBufferInterface::Create(m_milFloatImageBuffer, mbCstruct);
		if (res != S_OK)
		{
			Result = false;
		}
	}
	if (enableGain && useLut && Result)
	{
		auto info = m_OutputImage.GetImageInfo();
		assert(info.getPixelDepth() == SV8BitUnsigned);
		if (!m_pLookUp)
		{
			m_pLookUp = std::make_unique<LookUp>(256);
		}
		double gain, offset;
		if (getGainAndOffset(gain, offset, pErrorMessages) == false)
		{
			return false;
		}
		long op;
		if (getOperator(op, pErrorMessages) == false)
		{
			return false;
		}
		m_pLookUp->Init(gain, offset, op);

	}
	return Result;
}

bool SVImageArithmetic::isInputImage(uint32_t imageId) const
{
	bool Result(false);
	const SvIe::SVImageClass* pImageA = m_InputImageA.getInput<SvIe::SVImageClass>();
	const SvIe::SVImageClass* pImageB = m_InputImageB.getInput<SvIe::SVImageClass>();

	if ((nullptr != pImageA && imageId == pImageA->getObjectId()) || (nullptr != pImageB && imageId == pImageB->getObjectId()))
	{
		Result = true;
	}
	return Result;
}

bool SVImageArithmetic::getEnableLut(BOOL& enableLut, SvStl::MessageContainerVector* pErrorMessages)
{

	SvVol::SVBoolValueObjectClass* pUseLut = m_InputUseLut.getInput<SvVol::SVBoolValueObjectClass>(true);

	if (!pUseLut || S_OK != pUseLut->GetValue(enableLut))
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}

bool SVImageArithmetic::getEnableGainAndOffset(BOOL& enableGain, SvStl::MessageContainerVector* pErrorMessages)
{

	SvVol::SVBoolValueObjectClass* pEnableGain = m_InputIsGainOffsetEnabled.getInput<SvVol::SVBoolValueObjectClass>(true);

	if (!pEnableGain || S_OK != pEnableGain->GetValue(enableGain))
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}

bool SVImageArithmetic::getGainAndOffset(double& gain, double& offset, SvStl::MessageContainerVector* pErrorMessages)
{
	gain = 1.0;
	offset = 0.0;
	variant_t temp, temp2;
	//getInputObject instead of getInput to get Linked Value
	SvVol::LinkedValue* pGain = m_InputGain.getInputObject<SvVol::LinkedValue>(true);
	SvVol::LinkedValue* pOffset = m_InputOffset.getInputObject<SvVol::LinkedValue>(true);
	if (!pGain || !pOffset || S_OK != pGain->GetValue(temp) || S_OK != pOffset->GetValue(temp2))
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	else
	{
		gain = temp;
		offset = temp2;
		return true;
	}
}

bool SVImageArithmetic::getOperator(long& Operator, SvStl::MessageContainerVector* pErrorMessages)
{
	Operator = 0;
	SvVol::SVLongValueObjectClass* pArithOperator = m_InputArithmaticOperator.getInput<SvVol::SVLongValueObjectClass>(true);
	if (S_OK != pArithOperator->GetValue(Operator))
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}
bool SVImageArithmetic::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	// All inputs and outputs must be validated first
	
	if  (false == SVTaskObjectClass::onRun(rRunStatus) )
	{
		return false;
	}

	SvIe::SVImageClass* pImageA = m_InputImageA.getInput<SvIe::SVImageClass>(true);
	SvIe::SVImageClass* pImageB = m_InputImageB.getInput<SvIe::SVImageClass>(true);
	SvIe::SVImageClass* pOutputImage = getOutputImage();
	SvVol::SVLongValueObjectClass* pArithOperator = m_InputArithmaticOperator.getInput<SvVol::SVLongValueObjectClass>();


	if (nullptr == pImageA || nullptr == pImageB || nullptr == pArithOperator || nullptr ==  pOutputImage)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	

	BOOL enableGain {FALSE}, enableLut {FALSE};
	if (getEnableLut(enableLut, pErrorMessages) == false)
	{
		rRunStatus.SetInvalid();
		return false;
	}

	if (getEnableGainAndOffset(enableGain, pErrorMessages) == false)
	{
		rRunStatus.SetInvalid();
		return false;
	}
	long lOperator {0};
	if (getOperator(lOperator, pErrorMessages) == false)
	{
		rRunStatus.SetInvalid();
		return false;
	}

	double gain {1.0}, offset {0.0};
	if (getGainAndOffset(gain, offset, pErrorMessages) == false)
	{
		rRunStatus.SetInvalid();
		return false;
	}

	SvOi::ITRCImagePtr pImageBufferA = pImageA->getImageReadOnly(rRunStatus.m_triggerRecord.get());
	SvOi::ITRCImagePtr pImageBufferB = pImageB->getImageReadOnly(rRunStatus.m_triggerRecord.get());
	SvOi::ITRCImagePtr pOutputImageBuffer = pOutputImage->getImageToWrite(rRunStatus.m_triggerRecord);

	if (nullptr == pImageBufferA || pImageBufferA->isEmpty() ||
		nullptr == pImageBufferB || pImageBufferB->isEmpty() ||
		nullptr == pOutputImageBuffer || pOutputImageBuffer->isEmpty())
	{

		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;

	}

	//@TODO[MEC][10.20][04.08.2021] The images sizes are different. Calculations are only in the range of the destination buffer
	// Check if using the same size would be more performant
#ifdef TRACE_IMAGE_SIZE  
	auto MBs1 = pImageBufferA->getHandle()->GetBuffer();
	MIL_INT Source_1 = MbufInquire(MBs1.GetIdentifier(), M_HOST_ADDRESS, M_NULL);
	MIL_INT Pitch_1 = MbufInquire(MBs1.GetIdentifier(), M_PITCH, M_NULL);
	MIL_INT Height_1 = MbufInquire(MBs1.GetIdentifier(), M_SIZE_Y, M_NULL);
	MIL_INT Width_1 = MbufInquire(MBs1.GetIdentifier(), M_SIZE_X, M_NULL);

	auto MBs2 = pImageBufferB->getHandle()->GetBuffer();
	MIL_INT Source_2 = MbufInquire(MBs2.GetIdentifier(), M_HOST_ADDRESS, M_NULL);
	MIL_INT Pitch_2 = MbufInquire(MBs2.GetIdentifier(), M_PITCH, M_NULL);
	MIL_INT Height_2 = MbufInquire(MBs2.GetIdentifier(), M_SIZE_Y, M_NULL);
	MIL_INT Width_2 = MbufInquire(MBs2.GetIdentifier(), M_SIZE_X, M_NULL);

	auto MBd = pOutputImageBuffer->getHandle()->GetBuffer();
	MIL_INT Dest = MbufInquire(MBd.GetIdentifier(), M_HOST_ADDRESS, M_NULL);
	MIL_INT PitchDest = MbufInquire(MBd.GetIdentifier(), M_PITCH, M_NULL);
	MIL_INT Height_Dest = MbufInquire(MBd.GetIdentifier(), M_SIZE_Y, M_NULL);
	MIL_INT Width_Dest = MbufInquire(MBd.GetIdentifier(), M_SIZE_X, M_NULL);

	OutputDebugString(SvUl::Format("S1H: %ld; S1W: %ld; S2H: %ld; S2W: %ld; dH: %ld; dW: %ld\n", 
		Height_1, Width_1, Height_2, Width_2, Height_Dest, Width_Dest).c_str());
#endif // TRACE_IMAGE_SIZE  

	if (enableGain && enableLut && m_pLookUp)
	{
		
		if (S_OK == SVMatroxImageInterface::ArithmeticLut(pOutputImageBuffer->getHandle()->GetBuffer(),
			pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(), m_pLookUp))
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	
	bool UseFloatBuffer = enableGain && SVMatroxImageInterface::isFloatBufferAllowed(SVImageOperationTypeEnum(lOperator));


	// Filter for special image arithmetic operators...
	// e.g. SvDef::SVImageOperatorAverage ( not defined by MIL )
	switch (lOperator)
	{
		case SvDef::SVImageOperatorAverage:
		{

			/*l_Code = */SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(), SVImageMultipleAccumulate);
			// Build average of two input images and store resulting image in output image...
			// NOTE: 
			//		 M_MULTIPLY_ACCUMULATE_2 
			//		 For a detailed parameter description refer to MIL Manual: MimArithMultiple( ... ).
			//
			// IMPORTANT:
			//		 Seems to use Double or DWORD for internal calculations, no Overflow Problem 
			//		 with 8 Bit Buffer if "Pixel of A" plus "Pixel of B" is greater 0xff !!!
			//
			//		 Formula: ( ( Src1 * Src2 ) + ( Src3 * Src4 ) ) / Src5      
			//
			//		 So the images could also be weighted, if needed for later use...
		}
		break;

		case SvDef::SVImageOperatorFlipVertical:
		{
			/*l_Code = */SVMatroxImageInterface::Flip(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), SVImageFlipVertical);
		}
		break;

		case SvDef::SVImageOperatorFlipHorizontal:
		{
			/*l_Code = */SVMatroxImageInterface::Flip(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), SVImageFlipHorizontal);
		}
		break;


		case SvDef::SVImageOperatorDoubleHeight:
		{
			RECT inputRect;
			RECT outputRect;

			if (S_OK == pImageA->GetImageInfo().GetOutputRectangle(inputRect) && S_OK == pOutputImage->GetImageInfo().GetOutputRectangle(outputRect))
			{
				ScaleWithAveraging(pImageBufferA, inputRect, pOutputImageBuffer, outputRect);
			}
		}
		break;


		default:
		{

			if (UseFloatBuffer)
			{
				SVMatroxImageInterface::Arithmetic(m_milFloatImageBuffer, pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(),
					static_cast<SVImageOperationTypeEnum>(lOperator));
			}
			else
			{
				SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(),
						static_cast<SVImageOperationTypeEnum>(lOperator));
			}
		}
	}

	if (UseFloatBuffer)
	{
		SVMatroxImageInterface::ArithmeticGainOffsetClip(pOutputImageBuffer->getHandle()->GetBuffer(), m_milFloatImageBuffer, gain, offset);
	}
	else if (enableGain && (gain != 1.0 || offset != 0.0))
	{
		SVMatroxImageInterface::ArithmeticGainOffset(pOutputImageBuffer->getHandle()->GetBuffer(), gain, offset);
	}


	return true;


}

void SVImageArithmetic::ScaleWithAveraging(SvOi::ITRCImagePtr pInputImageBuffer, const RECT& rInputRect, SvOi::ITRCImagePtr pOutputImageBuffer, const RECT& rOutputRect)
{
	if (nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty() && nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
	{
		LPVOID pSrcHostBuffer = pInputImageBuffer->getHandle()->GetBufferAddress();
		LPVOID pDstHostBuffer = pOutputImageBuffer->getHandle()->GetBufferAddress();

		if (pSrcHostBuffer && pDstHostBuffer)
		{
			if (rOutputRect.bottom && rInputRect.bottom)
			{
				long srcPitch = 0;
				long dstPitch = 0;
				/*HRESULT l_Code = */SVMatroxBufferInterface::Get(pInputImageBuffer->getHandle()->GetBuffer(), SVPitch, srcPitch);
				/*l_Code = */SVMatroxBufferInterface::Get(pOutputImageBuffer->getHandle()->GetBuffer(), SVPitch, dstPitch);

				unsigned char* srcLinePtr = (unsigned char*)pSrcHostBuffer;
				unsigned char* srcLinePtr1 = (unsigned char*)pSrcHostBuffer + srcPitch;

				unsigned char* dstLinePtr = (unsigned char*)pDstHostBuffer;

				///////////////////////////////////////////////////////
				// if the input image is the same as the output image
				// just clear the Image...
				///////////////////////////////////////////////////////
				if (pInputImageBuffer->getHandle() == pOutputImageBuffer->getHandle())
				{
					memset(dstLinePtr, '\0', dstPitch * rOutputRect.bottom);
					return;
				}

				/////////////////////////////////////////////////////////////////
				// Note: Output Image Height must be twice the Height 
				// of the Input Image, So we use the ROI of the Output and
				// divide it's height by 2
				/////////////////////////////////////////////////////////////////

				// Copy Lines...
				int numRows = (rOutputRect.bottom / 2);

				// check if odd number of lines
				if (!(rOutputRect.bottom % 2))
					numRows -= 1;

				// check if source image has enough rows
				if (rInputRect.bottom <= numRows)
					numRows = rInputRect.bottom - 1;

				int numCols = std::min(srcPitch, dstPitch);

				// see if a fill is needed
				if (srcPitch < dstPitch || rInputRect.bottom < (rOutputRect.bottom / 2))
					memset(dstLinePtr, '\0', dstPitch * rOutputRect.bottom);

				if (rOutputRect.bottom > 1 && rInputRect.bottom > 1)
				{
					// process the rows of pixels
					for (int row = 0; row < numRows; row++)
					{
						// Copy the Line
						memcpy(dstLinePtr, srcLinePtr, numCols);

						// Increment Destination Row Pointer
						dstLinePtr += dstPitch;

						// Fill in the next line with the average pixel
						for (int col = 0; col < numCols; col++)
						{
							dstLinePtr[col] = (srcLinePtr[col] + srcLinePtr1[col]) / 2;
						}

						// Increment Source Row Pointers
						srcLinePtr += srcPitch;
						srcLinePtr1 += srcPitch;

						// Increment Destination Row Pointer
						dstLinePtr += dstPitch;
					}
				}
				// Handle Last Line - copy the previous line
				memcpy(dstLinePtr, srcLinePtr, numCols);
			}
		}
	}
}

// Set String value object for Source Image Names
HRESULT SVImageArithmetic::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	if (nullptr != pTool)
	{
		SvIe::SVImageClass* pInputImage = m_InputImageA.getInput<SvIe::SVImageClass>();
		SvVol::SVStringValueObjectClass* pImageNames = pTool->GetInputImageNames();
		if (nullptr != pInputImage && nullptr != pImageNames)
		{
			pImageNames->SetValue(pInputImage->GetCompleteName(), 0);
			l_hr = S_OK;
		}

		pInputImage = m_InputImageB.getInput<SvIe::SVImageClass>();
		if (nullptr != pInputImage && nullptr != pImageNames)
		{
			pImageNames->SetValue(pInputImage->GetCompleteName(), 1);
		}
	}
	return l_hr;
}


bool SVImageArithmetic::ValidateLocal(SvStl::MessageContainerVector* pErrorMessages) const
{
	bool Result = true;

	SvIe::SVImageClass* pImageA = m_InputImageA.getInput<SvIe::SVImageClass>();
	SvIe::SVImageClass* pImageB = m_InputImageB.getInput<SvIe::SVImageClass>();
	SvVol::SVLongValueObjectClass* pArithOperator = m_InputArithmaticOperator.getInput<SvVol::SVLongValueObjectClass>();

	if (nullptr == pImageA || nullptr == pImageB || nullptr == pArithOperator)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}



} //namespace SvOp
