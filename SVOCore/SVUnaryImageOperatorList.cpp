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
#include "SVObjectLibrary/SVClsids.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
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
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVSkeletonFilterObjectType;
	filterClassInfo.m_ClassId = SVSkeletonFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVSKELETONFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Thinning Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVThinningFilterObjectType;
	filterClassInfo.m_ClassId = SVThinningFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTHINNINGFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Thickening Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVThickeningFilterObjectType;
	filterClassInfo.m_ClassId = SVThickeningFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTHICKENINGFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Watershed Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVWatershedFilterObjectType;
	filterClassInfo.m_ClassId = SVWatershedFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVWATERSHEDFILTER );
	m_availableChildren.Add( filterClassInfo );
	
	// Add the Custom Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVCustomFilterObjectType;
	filterClassInfo.m_ClassId = SVCustomFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVCUSTOMFILTER );
	m_availableChildren.Add( filterClassInfo );
	
	// Add the Custom2 Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVCustom2FilterObjectType;
	filterClassInfo.m_ClassId = Custom2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_CUSTOM2FILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Ranking Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVRankingFilterObjectType;
	filterClassInfo.m_ClassId = SVRankingFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVRANKINGFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Smooth Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVSmoothFilterObjectType;
	filterClassInfo.m_ClassId = SVSmoothFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVSMOOTHFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Sharpen Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVSharpenFilterObjectType;
	filterClassInfo.m_ClassId = SVSharpenFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVSHARPENFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Sharpen2 Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVSharpen2FilterObjectType;
	filterClassInfo.m_ClassId = SVSharpen2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVSHARPEN2FILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Opening Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVOpeningFilterObjectType;
	filterClassInfo.m_ClassId = SVOpeningFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVOPENINGFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Closing Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVClosingFilterObjectType;
	filterClassInfo.m_ClassId = SVClosingFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVCLOSINGFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Dilation Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVDilationFilterObjectType;
	filterClassInfo.m_ClassId = SVDilationFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVDILATIONFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Erosion Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVErosionFilterObjectType;
	filterClassInfo.m_ClassId = SVErosionFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVEROSIONFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVEdgeDetectFilterObjectType;
	filterClassInfo.m_ClassId = SVEdgeDetectFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVEDGEDETECTFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Edge Detect2 Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVEdgeDetect2FilterObjectType;
	filterClassInfo.m_ClassId = SVEdgeDetect2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVEDGEDETECT2FILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Vert Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVVertEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SVVertEdgeFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVVERTEDGEFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Horiz Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVHorzEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SVHorizEdgeFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVHORIZEDGEFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Laplacian Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVLaplacianEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SVLaplacianEdgeFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLAPLACIANEDGEFILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Laplacian 2 Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVLaplacianEdge2FilterObjectType;
	filterClassInfo.m_ClassId = SVLaplacianEdge2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLAPLACIANEDGE2FILTER );
	m_availableChildren.Add( filterClassInfo );

	// Add the Max Minus Min Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SVMaxMinusMinFilterObjectType;
	filterClassInfo.m_ClassId = SVMaxMinusMinFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVMAXMINUSMINFILTER );
	m_availableChildren.Add( filterClassInfo );
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

bool SVUnaryImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (!inputImageObjectInfo.IsConnected() || nullptr == inputImageObjectInfo.GetInputObjectInfo().PObject)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;

}
