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

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"

#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVTool.h"
#include "SVToolImage.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVImageArithmeticClass, SVImageArithmeticClassGuid )

SVImageArithmeticClass::SVImageArithmeticClass( SVObjectClass* POwner, int StringResourceID )
					   :SVTaskObjectClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVImageArithmeticObjectType;

	// Identify our input type needs...

	// Input A...
	// Image
	m_InputImageAInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_InputImageAInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageAInfo, _T( "ImageArithmeticAImage" ) );
	// Enable Offset
	m_InputEnableOffsetAInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVBoolValueObjectType, SVEnableOffsetAObjectGuid);
	m_InputEnableOffsetAInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputEnableOffsetAInfo, _T( "ImageArithmeticAEnableOffset" ) );
	// Offset Point
	m_InputOffsetAPointInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVPointValueObjectType, SVOffsetAPointObjectGuid);
	m_InputOffsetAPointInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputOffsetAPointInfo, _T( "ImageArithmeticAOffsetPoint" ) );

	// Input B...
	// Image
	m_InputImageBInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_InputImageBInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageBInfo, _T( "ImageArithmeticBImage" ) );
	// Enable Offset
	m_InputEnableOffsetBInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVBoolValueObjectType, SVEnableOffsetBObjectGuid);
	m_InputEnableOffsetBInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputEnableOffsetBInfo, _T( "ImageArithmeticBEnableOffset" ) );
	// Offset Point
	m_InputOffsetBPointInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVPointValueObjectType, SVOffsetBPointObjectGuid);
	m_InputOffsetBPointInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputOffsetBPointInfo, _T( "ImageArithmeticBOffsetPoint" ) );

	// Operator Input...
	m_InputArithmaticOperatorInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVLongValueObjectType, SVArithmeticOperatorObjectGuid);
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
	CloseObject();
}

bool SVImageArithmeticClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	bOk &= ( S_OK == m_OutputImage.InitializeImage( getInputImageA() ) );

	// Reset Printable flag
	m_OutputImage.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

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

	SVInObjectInfoStruct* InputList[]
	{
		&m_InputImageAInfo,
		&m_InputEnableOffsetAInfo,
		&m_InputOffsetAPointInfo,
		&m_InputImageBInfo,
		&m_InputEnableOffsetBInfo,
		&m_InputOffsetBPointInfo,
		&m_InputArithmaticOperatorInfo
	};

	for (auto pEntry : InputList)
	{
		// Check if the input object is still valid otherwise the pointer is invalid
		// Pointer do not need to be checked as the list are pointers of member variables
		if (pEntry->IsConnected() && !pEntry->GetInputObjectInfo().CheckExistence())
		{
			pEntry->SetInputObject(nullptr);
		}
	}

	HRESULT l_hrOk = m_OutputImage.InitializeImage( getInputImageA() );
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

	const SVImageClass* pImageA = getInputImageA();
	const SVImageClass* pImageB = getInputImageB();
	if ((nullptr != pImageA && rImageGuid == pImageA->GetUniqueObjectID()) || (nullptr != pImageB && rImageGuid == pImageB->GetUniqueObjectID()))
	{
		Result = true;
	}

	return Result;
}

SVImageClass* SVImageArithmeticClass::getInputImageA(bool bRunMode /*= false*/) const
{
	if (m_InputImageAInfo.IsConnected() && nullptr != m_InputImageAInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputImageAInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVImageClass*> (pObject) : dynamic_cast<SVImageClass*> (pObject);
	}

	return nullptr;
}

SVBoolValueObjectClass* SVImageArithmeticClass::getInputEnableOffsetA(bool bRunMode /*= false*/) const
{
	if (m_InputEnableOffsetAInfo.IsConnected() && m_InputEnableOffsetAInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputEnableOffsetAInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVBoolValueObjectClass*> (pObject) : dynamic_cast<SVBoolValueObjectClass*> (pObject);
	}

	return nullptr;
}

SVPointValueObjectClass* SVImageArithmeticClass::getInputOffsetAPoint(bool bRunMode /*= false*/) const
{
	if (m_InputOffsetAPointInfo.IsConnected() && m_InputOffsetAPointInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputOffsetAPointInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVPointValueObjectClass*> (pObject) : dynamic_cast<SVPointValueObjectClass*> (pObject);
	}

	return nullptr;
}

SVImageClass* SVImageArithmeticClass::getInputImageB(bool bRunMode /*= false*/) const
{
	if (m_InputImageBInfo.IsConnected() && nullptr != m_InputImageBInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputImageBInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVImageClass*> (pObject) : dynamic_cast<SVImageClass*> (pObject);
	}

	return nullptr;
}

