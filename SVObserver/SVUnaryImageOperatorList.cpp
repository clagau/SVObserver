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

#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"
#include "SVMatroxImageProcessingClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;

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

SVUnaryImageOperatorListClass::~SVUnaryImageOperatorListClass() 
{ 
	CloseObject();
}

BOOL SVUnaryImageOperatorListClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectListClass::CreateObject( PCreateStructure ) )
	{
		bOk = GetTool() && GetInspection() && getInputImage();
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVUnaryImageOperatorListClass::CloseObject()
{
	return SVTaskObjectListClass::CloseObject();
}

HRESULT SVUnaryImageOperatorListClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage && p_psvImage == getInputImage() )
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

	return nullptr;
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
