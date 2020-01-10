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
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "InspectionEngine/SVImageClass.h"
#include "Tools/SVTool.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS( SVImageArithmeticClass, SVImageArithmeticClassGuid )

SVImageArithmeticClass::SVImageArithmeticClass( SVObjectClass* POwner, int StringResourceID )
					   :SVTaskObjectClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVImageArithmeticObjectType;

	// Identify our input type needs...

	// Input A...
	// Image
	m_InputImageAInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_InputImageAInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageAInfo, _T( "ImageArithmeticAImage" ) );
	// Enable Offset
	m_InputEnableOffsetAInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SVEnableOffsetAObjectGuid);
	m_InputEnableOffsetAInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputEnableOffsetAInfo, _T( "ImageArithmeticAEnableOffset" ) );
	// Offset Point
	m_InputOffsetAPointInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType, SVOffsetAPointObjectGuid);
	m_InputOffsetAPointInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputOffsetAPointInfo, _T( "ImageArithmeticAOffsetPoint" ) );

	// Input B...
	// Image
	m_InputImageBInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_InputImageBInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageBInfo, _T( "ImageArithmeticBImage" ) );
	// Enable Offset
	m_InputEnableOffsetBInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SVEnableOffsetBObjectGuid);
	m_InputEnableOffsetBInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputEnableOffsetBInfo, _T( "ImageArithmeticBEnableOffset" ) );
	// Offset Point
	m_InputOffsetBPointInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVPointValueObjectType, SVOffsetBPointObjectGuid);
	m_InputOffsetBPointInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputOffsetBPointInfo, _T( "ImageArithmeticBOffsetPoint" ) );

	// Operator Input...
	m_InputArithmaticOperatorInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVLongValueObjectType, SVArithmeticOperatorObjectGuid);
	m_InputArithmaticOperatorInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputArithmaticOperatorInfo, _T( "ImageArithmeticOperator" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_OutputImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults

	m_OutputImage.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	// Set up draw objects...
//	graphFigure.SetDrawPen( TRUE, PS_SOLID, 1, SvDef::DefaultSubFunctionColor1 );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImageArithmeticClass::~SVImageArithmeticClass()
{
	SVImageArithmeticClass::CloseObject();
}

bool SVImageArithmeticClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	bOk &= ( S_OK == m_OutputImage.InitializeImage(SvOl::getInput<SvIe::SVImageClass>(m_InputImageAInfo)));

	// Reset Printable flag
	m_OutputImage.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVImageArithmeticClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() && getOutputImage() )
	{
		return getOutputImage()->CloseObject();
	}
	return false;
}

bool SVImageArithmeticClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result{true};

	SvOl::SVInObjectInfoStructPtrVector InputList
	{
		&m_InputImageAInfo,
		&m_InputEnableOffsetAInfo,
		&m_InputOffsetAPointInfo,
		&m_InputImageBInfo,
		&m_InputEnableOffsetBInfo,
		&m_InputOffsetBPointInfo,
		&m_InputArithmaticOperatorInfo
	};

	SvOl::ValidateInputList(InputList);

	HRESULT l_hrOk = m_OutputImage.InitializeImage(SvOl::getInput<SvIe::SVImageClass>(m_InputImageAInfo));
	if (S_OK != l_hrOk)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	CollectInputImageNames();

	Result = SVTaskObjectClass::ResetObject(pErrorMessages) && Result && ValidateLocal(pErrorMessages);
	return Result;
}

bool SVImageArithmeticClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	const SvIe::SVImageClass* pImageA = SvOl::getInput<SvIe::SVImageClass>(m_InputImageAInfo);
	const SvIe::SVImageClass* pImageB = SvOl::getInput<SvIe::SVImageClass>(m_InputImageBInfo);
	if ((nullptr != pImageA && rImageGuid == pImageA->GetUniqueObjectID()) || (nullptr != pImageB && rImageGuid == pImageB->GetUniqueObjectID()))
	{
		Result = true;
	}

	return Result;
}

