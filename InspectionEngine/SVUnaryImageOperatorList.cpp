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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVUnaryImageOperatorListClass::SVUnaryImageOperatorListClass( SVObjectClass* POwner, int StringResourceID )
							  :SVTaskObjectListClass( POwner, StringResourceID ) 
{
	init();
}

SVUnaryImageOperatorListClass::~SVUnaryImageOperatorListClass() 
{ 
	CloseObject();
}

bool SVUnaryImageOperatorListClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectListClass::CreateObject(rCreateStructure) && nullptr != GetTool() && nullptr != GetInspection() && nullptr != getInputImage();

	return m_isCreated;
}

SVImageClass* SVUnaryImageOperatorListClass::getInputImage(bool bRunMode /*=false*/) const
{
	return SvOl::getInput<SVImageClass>(m_inputImageObjectInfo, bRunMode);
}

bool SVUnaryImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_inputImageObjectInfo);

	SVImageClass* pInputImage = getInputImage();
	if (nullptr != pInputImage)
	{
		//! Check that the image color type is correct
		if (pInputImage->GetObjectSubType() != m_inputImageObjectInfo.GetInputObjectInfo().m_ObjectTypeInfo.SubType)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongInputImageType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;

}

#pragma region Protected Methods
bool SVUnaryImageOperatorListClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result(false);

	SVImageClass* pImage = getInputImage();
	if (nullptr != pImage && rImageGuid == pImage->GetUniqueObjectID())
	{
		Result = true;
	}

	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVUnaryImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVUnaryImageOperatorListObjectType;

	// Identify our input type needs
	m_inputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_inputImageObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_inputImageObjectInfo, _T("UnaryImageOperatorListImage"));

	// Register Embedded Objects

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();

	// Set available Filters
	// Populate the available filter list
	SVClassInfoStruct filterClassInfo;

	// Add the Skeleton Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVSkeletonFilterObjectType;
	filterClassInfo.m_ClassId = SVSkeletonFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSKELETONFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Thinning Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVThinningFilterObjectType;
	filterClassInfo.m_ClassId = SVThinningFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVTHINNINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Thickening Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVThickeningFilterObjectType;
	filterClassInfo.m_ClassId = SVThickeningFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVTHICKENINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Watershed Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVWatershedFilterObjectType;
	filterClassInfo.m_ClassId = SVWatershedFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVWATERSHEDFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Custom Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVCustomFilterObjectType;
	filterClassInfo.m_ClassId = SVCustomFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVCUSTOMFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Custom2 Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVCustom2FilterObjectType;
	filterClassInfo.m_ClassId = Custom2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_CUSTOM2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Ranking Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVRankingFilterObjectType;
	filterClassInfo.m_ClassId = SVRankingFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVRANKINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Smooth Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVSmoothFilterObjectType;
	filterClassInfo.m_ClassId = SVSmoothFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSMOOTHFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Sharpen Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVSharpenFilterObjectType;
	filterClassInfo.m_ClassId = SVSharpenFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSHARPENFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Sharpen2 Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVSharpen2FilterObjectType;
	filterClassInfo.m_ClassId = SVSharpen2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSHARPEN2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Opening Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVOpeningFilterObjectType;
	filterClassInfo.m_ClassId = SVOpeningFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVOPENINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Closing Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVClosingFilterObjectType;
	filterClassInfo.m_ClassId = SVClosingFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVCLOSINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Dilation Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVDilationFilterObjectType;
	filterClassInfo.m_ClassId = SVDilationFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVDILATIONFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Erosion Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVErosionFilterObjectType;
	filterClassInfo.m_ClassId = SVErosionFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVEROSIONFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVEdgeDetectFilterObjectType;
	filterClassInfo.m_ClassId = SVEdgeDetectFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVEDGEDETECTFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Edge Detect2 Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVEdgeDetect2FilterObjectType;
	filterClassInfo.m_ClassId = SVEdgeDetect2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVEDGEDETECT2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Vert Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVVertEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SVVertEdgeFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVVERTEDGEFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Horiz Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVHorzEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SVHorizEdgeFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVHORIZEDGEFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Laplacian Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVLaplacianEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SVLaplacianEdgeFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVLAPLACIANEDGEFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Laplacian 2 Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVLaplacianEdge2FilterObjectType;
	filterClassInfo.m_ClassId = SVLaplacianEdge2FilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVLAPLACIANEDGE2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Max Minus Min Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVMaxMinusMinFilterObjectType;
	filterClassInfo.m_ClassId = SVMaxMinusMinFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVMAXMINUSMINFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Erase Border Blobs Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::EraseBorderBlobsFilterObjectType;
	filterClassInfo.m_ClassId = SVEraseBorderBlobsFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_ERASEBORDERBLOBSFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Extract Holes Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::ExtractHolesFilterObjectType;
	filterClassInfo.m_ClassId = SVExtractHolesFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_EXTRACTHOLESFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Fill Holes Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::FillHolesFilterObjectType;
	filterClassInfo.m_ClassId = SVFillHolesFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_FILLHOLESFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Reconstruct Filter
	filterClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.SubType = SvPb::ReconstructFilterObjectType;
	filterClassInfo.m_ClassId = SVReconstructFilterClassGuid;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_RECONSTRUCTFILTER);
	m_availableChildren.push_back(filterClassInfo);
}
#pragma endregion Private Methods