SVBoolValueObjectClass* SVImageArithmeticClass::getInputEnableOffsetB(bool bRunMode /*= false*/) const
{
	if (m_InputEnableOffsetBInfo.IsConnected() && m_InputEnableOffsetBInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputEnableOffsetBInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVBoolValueObjectClass*> (pObject) : dynamic_cast<SVBoolValueObjectClass*> (pObject);
	}

	return nullptr;
}

SVPointValueObjectClass* SVImageArithmeticClass::getInputOffsetBPoint(bool bRunMode /*= false*/) const
{
	if (m_InputOffsetBPointInfo.IsConnected() && m_InputOffsetBPointInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputOffsetBPointInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVPointValueObjectClass*> (pObject) : dynamic_cast<SVPointValueObjectClass*> (pObject);
	}

	return nullptr;
}

SVLongValueObjectClass* SVImageArithmeticClass::getInputArithOperator(bool bRunMode /*= false*/) const
{
	if (m_InputArithmaticOperatorInfo.IsConnected() && m_InputArithmaticOperatorInfo.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = m_InputArithmaticOperatorInfo.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<SVLongValueObjectClass*> (pObject) : dynamic_cast<SVLongValueObjectClass*> (pObject);
	}

	return nullptr;
}
	
SVImageClass* SVImageArithmeticClass::getOutputImage()
{
	return &m_OutputImage;
}

