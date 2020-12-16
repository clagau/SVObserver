//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVStatusLibrary/RunStatus.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/SVPointValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVImageArithmetic, SvPb::ImageArithmeticClassId)

SVImageArithmetic::SVImageArithmetic( SVObjectClass* POwner, int StringResourceID )
					   :SVTaskObjectClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVImageArithmeticObjectType;

	// Identify our input type needs...

	// Input A...
	// Image
	m_InputImageA.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_InputImageA, _T( "ImageArithmeticAImage" ), SvPb::ImageArithmeticAImageInputEId);
	// Enable Offset
	m_InputEnableOffsetA.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::EnableOffsetAEId);
	registerInputObject( &m_InputEnableOffsetA, _T( "ImageArithmeticAEnableOffset" ), SvPb::ImageArithmeticAEnableOffsetInputEId);
	m_InputEnableOffsetA.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;
	// Offset Point
	m_InputOffsetAPoint.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType, SvPb::OffsetAPointEId);
	registerInputObject( &m_InputOffsetAPoint, _T( "ImageArithmeticAOffsetPoint" ), SvPb::ImageArithmeticAOffsetPointInputEId);
	m_InputOffsetAPoint.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Input B...
	// Image
	m_InputImageB.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_InputImageB, _T( "ImageArithmeticBImage" ), SvPb::ImageArithmeticBImageInputEId);
	// Enable Offset
	m_InputEnableOffsetB.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::EnableOffsetBEId);
	registerInputObject( &m_InputEnableOffsetB, _T( "ImageArithmeticBEnableOffset" ), SvPb::ImageArithmeticBEnableOffsetInputEId);
	m_InputEnableOffsetB.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;
	// Offset Point
	m_InputOffsetBPoint.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType, SvPb::OffsetBPointEId);
	registerInputObject( &m_InputOffsetBPoint, _T( "ImageArithmeticBOffsetPoint" ), SvPb::ImageArithmeticBOffsetPointInputEId);
	m_InputOffsetBPoint.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Operator Input...
	m_InputArithmaticOperator.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVLongValueObjectType, SvPb::ArithmeticOperatorEId);
	registerInputObject( &m_InputArithmaticOperator, _T( "ImageArithmeticOperator" ), SvPb::ImageArithmeticOperatorInputEId);
	m_InputArithmaticOperator.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults

	m_OutputImage.InitializeImage( SvPb::SVImageTypeEnum::SVImageTypePhysical );

	// Set up draw objects...
//	graphFigure.SetDrawPen( TRUE, PS_SOLID, 1, SvDef::DefaultSubFunctionColor1 );
}

SVImageArithmetic::~SVImageArithmetic()
{
	SVImageArithmetic::CloseObject();
}

bool SVImageArithmetic::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	bOk &= ( S_OK == m_OutputImage.InitializeImage(m_InputImageA.getInput<SvIe::SVImageClass>()));

	// Reset Printable flag
	m_OutputImage.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVImageArithmetic::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() && getOutputImage() )
	{
		return getOutputImage()->CloseObject();
	}
	return false;
}

bool SVImageArithmetic::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result{true};

	m_InputImageA.validateInput();
	m_InputEnableOffsetA.validateInput();
	m_InputOffsetAPoint.validateInput();
	m_InputImageB.validateInput();
	m_InputEnableOffsetB.validateInput();
	m_InputOffsetBPoint.validateInput();
	m_InputArithmaticOperator.validateInput();
	
	HRESULT l_hrOk = m_OutputImage.InitializeImage(m_InputImageA.getInput<SvIe::SVImageClass>());
	if (S_OK != l_hrOk)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	CollectInputImageNames();

	Result = SVTaskObjectClass::ResetObject(pErrorMessages) && Result && ValidateLocal(pErrorMessages);
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

