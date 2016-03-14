//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUnaryImageOperatorList
//* .File Name       : $Workfile:   SVUnaryImageOperatorList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   24 Oct 2014 11:50:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUnaryImageOperatorList.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"
#include "SVMatroxImageProcessingClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVUnaryImageOperatorListClass::SVUnaryImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							  :SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVUnaryImageOperatorListClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "UnaryImageOperatorListImage" ) );

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();

	// Set available Filters
	// Populate the available filter list
	SVClassInfoStruct filterClassInfo;
	
	// Add the Skeleton Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVSkeletonFilterObjectType;
	filterClassInfo.ClassId = SVSkeletonFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVSKELETONFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Thinning Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVThinningFilterObjectType;
	filterClassInfo.ClassId = SVThinningFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVTHINNINGFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Thickening Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVThickeningFilterObjectType;
	filterClassInfo.ClassId = SVThickeningFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVTHICKENINGFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Watershed Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVWatershedFilterObjectType;
	filterClassInfo.ClassId = SVWatershedFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVWATERSHEDFILTER );
	availableChildren.Add( filterClassInfo );
	
	// Add the Custom Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVCustomFilterObjectType;
	filterClassInfo.ClassId = SVCustomFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVCUSTOMFILTER );
	availableChildren.Add( filterClassInfo );
	
	// Add the Custom2 Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVCustom2FilterObjectType;
	filterClassInfo.ClassId = Custom2FilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_CUSTOM2FILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Ranking Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVRankingFilterObjectType;
	filterClassInfo.ClassId = SVRankingFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVRANKINGFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Smooth Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVSmoothFilterObjectType;
	filterClassInfo.ClassId = SVSmoothFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVSMOOTHFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Sharpen Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVSharpenFilterObjectType;
	filterClassInfo.ClassId = SVSharpenFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVSHARPENFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Sharpen2 Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVSharpen2FilterObjectType;
	filterClassInfo.ClassId = SVSharpen2FilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVSHARPEN2FILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Opening Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVOpeningFilterObjectType;
	filterClassInfo.ClassId = SVOpeningFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVOPENINGFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Closing Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVClosingFilterObjectType;
	filterClassInfo.ClassId = SVClosingFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVCLOSINGFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Dilation Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVDilationFilterObjectType;
	filterClassInfo.ClassId = SVDilationFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVDILATIONFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Erosion Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVErosionFilterObjectType;
	filterClassInfo.ClassId = SVErosionFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEROSIONFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Edge Detect Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVEdgeDetectFilterObjectType;
	filterClassInfo.ClassId = SVEdgeDetectFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGEDETECTFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Edge Detect2 Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVEdgeDetect2FilterObjectType;
	filterClassInfo.ClassId = SVEdgeDetect2FilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGEDETECT2FILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Vert Edge Detect Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVVertEdgeFilterObjectType;
	filterClassInfo.ClassId = SVVertEdgeFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVVERTEDGEFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Horiz Edge Detect Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVHorzEdgeFilterObjectType;
	filterClassInfo.ClassId = SVHorizEdgeFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVHORIZEDGEFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Laplacian Edge Detect Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVLaplacianEdgeFilterObjectType;
	filterClassInfo.ClassId = SVLaplacianEdgeFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLAPLACIANEDGEFILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Laplacian 2 Edge Detect Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVLaplacianEdge2FilterObjectType;
	filterClassInfo.ClassId = SVLaplacianEdge2FilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLAPLACIANEDGE2FILTER );
	availableChildren.Add( filterClassInfo );

	// Add the Max Minus Min Filter
	filterClassInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.ObjectTypeInfo.SubType = SVMaxMinusMinFilterObjectType;
	filterClassInfo.ClassId = SVMaxMinusMinFilterClassGuid;
	filterClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXMINUSMINFILTER );
	availableChildren.Add( filterClassInfo );
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVUnaryImageOperatorListClass::~SVUnaryImageOperatorListClass() 
{ 
	CloseObject();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :31.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVUnaryImageOperatorListClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectListClass::CreateObject( PCreateStructure ) )
	{
		bOk = GetTool() && GetInspection() && getInputImage();
	}

	isCreated = bOk;

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CloseObject
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :31.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVUnaryImageOperatorListClass::CloseObject()
{
	return SVTaskObjectListClass::CloseObject();
}



HRESULT SVUnaryImageOperatorListClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL && p_psvImage == getInputImage() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getInputImage
// -----------------------------------------------------------------------------
// .Description : Returns Input Image Attribute of this class
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :31.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVImageClass* SVUnaryImageOperatorListClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : Special routing.
//				: NOTE
//				: Since we have here a special routing for Run(...), and the 
//				: image operators are not calling their base class onRun(...), 
//				: we have to do a complete validation 
//				: ( including children validation !!!).
//				: So we call Validate() of Task Object List instead of 
//				: base class OnValidate() !!!
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :31.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVUnaryImageOperatorListClass::OnValidate()
{
	BOOL bRetVal = SVTaskObjectListClass::OnValidate();

	bRetVal = bRetVal && IsCreated();
	bRetVal = bRetVal && inputImageObjectInfo.IsConnected();
	bRetVal = bRetVal && inputImageObjectInfo.GetInputObjectInfo().PObject;

	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

















//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVStdImageOperatorListClass
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVStdImageOperatorListClass, SVStdImageOperatorListClassGuid )


SVStdImageOperatorListClass::SVStdImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							:SVUnaryImageOperatorListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVStdImageOperatorListClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	outObjectInfo.ObjectTypeInfo.SubType	= SVStdImageOperatorListObjectType;

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults
	outputImageObject.InitializeImage( SVImageTypeLogicalAndPhysical );

	// Set default inputs and outputs
	addDefaultInputObjects();
}


SVStdImageOperatorListClass::~SVStdImageOperatorListClass() 
{ 
	CloseObject();

}

BOOL SVStdImageOperatorListClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = SVUnaryImageOperatorListClass::CreateObject( PCreateStructure );

	// Image input must already exist, and must be created!!!
	
	// Embedded Image output must already exist!!!
	l_bOk &= ( outputImageObject.InitializeImage( getInputImage() ) == S_OK );

	isCreated = l_bOk;

	return l_bOk;
}

BOOL SVStdImageOperatorListClass::CloseObject()
{
	BOOL bRetVal = TRUE;
	
	if( getOutputImage() )
	{
		bRetVal = getOutputImage()->CloseObject() && bRetVal;
	}

	if ( isCreated )
	{
		isCreated = FALSE;
		
		bRetVal = SVUnaryImageOperatorListClass::CloseObject() && bRetVal;
	}

	return bRetVal;
}