bool SVImageArithmeticClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool Return = SVTaskObjectClass::onRun(rRunStatus) && ValidateLocal(pErrorMessages);
	if (Return)
	{
		SVImageClass*			 pImageA		= getInputImageA(true);
		ASSERT( pImageA );
		SVBoolValueObjectClass*  pEnableOffsetA = getInputEnableOffsetA();
		ASSERT( pEnableOffsetA );
		SVPointValueObjectClass* pOffsetAPoint  = getInputOffsetAPoint();
		ASSERT( pOffsetAPoint );
		SVImageClass*			 pImageB		= getInputImageB(true);
		ASSERT( pImageB );
		SVBoolValueObjectClass*  pEnableOffsetB	= getInputEnableOffsetB();
		ASSERT( pEnableOffsetB );
		SVPointValueObjectClass* pOffsetBPoint  = getInputOffsetBPoint();
		ASSERT( pOffsetBPoint );
		SVLongValueObjectClass*  pArithOperator = getInputArithOperator();
		ASSERT( pArithOperator );
		SVImageClass*            pOutputImage   = getOutputImage();
		ASSERT( pOutputImage );

		if ( ! pOutputImage->SetImageHandleIndex( rRunStatus.Images ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		BOOL bEnableOffsetA;
		if( S_OK != pEnableOffsetA->GetValue( bEnableOffsetA ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		SVPoint offsetA;
		if( S_OK != pOffsetAPoint->GetValue( offsetA ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			SetInvalid();
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
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		SVPoint offsetB;
		if( S_OK != pOffsetBPoint->GetValue( offsetB ) )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			SetInvalid();
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
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		SvOi::SVImageBufferHandlePtr HandleA;
		SvOi::SVImageBufferHandlePtr HandleB;
		SvOi::SVImageBufferHandlePtr Output;

		if ( pImageA->GetImageHandle( HandleA ) && nullptr != HandleA &&
			pImageB->GetImageHandle( HandleB ) && nullptr != HandleB &&
			pOutputImage->GetImageHandle( Output ) && nullptr != Output &&
			!(HandleA->empty() ) &&  !(HandleB->empty() ) && !(Output->empty() ) )
		{
			HRESULT l_Code;

			// Filter for special image arithmetic operators...
			// e.g. SvDef::SVImageOperatorAverage ( not defined by MIL )
			switch( lOperator )
			{
			case SvDef::SVImageOperatorAverage:
				{

					l_Code = SVMatroxImageInterface::Arithmetic(Output->GetBuffer(), HandleA->GetBuffer(), HandleB->GetBuffer(), SVImageMultipleAccumulate );
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
					l_Code = SVMatroxImageInterface::Flip(Output->GetBuffer(), HandleA->GetBuffer(), SVImageFlipVertical );
				}
				break;

			case SvDef::SVImageOperatorFlipHorizontal:
				{
					l_Code = SVMatroxImageInterface::Flip(Output->GetBuffer(), HandleA->GetBuffer(), SVImageFlipHorizontal );
				}
				break;


			case SvDef::SVImageOperatorDoubleHeight:
				{
					ScaleWithAveraging( pImageA, pOutputImage );
				}
				break;


			default:
				// Default Operation,,,
				l_Code = SVMatroxImageInterface::Arithmetic(Output->GetBuffer(), HandleA->GetBuffer(), HandleB->GetBuffer(),
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

void SVImageArithmeticClass::ScaleWithAveraging( SVImageClass* pInputImage, SVImageClass* pOutputImage )
{
	if( pInputImage && pOutputImage )
	{
		LPVOID pSrcHostBuffer = nullptr;
		LPVOID pDstHostBuffer = nullptr;
		unsigned char *srcLinePtr,*srcLinePtr1,*dstLinePtr;
	
		long srcPitch = 0;
		long dstPitch = 0;
		int row = 0;
		int col = 0;

		SVImageInfoClass InputImageInfo = pInputImage->GetImageInfo();
		SVImageInfoClass OutputImageInfo = pOutputImage->GetImageInfo();

		SvOi::SVImageBufferHandlePtr InputImageBufferHandle;
		SvOi::SVImageBufferHandlePtr OutputImageBufferHandle;

		RECT l_oInputRect;
		RECT l_oOutputRect;
		
		if( pInputImage->GetImageHandle( InputImageBufferHandle ) && nullptr != InputImageBufferHandle &&
			pOutputImage->GetImageHandle( OutputImageBufferHandle ) && nullptr != OutputImageBufferHandle &&
			S_OK == InputImageInfo.GetOutputRectangle( l_oInputRect ) &&
			S_OK == OutputImageInfo.GetOutputRectangle( l_oOutputRect ) )
		{
			pSrcHostBuffer = InputImageBufferHandle->GetBufferAddress();
			pDstHostBuffer = OutputImageBufferHandle->GetBufferAddress();
		}
		
		if( pSrcHostBuffer && pDstHostBuffer && !(InputImageBufferHandle->empty() ) && !(OutputImageBufferHandle->empty() ) )
		{
			HRESULT l_Code;
			
			if( l_oOutputRect.bottom && l_oInputRect.bottom )
			{
				l_Code = SVMatroxBufferInterface::Get(InputImageBufferHandle->GetBuffer(), SVPitch, srcPitch );
				l_Code = SVMatroxBufferInterface::Get(OutputImageBufferHandle->GetBuffer(), SVPitch, dstPitch );
				
				srcLinePtr = ( unsigned char * )pSrcHostBuffer;
				srcLinePtr1 = ( unsigned char * )pSrcHostBuffer + srcPitch;
				
				dstLinePtr = ( unsigned char * )pDstHostBuffer;
				
				///////////////////////////////////////////////////////
				// if the input image is the same as the output image
				// just clear the Image...
				///////////////////////////////////////////////////////
				if( pInputImage == pOutputImage )
				{
					memset( dstLinePtr, '\0', dstPitch * l_oOutputRect.bottom );
					return;
				}
				
				/////////////////////////////////////////////////////////////////
				// Note: Output Image Height must be twice the Height 
				// of the Input Image, So we use the ROI of the Output and
				// divide it's height by 2
				/////////////////////////////////////////////////////////////////
				
				// Copy Lines...
				int numRows = ( l_oOutputRect.bottom / 2 );
				
				// check if odd number of lines
				if( !( l_oOutputRect.bottom % 2 ) )
					numRows -= 1;
				
				// check if source image has enough rows
				if( l_oInputRect.bottom <= numRows )
					numRows = l_oInputRect.bottom - 1;
				
				int numCols = std::min( srcPitch, dstPitch );
				
				// see if a fill is needed
				if( srcPitch < dstPitch || l_oInputRect.bottom < ( l_oOutputRect.bottom / 2 ) )
					memset( dstLinePtr, '\0', dstPitch * l_oOutputRect.bottom );
				
				if( l_oOutputRect.bottom > 1 && l_oInputRect.bottom > 1)
				{
					// process the rows of pixels
					for( row = 0;row < numRows;row++ )
					{
						// Copy the Line
						memcpy( dstLinePtr, srcLinePtr, numCols );
						
						// Increment Destination Row Pointer
						dstLinePtr += dstPitch;
						
						// Fill in the next line with the average pixel
						for( col = 0;col < numCols;col++ )
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
	SVImageToolClass* pTool = dynamic_cast<SVImageToolClass*>(GetTool());
	if (nullptr != pTool)
	{
		SVImageClass* pInputImage = getInputImageA();
		SVStringValueObjectClass* pImageNames = pTool->GetInputImageNames();
		if( nullptr != pInputImage && nullptr != pImageNames )
		{
			pImageNames->SetValue( pInputImage->GetCompleteName(), 0 );
			l_hr = S_OK;
		}

		pInputImage = getInputImageB();
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

	if( nullptr == getInputImageA() || nullptr == getInputEnableOffsetA() || nullptr == getInputOffsetAPoint() ||
		nullptr == getInputImageB() || nullptr == getInputEnableOffsetB() || nullptr == getInputOffsetBPoint() || nullptr == getInputArithOperator() )
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


