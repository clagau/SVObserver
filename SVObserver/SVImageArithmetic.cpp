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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVTool.h"
#include "SVToolImage.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVImageArithmeticClass, SVImageArithmeticClassGuid )

SVImageArithmeticClass::SVImageArithmeticClass( SVObjectClass* POwner, int StringResourceID )
					   :SVTaskObjectClass( POwner, StringResourceID ) 
{

	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVImageArithmeticObjectType;

	// Identify our input type needs...

	// Input A...
	// Image
	inputImageAInfo.SetInputObjectType( SVImageObjectType );
	inputImageAInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageAInfo, _T( "ImageArithmeticAImage" ) );
	// Enable Offset
	inputEnableOffsetAInfo.SetInputObjectType( SVEnableOffsetAObjectGuid, SVBoolValueObjectType );
	inputEnableOffsetAInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEnableOffsetAInfo, _T( "ImageArithmeticAEnableOffset" ) );
	// Offset Point
	inputOffsetAPointInfo.SetInputObjectType( SVOffsetAPointObjectGuid, SVPointValueObjectType );
	inputOffsetAPointInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputOffsetAPointInfo, _T( "ImageArithmeticAOffsetPoint" ) );

	// Input B...
	// Image
	inputImageBInfo.SetInputObjectType( SVImageObjectType );
	inputImageBInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageBInfo, _T( "ImageArithmeticBImage" ) );
	// Enable Offset
	inputEnableOffsetBInfo.SetInputObjectType( SVEnableOffsetBObjectGuid, SVBoolValueObjectType );
	inputEnableOffsetBInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputEnableOffsetBInfo, _T( "ImageArithmeticBEnableOffset" ) );
	// Offset Point
	inputOffsetBPointInfo.SetInputObjectType( SVOffsetBPointObjectGuid, SVPointValueObjectType );
	inputOffsetBPointInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputOffsetBPointInfo, _T( "ImageArithmeticBOffsetPoint" ) );

	// Operator Input...
	inputArithOperatorInfo.SetInputObjectType( SVArithmeticOperatorObjectGuid, SVLongValueObjectType );
	inputArithOperatorInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputArithOperatorInfo, _T( "ImageArithmeticOperator" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults

	outputImageObject.InitializeImage( SVImageTypePhysical );

	// Set up draw objects...
//	graphFigure.SetDrawPen( TRUE, PS_SOLID, 1, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImageArithmeticClass::~SVImageArithmeticClass()
{
	CloseObject();
}

BOOL SVImageArithmeticClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	bOk &= ( S_OK == outputImageObject.InitializeImage( getInputImageA() ) );

	// Reset Printable flag
	outputImageObject.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVImageArithmeticClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() && getOutputImage() )
	{
		BOOL bRetVal = true;
		return getOutputImage()->CloseObject() && bRetVal;
	}
	return false;
}

HRESULT SVImageArithmeticClass::ResetObject()
{
	HRESULT l_hrOk = outputImageObject.InitializeImage( getInputImageA() );

	CollectInputImageNames();

	if ( S_OK != SVTaskObjectClass::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}
	return l_hrOk;
}

HRESULT SVImageArithmeticClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage && ( p_psvImage == getInputImageA() || p_psvImage == getInputImageB() ) )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