HRESULT SVStdImageOperatorListClass::ResetObject()
{
	HRESULT l_hrOk = outputImageObject.InitializeImage( getInputImage() );
	
	if ( SVUnaryImageOperatorListClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	CollectInputImageNames();

	//create tmp mil buffer for operator
	SVImageClass* pImage = getOutputImage();
	if (pImage)
	{
		SVImageInfoClass imageInfo = pImage->GetImageInfo();
		imageInfo.setDibBufferFlag(false);
		SVMatroxImageProcessingClass::Instance().CreateImageBuffer(imageInfo, m_milTmpImageObjectInfo1);
		SVMatroxImageProcessingClass::Instance().CreateImageBuffer(imageInfo, m_milTmpImageObjectInfo2);
	}

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getOutputImage
// -----------------------------------------------------------------------------
// .Description : Returns Output Image Attribute of this class
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVImageClass* SVStdImageOperatorListClass::getOutputImage()
{
	return &outputImageObject;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Special routing here.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVStdImageOperatorListClass::Run( SVRunStatusClass& RRunStatus )
{
	BOOL bRetVal = true;

	SVSmartHandlePointer input;
	SVSmartHandlePointer output;
	
	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = RRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

	SVDataManagerHandle dmHandleInput;
	
	// Run yourself...
	bRetVal = onRun( RRunStatus );

	SVImageClass* pInputImage = getInputImage();
	SVImageClass* pOutputImage = getOutputImage();
	
	bRetVal = bRetVal && ( nullptr != pInputImage );
	bRetVal = bRetVal && ( nullptr != pOutputImage );

	if( bRetVal )
	{
		bRetVal = pOutputImage->SetImageHandleIndex( RRunStatus.Images );

		if ( bRetVal )
		{
			// Check for new image type...
			if( nullptr == pOutputImage->GetParentImage() )
			{
				// Use just the image input of this unary image operator list...
				pInputImage->GetImageHandle( input );
			}
			else
			{
				// Old image types...( i.e. 'S', used in Window Tool's Unary Image Operator List )
				
				// NOTE: 
				// The old image type 'S' provided also a child layer in his derived image info!!!
				
				// Use the Child layer on Input Image as our input image
				if( nullptr != GetTool() && pOutputImage->GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
				{
					GetTool()->UpdateImageWithExtent( RRunStatus.m_lResultDataIndex );
				}

				pOutputImage->GetParentImageHandle( input );
			}

			pOutputImage->GetImageHandle( output );

			bRetVal = bRetVal && !( input.empty() );
			bRetVal = bRetVal && !( output.empty() );

			if( input.empty() )
			{
				SVImageProcessingClass::Instance().InitBuffer( output );

				input = output;
			}

			//set tmp variable
			SVSmartHandlePointer sourceImage = m_milTmpImageObjectInfo1;
			SVSmartHandlePointer destinationImage = m_milTmpImageObjectInfo2;
			bRetVal = bRetVal && !( sourceImage->empty() );
			bRetVal = bRetVal && !( destinationImage->empty() );
			bRetVal = bRetVal && copyBuffer( input, sourceImage );

			if (bRetVal)
			{
				// Run children...
				for( int i = 0; i < GetSize(); i++ )
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();

					SVUnaryImageOperatorClass*  pOperator = ( SVUnaryImageOperatorClass* )GetAt( i );

					bRetVal &= ( nullptr != pOperator );

					if( nullptr != pOperator )
					{
						if( pOperator->Run( true, sourceImage, destinationImage, ChildRunStatus ) )
						{
							//switch image buffer for next run
							SVSmartHandlePointer tmpImage = sourceImage;
							sourceImage = destinationImage;
							destinationImage = tmpImage;
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

					// Update our Run Status
					if( ChildRunStatus.IsDisabled() )
						RRunStatus.SetDisabled();

					if( ChildRunStatus.IsDisabledByCondition() )
						RRunStatus.SetDisabledByCondition();

					if( ChildRunStatus.IsWarned() )
						RRunStatus.SetWarned();

					if( ChildRunStatus.IsFailed() )
						RRunStatus.SetFailed();

					if( ChildRunStatus.IsPassed() )
						RRunStatus.SetPassed();

					if( ChildRunStatus.IsCriticalFailure() )
						RRunStatus.SetCriticalFailure();
				}
			} // for( int i = 0; i < GetSize(); i++ )
			
			// 'no operator was running' check...
			// RO_22Mar2000
			
			if( bRetVal )
			{
				bRetVal = copyBuffer(sourceImage, output);
			} // if( bFirstFlag ) 
		}
	}
	
	if( ! bRetVal )
	{
		// Something was wrong...
		
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	return bRetVal;
}

BOOL SVStdImageOperatorListClass::OnValidate()
{
	if( SVUnaryImageOperatorListClass::OnValidate() )
	{
		if( outputImageObject.OnValidate() ) 
		{
			return TRUE;
		}
	}

	SetInvalid();
	return FALSE;
}

DWORD_PTR SVStdImageOperatorListClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

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

	return( SVUnaryImageOperatorListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) );
}

// Set String value object for Source Image Names
HRESULT SVStdImageOperatorListClass::CollectInputImageNames()
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVToolClass* l_pTool = dynamic_cast<SVToolClass*>( GetTool() );
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		SVStaticStringValueObjectClass* l_lstrVO = l_pTool->GetInputImageNames();
		if( l_lstrVO )
		{
			l_lstrVO->SetValue( 0/*Static value, this parameter will not used*/, l_strName );
		}

		l_hr = S_OK;
	}
	return l_hr;
}

bool SVStdImageOperatorListClass::copyBuffer( const SVSmartHandlePointer input, SVSmartHandlePointer output )
{
	bool bRetVal = true;
	SVImageBufferHandleImage sourceMilHandle;
	SVImageBufferHandleImage destinationMilHandle;

	input->GetData( sourceMilHandle );
	output->GetData( destinationMilHandle );

	bRetVal = bRetVal && !( sourceMilHandle.empty() );
	bRetVal = bRetVal && !( destinationMilHandle.empty() );

	if( bRetVal )
	{
		SVMatroxBufferInterface::SVStatusCode  l_Code = SVMatroxBufferInterface::CopyBuffer( destinationMilHandle.GetBuffer(), sourceMilHandle.GetBuffer() );
		bRetVal = (SVMEE_STATUS_OK == l_Code);
	}	
	
	return bRetVal;
}













//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVInPlaceImageOperatorListClass
//* Note(s)    : Unary image operator list 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVInPlaceImageOperatorListClass, SVInPlaceImageOperatorListClassGuid )


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInPlaceImageOperatorListClass
// -----------------------------------------------------------------------------
// .Description : Standard constructor
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVInPlaceImageOperatorListClass::SVInPlaceImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							    :SVUnaryImageOperatorListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVInPlaceImageOperatorListClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	outObjectInfo.ObjectTypeInfo.SubType	= SVInPlaceImageOperatorListObjectType;

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();

}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVInPlaceImageOperatorListClass::~SVInPlaceImageOperatorListClass() 
{ 
	CloseObject();

}

HRESULT SVInPlaceImageOperatorListClass::IsInputImage( SVImageClass *p_psvImage )
{
	return S_FALSE;
}

//******************************************************************************
// Operator(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : Validates base class and checks, 
//				: if input image is a physical type.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVInPlaceImageOperatorListClass::OnValidate()
{
	if( SVUnaryImageOperatorListClass::OnValidate() )
	{
		if( getInputImage() != NULL && getInputImage()->GetImageType() == SVImageTypePhysical )
		{
			return TRUE;
		}
	}

	SetInvalid();
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Is doing in place processing on input image!!!
//				:
//				: Special routing for image operators here.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVInPlaceImageOperatorListClass::Run( SVRunStatusClass& RRunStatus )
{
	SVSmartHandlePointer input;
	SVSmartHandlePointer output;

	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = RRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

	// Run yourself...
	BOOL bRetVal = onRun( RRunStatus );

	if( ! getInputImage() )
	{
		// Signal something is wrong...
		bRetVal = FALSE;
	}

	if( bRetVal )
	{
		// Use input image for in- and output.
		// Image must be a Physical type!!! ( Is already checked in OnValidate() )
		getInputImage()->GetImageHandle( input );
		getInputImage()->GetImageHandle( output );
		
		bool bFirstFlag = true;

		// Run children...
		for( int i = 0; i < GetSize(); i++ )
		{
			ChildRunStatus.ResetRunStateAndToolSetTimes();

			SVObjectClass* pObject = GetAt( i );
			
			// Do safe type casting...
			SVUnaryImageOperatorClass* pOperator = dynamic_cast< SVUnaryImageOperatorClass* >( pObject );

			if( pOperator )
			{
				if( pOperator->Run( bFirstFlag, input, output, ChildRunStatus ) )
				{
					// Switch first flag off ( FALSE )...
					bFirstFlag = false;

					// NOTE:
					// If operator returns FALSE, he was not running ( may be deactivated )
					// So, don't switch first flag off, so that a following operator knows
					// he is the first one or the 'no operator was running' check can do his job!!!
					// RO_22Mar2000

					// WARNING:
					// Do not set bRetVal automatically to FALSE, if operator was not running !!!
					// ChildRunStatus keeps information about, if an error occured while running !!!

					// NOTE: 
					// Since we do here an in place process, that means input and output image is
					// the same, bFirstFlag is not really necessary, but supported for sanity.
					// But if no operator exists, we don't have to copy input to output anymore!
				}
			}
			else
				bRetVal = FALSE;

			// Update our Run Status
			if( ChildRunStatus.IsDisabled() )
				RRunStatus.SetDisabled();

			if( ChildRunStatus.IsDisabledByCondition() )
				RRunStatus.SetDisabledByCondition();
			
			if( ChildRunStatus.IsWarned() )
				RRunStatus.SetWarned();

			if( ChildRunStatus.IsFailed() )
				RRunStatus.SetFailed();

			if( ChildRunStatus.IsPassed() )
				RRunStatus.SetPassed();

			if( ChildRunStatus.IsCriticalFailure() )
				RRunStatus.SetCriticalFailure();
		}
  }
	
	if( ! bRetVal )
	{
		// Something was wrong...

		SetInvalid();
		RRunStatus.SetInvalid();
	}

	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return bRetVal;
}








