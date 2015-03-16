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

#include "stdafx.h"
#include "SVUnaryImageOperatorList.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"
#include "SVMatroxImageProcessingClass.h"

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
	outputImageObject.InitializeImage( SVImageTypeLogical );

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

	//create tmp mil buffer for operator
	SVImageClass* pImage = getOutputImage();
	if (pImage)
	{
		SVImageInfoClass imageInfo = pImage->GetImageInfo();
		SVMatroxImageProcessingClass::Instance().CreateImageMilBuffer(imageInfo, m_milTmpImageObjectInfo1);
		SVMatroxImageProcessingClass::Instance().CreateImageMilBuffer(imageInfo, m_milTmpImageObjectInfo2);
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
			CollectInputImageNames(RRunStatus);

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
					ChildRunStatus.ClearAll();

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
			}
			
			// 'no operator was running' check...
			// RO_22Mar2000
			
			if( bRetVal )
			{
				bRetVal = copyBuffer(sourceImage, output);
			}
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
HRESULT SVStdImageOperatorListClass::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	HRESULT l_hr = S_FALSE;
	SVImageClass* l_pInputImage = getInputImage();
	SVToolClass* l_pTool = dynamic_cast<SVToolClass*>( GetTool() );
	if( l_pInputImage && l_pTool )
	{
		CString l_strName = l_pInputImage->GetCompleteObjectName();

		SVStringValueObjectClass* l_lstrVO=NULL;
		l_pTool->GetInputImageNames( l_lstrVO );
		if( l_lstrVO )
		{
			l_lstrVO->SetValue( RRunStatus.m_lResultDataIndex, 0, l_strName );
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
			ChildRunStatus.ClearAll();

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








//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUnaryImageOperatorList.cpp_v  $
 * 
 *    Rev 1.5   24 Oct 2014 11:50:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   added Custom2 Filter to the filter types
 * 
 * Method Changed: init
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 15:06:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 12:26:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Aug 2013 10:08:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVUnaryImageOperatorClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:37:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   02 May 2013 11:25:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:53:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   18 Jul 2012 14:27:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   12 Jul 2012 15:12:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   19 Jun 2012 14:28:20   jspila
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
 *    Rev 1.34   11 Apr 2011 18:55:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated child run status objects with update counters flag
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   05 Apr 2011 12:31:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix automatic moving/resizing issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   04 Apr 2011 11:45:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle issues with logical buffer resizing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   11 Mar 2011 14:49:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   25 Feb 2011 12:27:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   10 Feb 2011 15:17:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   27 Jan 2011 12:04:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   08 Dec 2010 13:48:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   04 Nov 2010 14:18:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   29 Jun 2010 14:08:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   01 Jun 2010 15:19:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   05 Mar 2010 11:01:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element to run status for critical errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   16 Dec 2009 13:28:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   30 Jul 2009 13:12:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   09 Apr 2008 08:55:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   11 Mar 2008 08:57:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added CollectInputImageNames to Run
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Jul 2007 07:58:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17.3.0   14 Jul 2009 15:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects and image methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17.2.0   07 Apr 2008 09:40:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   20 Mar 2006 13:57:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  547
 * SCR Title:  Fix Image overlay data
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed overlays issue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   02 Nov 2005 07:33:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 May 2005 15:01:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Feb 2005 14:18:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to support new Extents and new Reset Methodology. 
 * Added ResetObject(
 * Added OnValidate(
 * Added GetOutputImage(
 * Removed Unused Code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Feb 2005 15:25:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Sep 2004 09:57:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add new Filters to List in SVUnaryImageOperatorListClass::init()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Nov 2003 14:40:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Nov 2003 14:12:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Sep 2003 15:30:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   checked MIL handles before making call
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Aug 2003 12:04:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   modified the CloseObject method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jul 2003 10:15:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 17:18:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Jan 2003 09:36:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Run, OnValidate, CloseObject, CreateObject and processMessage methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Dec 2002 20:17:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Nov 2002 14:51:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed GetSystem & GetSystemInfo
 * Removed include SVSystem.h
 * Added SVObserver.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 20:00:28   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Apr 2000 17:00:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreateObject method to correct problem with profile tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:55:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides SVUnaryImageOperatorList base class and
 * SVStdUnaryImageOperatorList (replaces old SVUnaryImageOperatorList)
 * SVInPlaceImageOperatorList (has no image ouput,
 * performs inplace processing on input image(.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