bool SVImageArithmeticClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool Return = SVTaskObjectClass::onRun(rRunStatus) && ValidateLocal(pErrorMessages);
	if (Return)
	{
		SvIe::SVImageClass* pImageA = SvOl::getInput<SvIe::SVImageClass>(m_InputImageAInfo, true);
		SvVol::SVBoolValueObjectClass*  pEnableOffsetA = SvOl::getInput<SvVol::SVBoolValueObjectClass>(m_InputEnableOffsetAInfo, true);
		SvVol::SVPointValueObjectClass* pOffsetAPoint  = SvOl::getInput<SvVol::SVPointValueObjectClass>(m_InputOffsetAPointInfo, true);
		SvIe::SVImageClass* pImageB = SvOl::getInput<SvIe::SVImageClass>(m_InputImageBInfo, true);
		SvVol::SVBoolValueObjectClass*  pEnableOffsetB = SvOl::getInput<SvVol::SVBoolValueObjectClass>(m_InputEnableOffsetBInfo, true);
		SvVol::SVPointValueObjectClass* pOffsetBPoint = SvOl::getInput<SvVol::SVPointValueObjectClass>(m_InputOffsetBPointInfo, true);
		SvVol::SVLongValueObjectClass* pArithOperator = SvOl::getInput<SvVol::SVLongValueObjectClass>(m_InputArithmaticOperatorInfo, true);
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		SvTrc::IImagePtr pImageBufferA = pImageA->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		SvTrc::IImagePtr pImageBufferB = pImageB->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		SvTrc::IImagePtr pOutputImageBuffer = pOutputImage->getImageToWrite(rRunStatus.m_triggerRecord);

		if ( nullptr != pImageBufferA && !pImageBufferA->isEmpty() &&
			nullptr != pImageBufferB && !pImageBufferB->isEmpty() &&
			nullptr != pOutputImageBuffer && !pOutputImageBuffer->isEmpty())
		{
			HRESULT l_Code;

			// Filter for special image arithmetic operators...
			// e.g. SvDef::SVImageOperatorAverage ( not defined by MIL )
			switch( lOperator )
			{
			case SvDef::SVImageOperatorAverage:
				{

					l_Code = SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(), SVImageMultipleAccumulate );
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
					l_Code = SVMatroxImageInterface::Flip(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), SVImageFlipVertical );
				}
				break;

			case SvDef::SVImageOperatorFlipHorizontal:
				{
					l_Code = SVMatroxImageInterface::Flip(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), SVImageFlipHorizontal );
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
				l_Code = SVMatroxImageInterface::Arithmetic(pOutputImageBuffer->getHandle()->GetBuffer(), pImageBufferA->getHandle()->GetBuffer(), pImageBufferB->getHandle()->GetBuffer(),
					static_cast<SVImageOperationTypeEnum>(lOperator) );
			}

			return true;
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}
	return false;
}

void SVImageArithmeticClass::ScaleWithAveraging(SvTrc::IImagePtr pInputImageBuffer, const RECT& rInputRect, SvTrc::IImagePtr pOutputImageBuffer, const RECT& rOutputRect)
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
HRESULT SVImageArithmeticClass::CollectInputImageNames( )
{
	HRESULT l_hr = S_FALSE;
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	if (nullptr != pTool)
	{
		SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageAInfo);
		SvVol::SVStringValueObjectClass* pImageNames = pTool->GetInputImageNames();
		if( nullptr != pInputImage && nullptr != pImageNames )
		{
			pImageNames->SetValue( pInputImage->GetCompleteName(), 0 );
			l_hr = S_OK;
		}

		pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageBInfo);
		if( nullptr != pInputImage && nullptr != pImageNames)
		{
			pImageNames->SetValue( pInputImage->GetCompleteName(), 1 );
		}
	}
	return l_hr;
}


bool SVImageArithmeticClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result = true;

	SvIe::SVImageClass* pImageA = SvOl::getInput<SvIe::SVImageClass>(m_InputImageAInfo);
	SvVol::SVBoolValueObjectClass*  pEnableOffsetA = SvOl::getInput<SvVol::SVBoolValueObjectClass>(m_InputEnableOffsetAInfo);
	SvVol::SVPointValueObjectClass* pOffsetAPoint = SvOl::getInput<SvVol::SVPointValueObjectClass>(m_InputOffsetAPointInfo);
	SvIe::SVImageClass* pImageB = SvOl::getInput<SvIe::SVImageClass>(m_InputImageBInfo);
	SvVol::SVBoolValueObjectClass*  pEnableOffsetB = SvOl::getInput<SvVol::SVBoolValueObjectClass>(m_InputEnableOffsetBInfo);
	SvVol::SVPointValueObjectClass* pOffsetBPoint = SvOl::getInput<SvVol::SVPointValueObjectClass>(m_InputOffsetBPointInfo);
	SvVol::SVLongValueObjectClass* pArithOperator = SvOl::getInput<SvVol::SVLongValueObjectClass>(m_InputArithmaticOperatorInfo);

	if( nullptr == pImageA || nullptr == pEnableOffsetA || nullptr == pOffsetAPoint ||
		nullptr == pImageB || nullptr == pEnableOffsetB || nullptr == pOffsetBPoint || nullptr == pArithOperator)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

} //namespace SvOp