SVImageClass* SVImageArithmeticClass::getInputImageA()
{
	if( inputImageAInfo.IsConnected() && inputImageAInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageAInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVBoolValueObjectClass* SVImageArithmeticClass::getInputEnableOffsetA()
{
	if( inputEnableOffsetAInfo.IsConnected() && inputEnableOffsetAInfo.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputEnableOffsetAInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVPointValueObjectClass* SVImageArithmeticClass::getInputOffsetAPoint()
{
	if( inputOffsetAPointInfo.IsConnected() && inputOffsetAPointInfo.GetInputObjectInfo().PObject )
		return ( SVPointValueObjectClass* ) inputOffsetAPointInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVImageClass* SVImageArithmeticClass::getInputImageB()
{
	if( inputImageBInfo.IsConnected() && inputImageBInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageBInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVBoolValueObjectClass* SVImageArithmeticClass::getInputEnableOffsetB()
{
	if( inputEnableOffsetBInfo.IsConnected() && inputEnableOffsetBInfo.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputEnableOffsetBInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVPointValueObjectClass* SVImageArithmeticClass::getInputOffsetBPoint()
{
	if( inputOffsetBPointInfo.IsConnected() && inputOffsetBPointInfo.GetInputObjectInfo().PObject )
		return ( SVPointValueObjectClass* ) inputOffsetBPointInfo.GetInputObjectInfo().PObject;

	return nullptr;
}

SVLongValueObjectClass* SVImageArithmeticClass::getInputArithOperator()
{
	if( inputArithOperatorInfo.IsConnected() && inputArithOperatorInfo.GetInputObjectInfo().PObject )
		return ( SVLongValueObjectClass* ) inputArithOperatorInfo.GetInputObjectInfo().PObject;

	return nullptr;
}
	
SVImageClass* SVImageArithmeticClass::getOutputImage()
{
	return &outputImageObject;
}

BOOL SVImageArithmeticClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVTaskObjectClass::onRun( RRunStatus ) )
	{
		SVImageClass*			 pImageA		= getInputImageA();
		ASSERT( pImageA );
		SVBoolValueObjectClass*  pEnableOffsetA = getInputEnableOffsetA();
		ASSERT( pEnableOffsetA );
		SVPointValueObjectClass* pOffsetAPoint  = getInputOffsetAPoint();
		ASSERT( pOffsetAPoint );
		SVImageClass*			 pImageB		= getInputImageB();
		ASSERT( pImageB );
		SVBoolValueObjectClass*  pEnableOffsetB	= getInputEnableOffsetB();
		ASSERT( pEnableOffsetB );
		SVPointValueObjectClass* pOffsetBPoint  = getInputOffsetBPoint();
		ASSERT( pOffsetBPoint );
		SVLongValueObjectClass*  pArithOperator = getInputArithOperator();
		ASSERT( pArithOperator );
		SVImageClass*            pOutputImage   = getOutputImage();
		ASSERT( pOutputImage );

    if ( ! pOutputImage->SetImageHandleIndex( RRunStatus.Images ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		BOOL bEnableOffsetA;
		if( S_OK != pEnableOffsetA->GetValue( bEnableOffsetA ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		POINT offsetA;
		if( S_OK != pOffsetAPoint->GetValue( offsetA ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		BOOL bEnableOffsetB;
		if( S_OK != pEnableOffsetB->GetValue( bEnableOffsetB ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		POINT offsetB;
		if( S_OK != pOffsetBPoint->GetValue( offsetB ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		long lOperator;
		if( S_OK != pArithOperator->GetValue( lOperator ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

    SVSmartHandlePointer HandleA;
    SVSmartHandlePointer HandleB;
    SVSmartHandlePointer Output;

		SVImageBufferHandleImage l_MilAHandle;
		SVImageBufferHandleImage l_MilBHandle;
		SVImageBufferHandleImage l_MilOutHandle;

    if ( pImageA->GetImageHandle( HandleA ) && !( HandleA.empty() ) &&
         pImageB->GetImageHandle( HandleB ) && !( HandleB.empty() ) &&
         pOutputImage->GetImageHandle( Output ) && !( Output.empty() ) )
    {
			HandleA->GetData( l_MilAHandle );
			HandleB->GetData( l_MilBHandle );
			Output->GetData( l_MilOutHandle );
		}

		if( !( l_MilAHandle.empty() ) &&  !( l_MilBHandle.empty() ) && !( l_MilOutHandle.empty() ) )
		{
		  SVMatroxImageInterface::SVStatusCode l_Code;

		  // Filter for special image arithmetic operators...
		  // e.g. SV_IMGOP_AVERAGE ( not defined by MIL )
		  switch( lOperator )
		  {
			  case SV_IMGOP_AVERAGE:
			  {

					l_Code = SVMatroxImageInterface::Arithmetic( l_MilOutHandle.GetBuffer(), l_MilAHandle.GetBuffer(), l_MilBHandle.GetBuffer(), SVImageMultipleAccumulate );
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

			  case SV_IMGOP_FLIP_VERTICAL:
			  {
				  l_Code = SVMatroxImageInterface::Flip( l_MilOutHandle.GetBuffer(), l_MilAHandle.GetBuffer(), SVImageFlipVertical );
			  }
			  break;

			  case SV_IMGOP_FLIP_HORIZONTAL:
			  {
				  l_Code = SVMatroxImageInterface::Flip( l_MilOutHandle.GetBuffer(), l_MilAHandle.GetBuffer(), SVImageFlipHorizontal );
			  }
			  break;


			  case SV_IMGOP_DOUBLE_HEIGHT:
			  {
				  ScaleWithAveraging( pImageA, pOutputImage );
			  }
			  break;


			  default:
				  // Default Operation,,,
				  l_Code = SVMatroxImageInterface::Arithmetic( l_MilOutHandle.GetBuffer(), l_MilAHandle.GetBuffer(), l_MilBHandle.GetBuffer(),
						static_cast<SVImageOperationTypeEnum>(lOperator) );
		  }

  		return TRUE;
    }
	}
	return FALSE;
}

BOOL SVImageArithmeticClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputImageA() && getInputEnableOffsetA() && getInputOffsetAPoint() &&
		getInputImageB() && getInputEnableOffsetB() && getInputOffsetBPoint() &&
		getInputArithOperator() 
	  )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}


bool SVImageArithmeticClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );
	
	return( __super::resetAllObjects( shouldNotifyFriends, silentReset) && Result );
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

		SVSmartHandlePointer InputImageBufferHandle;
		SVSmartHandlePointer OutputImageBufferHandle;

		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RECT l_oInputRect;
		RECT l_oOutputRect;
		
		if( pInputImage->GetImageHandle( InputImageBufferHandle ) && !( InputImageBufferHandle.empty() ) &&
			pOutputImage->GetImageHandle( OutputImageBufferHandle ) && !( OutputImageBufferHandle.empty() ) &&
			S_OK == InputImageInfo.GetOutputRectangle( l_oInputRect ) &&
			S_OK == OutputImageInfo.GetOutputRectangle( l_oOutputRect ) )
		{
			pSrcHostBuffer = InputImageBufferHandle->GetBufferAddress();
			pDstHostBuffer = OutputImageBufferHandle->GetBufferAddress();

			InputImageBufferHandle->GetData( l_InMilHandle );
			OutputImageBufferHandle->GetData( l_OutMilHandle );
		}
		
		if( pSrcHostBuffer && pDstHostBuffer && !( l_InMilHandle.empty() ) && !( l_OutMilHandle.empty() ) )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;
			
			if( l_oOutputRect.bottom && l_oInputRect.bottom )
			{
				l_Code = SVMatroxBufferInterface::Get( l_InMilHandle.GetBuffer(), SVPitch, srcPitch );
				l_Code = SVMatroxBufferInterface::Get( l_OutMilHandle.GetBuffer(), SVPitch, dstPitch );
				
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
	SVImageClass* l_pInputImage = getInputImageA();
	SVImageToolClass* pTool = dynamic_cast<SVImageToolClass*>(GetTool());
	if (nullptr != pTool)
	{
		SVStaticStringValueObjectClass* imageNames = pTool->GetInputImageNames();
		if( l_pInputImage && nullptr != imageNames )
		{
			CString l_strName = l_pInputImage->GetCompleteObjectName();

			imageNames->SetValue( 0/*Static value, this parameter will not used*/, 0, l_strName );

			l_hr = S_OK;
		}

		l_pInputImage = getInputImageB();
		if( l_pInputImage && nullptr != imageNames)
		{
			CString l_strName = l_pInputImage->GetCompleteObjectName();

			imageNames->SetValue( 0/*Static value, this parameter will not used*/, 1, l_strName );
		}
	}
	return l_hr;
}

