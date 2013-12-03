//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageArithmetic
//* .File Name       : $Workfile:   SVImageArithmetic.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageArithmetic.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVTool.h"
#include "SVToolImage.h"


SV_IMPLEMENT_CLASS( SVImageArithmeticClass, SVImageArithmeticClassGuid )

SVImageArithmeticClass::SVImageArithmeticClass( SVObjectClass* POwner, int StringResourceID )
					   :SVTaskObjectClass( POwner, StringResourceID ) 
{

	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageArithmeticObjectType;

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

	bOk &= ( outputImageObject.InitializeImage( getInputImageA() ) == S_OK );

	// Reset Printable flag
	outputImageObject.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;


	isCreated = bOk;

	return bOk;
}

BOOL SVImageArithmeticClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() && getOutputImage() )
	{
		BOOL bRetVal = TRUE;
		return getOutputImage()->CloseObject() && bRetVal;
	}
	return FALSE;
}

HRESULT SVImageArithmeticClass::ResetObject()
{
	HRESULT l_hrOk = outputImageObject.InitializeImage( getInputImageA() );

	if ( SVTaskObjectClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVImageArithmeticClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL && ( p_psvImage == getInputImageA() || p_psvImage == getInputImageB() ) )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

SVImageClass* SVImageArithmeticClass::getInputImageA()
{
	if( inputImageAInfo.IsConnected() && inputImageAInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageAInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVBoolValueObjectClass* SVImageArithmeticClass::getInputEnableOffsetA()
{
	if( inputEnableOffsetAInfo.IsConnected() && inputEnableOffsetAInfo.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputEnableOffsetAInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVPointValueObjectClass* SVImageArithmeticClass::getInputOffsetAPoint()
{
	if( inputOffsetAPointInfo.IsConnected() && inputOffsetAPointInfo.GetInputObjectInfo().PObject )
		return ( SVPointValueObjectClass* ) inputOffsetAPointInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVImageArithmeticClass::getInputImageB()
{
	if( inputImageBInfo.IsConnected() && inputImageBInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageBInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVBoolValueObjectClass* SVImageArithmeticClass::getInputEnableOffsetB()
{
	if( inputEnableOffsetBInfo.IsConnected() && inputEnableOffsetBInfo.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputEnableOffsetBInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVPointValueObjectClass* SVImageArithmeticClass::getInputOffsetBPoint()
{
	if( inputOffsetBPointInfo.IsConnected() && inputOffsetBPointInfo.GetInputObjectInfo().PObject )
		return ( SVPointValueObjectClass* ) inputOffsetBPointInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVLongValueObjectClass* SVImageArithmeticClass::getInputArithOperator()
{
	if( inputArithOperatorInfo.IsConnected() && inputArithOperatorInfo.GetInputObjectInfo().PObject )
		return ( SVLongValueObjectClass* ) inputArithOperatorInfo.GetInputObjectInfo().PObject;

	return NULL;
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

		CollectInputImageNames(RRunStatus);

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


DWORD SVImageArithmeticClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

void SVImageArithmeticClass::ScaleWithAveraging( SVImageClass* pInputImage, SVImageClass* pOutputImage )
{
	if( pInputImage && pOutputImage )
	{
		LPVOID pSrcHostBuffer = NULL;
		LPVOID pDstHostBuffer = NULL;
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
			InputImageInfo.GetOutputRectangle( l_oInputRect ) == S_OK &&
			OutputImageInfo.GetOutputRectangle( l_oOutputRect ) == S_OK )
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
HRESULT SVImageArithmeticClass::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImageA();
	SVImageToolClass* l_pTool = dynamic_cast<SVImageToolClass*>(GetTool());
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		l_pTool->m_svSourceImageNames.SetValue( RRunStatus.m_lResultDataIndex, 0, l_strName );

		l_hr = S_OK;
	}

	l_pInputImage = getInputImageB();
	if( l_pInputImage && l_pTool)
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		l_pTool->m_svSourceImageNames.SetValue( RRunStatus.m_lResultDataIndex, 1, l_strName );
	}
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageArithmetic.cpp_v  $
 * 
 *    Rev 1.2   01 Oct 2013 14:19:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   02 May 2013 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:49:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   25 Jul 2012 14:10:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   12 Jul 2012 14:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   19 Jun 2012 12:47:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   30 Mar 2011 15:08:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   11 Mar 2011 14:22:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   25 Feb 2011 12:10:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   10 Feb 2011 14:45:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   27 Jan 2011 11:20:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   04 Nov 2010 13:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   01 Jun 2010 14:24:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   16 Dec 2009 11:00:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   07 Mar 2008 11:55:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for Transfer Image Definition List Source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   24 Jul 2007 13:06:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   02 Nov 2005 07:33:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   21 Jun 2005 08:07:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   17 Feb 2005 14:25:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   16 Feb 2005 14:29:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   06 Jan 2004 14:01:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  391
 * SCR Title:  Add horizontal and vertical flip for Image Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed two small bugs in the flip code for the Image Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   29 Oct 2003 14:08:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  391
 * SCR Title:  Add horizontal and vertical flip for Image Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new operations for flip horizontal and flip vertical
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   22 Apr 2003 10:07:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   29 Jan 2003 16:24:56   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, onUpdateFigure, onRun, processMessage and ScaleWithAveraging methods to use new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   10 Jan 2003 14:13:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * Added method to return the overlay figure from this object.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   09 Dec 2002 19:24:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   19 Nov 2002 13:22:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 *            GetIPDoc
 *            Changed method of access
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   02 Oct 2001 15:05:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified functions, SVImageArithmeticClass::CreateObject, 
 * SVImageArithmeticClass::onDraw.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   19 Apr 2001 14:23:14   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   19 May 2000 18:08:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ScaleWithAveraging to get the correct source image buffer address.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   19 May 2000 15:58:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved SV_IMGOP_AVERAGE from SVObserver.h.
 * Added SV_IMG_OP_DOUBLE_HEIGHT define.
 * Added ScaleWithAveraging method.
 * Revised OnRun to call ScaleWithAveraging if operator is
 * SV_IMGOP_DOUBLE_HEIGHT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   14 Apr 2000 10:54:24   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  120
 * SCR Title:  Pattern Analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added the function 'checkDrawAllowed' to SVImageArithmeticClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   03 Mar 2000 12:57:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVImageInfoClass methods to get/set image extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   25 Feb 2000 16:04:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onDraw to call pImage->GetTransform() to get contextual transformation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Feb 2000 14:36:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added logic in CreateObject to set Display Input Image Context.
 * Revised onDraw to call checkDrawAllowed().
 * Revised onUpdateFigure() to use relative extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 16:57:16   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   08 Feb 2000 10:59:32   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the onDraw function to use the current scaling
 * factor (lZoomFactor) when drawing an object.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:46:04   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Nov 17 1999 15:04:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO changes for image arithmetic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 13 1999 13:05:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides Image Tool and Image arithmetic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