bool SVImageArithmetic::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool Return = SVTaskObjectClass::onRun(rRunStatus) && ValidateLocal(pErrorMessages);
	if (Return)
	{
		SvIe::SVImageClass* pImageA = m_InputImageA.getInput<SvIe::SVImageClass>(true);
		SvVol::SVBoolValueObjectClass*  pEnableOffsetA = m_InputEnableOffsetA.getInput<SvVol::SVBoolValueObjectClass>(true);
		SvVol::SVPointValueObjectClass* pOffsetAPoint  = m_InputOffsetAPoint.getInput<SvVol::SVPointValueObjectClass>(true);
		SvIe::SVImageClass* pImageB = m_InputImageB.getInput<SvIe::SVImageClass>(true);
		SvVol::SVBoolValueObjectClass*  pEnableOffsetB = m_InputEnableOffsetB.getInput<SvVol::SVBoolValueObjectClass>(true);
		SvVol::SVPointValueObjectClass* pOffsetBPoint = m_InputOffsetBPoint.getInput<SvVol::SVPointValueObjectClass>(true);
		SvVol::SVLongValueObjectClass* pArithOperator = m_InputArithmaticOperator.getInput<SvVol::SVLongValueObjectClass>(true);
		SvIe::SVImageClass* pOutputImage   = getOutputImage();

		assert(pImageA);
		assert(pEnableOffsetA);
		assert(pOffsetAPoint);
		assert(pImageB);
		assert(pEnableOffsetB);
		assert(pOffsetBPoint);
		assert(pArithOperator);
		assert(pOutputImage);

		BOOL bEnableOffsetA;
		if( S_OK != pEnableOffsetA->GetValue( bEnableOffsetA ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		SVPoint<long> offsetA;
		if( S_OK != pOffsetAPoint->GetValue( offsetA ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		BOOL bEnableOffsetB;
		if( S_OK != pEnableOffsetB->GetValue( bEnableOffsetB ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		SVPoint<long> offsetB;
		if( S_OK != pOffsetBPoint->GetValue( offsetB ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		long lOperator;
		if( S_OK != pArithOperator->GetValue( lOperator ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		SvOi::ITRCImagePtr pImageBufferA = pImageA->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		SvOi::ITRCImagePtr pImageBufferB = pImageB->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		SvOi::ITRCImagePtr pOutputImageBuffer = pOutputImage->getImageToWrite(rRunStatus.m_triggerRecord);

		if ( nullptr != pImageBufferA && !pImageBufferA->isEmpty() &&
			nullptr != pImageBufferB && !pImageBufferB->isEmpty() &&
			nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
		{
			// Filter for special image arithmetic operators...
			// e.g. SvDef::SVImageOperatorAverage ( not defined by MIL )
			switch( lOperator )
			{
			case SvDef::SVImageOperatorAverage:
				{

					/*l_Code = */SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(), SVImageMultipleAccumulate );
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
					/*l_Code = */SVMatroxImageInterface::Flip(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), SVImageFlipVertical );
				}
				break;

			case SvDef::SVImageOperatorFlipHorizontal:
				{
					/*l_Code = */SVMatroxImageInterface::Flip(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), SVImageFlipHorizontal );
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
				// Default Operation,,,
				/*l_Code = */SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(),
					static_cast<SVImageOperationTypeEnum>(lOperator) );
			}

			return true;
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}
	return false;
}

void SVImageArithmetic::ScaleWithAveraging(SvOi::ITRCImagePtr pInputImageBuffer, const RECT& rInputRect, SvOi::ITRCImagePtr pOutputImageBuffer, const RECT& rOutputRect)
{
	if(nullptr != pInputImageBuffer && !pInputImageBuffer->isEmpty() && nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
	{
		LPVOID pSrcHostBuffer = pInputImageBuffer->getHandle()->GetBufferAddress();
		LPVOID pDstHostBuffer = pOutputImageBuffer->getHandle()->GetBufferAddress();
		
		if( pSrcHostBuffer && pDstHostBuffer )
		{
			if(rOutputRect.bottom && rInputRect.bottom )
			{
				long srcPitch = 0;
				long dstPitch = 0;
				/*HRESULT l_Code = */SVMatroxBufferInterface::Get(pInputImageBuffer->getHandle()->GetBuffer(), SVPitch, srcPitch );
				/*l_Code = */SVMatroxBufferInterface::Get(pOutputImageBuffer->getHandle()->GetBuffer(), SVPitch, dstPitch );
				
				unsigned char* srcLinePtr = ( unsigned char * )pSrcHostBuffer;
				unsigned char* srcLinePtr1 = ( unsigned char * )pSrcHostBuffer + srcPitch;
				
				unsigned char* dstLinePtr = ( unsigned char * )pDstHostBuffer;
				
				///////////////////////////////////////////////////////
				// if the input image is the same as the output image
				// just clear the Image...
				///////////////////////////////////////////////////////
				if(pInputImageBuffer->getHandle() == pOutputImageBuffer->getHandle())
				{
					memset( dstLinePtr, '\0', dstPitch * rOutputRect.bottom );
					return;
				}
				
				/////////////////////////////////////////////////////////////////
				// Note: Output Image Height must be twice the Height 
				// of the Input Image, So we use the ROI of the Output and
				// divide it's height by 2
				/////////////////////////////////////////////////////////////////
				
				// Copy Lines...
				int numRows = (rOutputRect.bottom / 2 );
				
				// check if odd number of lines
				if( !(rOutputRect.bottom % 2 ) )
					numRows -= 1;
				
				// check if source image has enough rows
				if(rInputRect.bottom <= numRows )
					numRows = rInputRect.bottom - 1;
				
				int numCols = std::min( srcPitch, dstPitch );
				
				// see if a fill is needed
				if( srcPitch < dstPitch || rInputRect.bottom < (rOutputRect.bottom / 2 ) )
					memset( dstLinePtr, '\0', dstPitch * rOutputRect.bottom );
				
				if(rOutputRect.bottom > 1 && rInputRect.bottom > 1)
				{
					// process the rows of pixels
					for( int row = 0;row < numRows;row++ )
					{
						// Copy the Line
						memcpy( dstLinePtr, srcLinePtr, numCols );
						
						// Increment Destination Row Pointer
						dstLinePtr += dstPitch;
						
						// Fill in the next line with the average pixel
						for(int col = 0;col < numCols;col++ )
						{
							dstLinePtr[col] = ( srcLinePtr[col] + srcLinePtr1[col] ) / 2;
						}
						
						// Increment Source Row Pointers
						srcLinePtr += srcPitch;
						srcLinePtr1 += srcPitch;
						
						// Increment Destination Row Pointer
						dstLinePtr += dstPitch;
					}
				}
				// Handle Last Line - copy the previous line
				memcpy( dstLinePtr, srcLinePtr, numCols );
			}
		}
	}
}

// Set String value object for Source Image Names
HRESULT SVImageArithmetic::CollectInputImageNames( )
{
	HRESULT l_hr = S_FALSE;
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	if (nullptr != pTool)
	{
		SvIe::SVImageClass* pInputImage = m_InputImageA.getInput<SvIe::SVImageClass>();
		SvVol::SVStringValueObjectClass* pImageNames = pTool->GetInputImageNames();
		if( nullptr != pInputImage && nullptr != pImageNames )
		{
			pImageNames->SetValue( pInputImage->GetCompleteName(), 0 );
			l_hr = S_OK;
		}

		pInputImage = m_InputImageB.getInput<SvIe::SVImageClass>();
		if( nullptr != pInputImage && nullptr != pImageNames)
		{
			pImageNames->SetValue( pInputImage->GetCompleteName(), 1 );
		}
	}
	return l_hr;
}


bool SVImageArithmetic::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result = true;

	SvIe::SVImageClass* pImageA = m_InputImageA.getInput<SvIe::SVImageClass>();
	SvVol::SVBoolValueObjectClass*  pEnableOffsetA = m_InputEnableOffsetA.getInput<SvVol::SVBoolValueObjectClass>();
	SvVol::SVPointValueObjectClass* pOffsetAPoint = m_InputOffsetAPoint.getInput<SvVol::SVPointValueObjectClass>();
	SvIe::SVImageClass* pImageB = m_InputImageB.getInput<SvIe::SVImageClass>();
	SvVol::SVBoolValueObjectClass*  pEnableOffsetB = m_InputEnableOffsetB.getInput<SvVol::SVBoolValueObjectClass>();
	SvVol::SVPointValueObjectClass* pOffsetBPoint = m_InputOffsetBPoint.getInput<SvVol::SVPointValueObjectClass>();
	SvVol::SVLongValueObjectClass* pArithOperator = m_InputArithmaticOperator.getInput<SvVol::SVLongValueObjectClass>();

	if( nullptr == pImageA || nullptr == pEnableOffsetA || nullptr == pOffsetAPoint ||
		nullptr == pImageB || nullptr == pEnableOffsetB || nullptr == pOffsetBPoint || nullptr == pArithOperator)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

} //namespace SvOp
