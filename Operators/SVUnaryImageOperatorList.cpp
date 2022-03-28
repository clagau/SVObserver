//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVUnaryImageOperatorList::SVUnaryImageOperatorList(bool onlyImageBefore, SVObjectClass* POwner, int StringResourceID )
							  :SVTaskObjectListClass( POwner, StringResourceID ) 
{
	init(onlyImageBefore);
}

SVUnaryImageOperatorList::~SVUnaryImageOperatorList() 
{ 
	SVUnaryImageOperatorList::CloseObject();
}

bool SVUnaryImageOperatorList::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectListClass::CreateObject(rCreateStructure) && nullptr != GetTool() && nullptr != GetInspection();

	return m_isCreated;
}

SvIe::SVImageClass* SVUnaryImageOperatorList::getInputImage(bool bRunMode /*=false*/) const
{
	return m_inputImage.getInput<SvIe::SVImageClass>(bRunMode);
}

bool SVUnaryImageOperatorList::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_inputImage.validateInput();

	SvIe::SVImageClass* pInputImage = getInputImage();
	if (nullptr != pInputImage)
	{
		//! Check that the image color type is correct
		if (pInputImage->GetObjectSubType() != m_inputImage.GetInputObjectInfo().m_ObjectTypeInfo.m_SubType)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongInputImageType, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;

}

#pragma region Protected Methods
bool SVUnaryImageOperatorList::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	SvIe::SVImageClass* pImage = getInputImage();
	if (nullptr != pImage && imageId == pImage->getObjectId())
	{
		Result = true;
	}

	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVUnaryImageOperatorList::init(bool onlyImageBefore)
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorListObjectType;

	// Identify our input type needs
	m_inputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject(&m_inputImage, SvDef::SourceImageInputName, SvPb::ImageInputEId);
	if (onlyImageBefore)
	{
		m_inputImage.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);
	}

	// Set available Filters
	// Populate the available filter list
	SvIe::SVClassInfoStruct filterClassInfo;

	// Add the Skeleton Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVSkeletonFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::SkeletonFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSKELETONFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Thinning Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVThinningFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::ThinningFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVTHINNINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Thickening Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVThickeningFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::ThickeningFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVTHICKENINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Watershed Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVWatershedFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::WatershedFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVWATERSHEDFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Custom Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVCustomFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::CustomFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVCUSTOMFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Custom2 Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVCustom2FilterObjectType;
	filterClassInfo.m_ClassId = SvPb::Custom2FilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_CUSTOM2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Ranking Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVRankingFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::RankingFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVRANKINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Ranking2 Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::Ranking2FilterObjectType;
	filterClassInfo.m_ClassId = SvPb::Ranking2FilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_RANKING2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Smooth Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVSmoothFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::SmoothFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSMOOTHFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Sharpen Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVSharpenFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::SharpenFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSHARPENFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Sharpen2 Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVSharpen2FilterObjectType;
	filterClassInfo.m_ClassId = SvPb::Sharpen2FilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVSHARPEN2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Opening Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVOpeningFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::OpeningFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVOPENINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Closing Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVClosingFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::ClosingFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVCLOSINGFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Dilation Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVDilationFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::DilationFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVDILATIONFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Erosion Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVErosionFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::ErosionFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVEROSIONFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVEdgeDetectFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::EdgeDetectFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVEDGEDETECTFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Edge Detect2 Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVEdgeDetect2FilterObjectType;
	filterClassInfo.m_ClassId = SvPb::EdgeDetect2FilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVEDGEDETECT2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Vert Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVVertEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::VertEdgeFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVVERTEDGEFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Horiz Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVHorzEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::HorizEdgeFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVHORIZEDGEFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Laplacian Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLaplacianEdgeFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::LaplacianEdgeFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVLAPLACIANEDGEFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Laplacian 2 Edge Detect Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLaplacianEdge2FilterObjectType;
	filterClassInfo.m_ClassId = SvPb::LaplacianEdge2FilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVLAPLACIANEDGE2FILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Max Minus Min Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVMaxMinusMinFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::MaxMinusMinFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_SVMAXMINUSMINFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Erase Border Blobs Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::EraseBorderBlobsFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::EraseBorderBlobsFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_ERASEBORDERBLOBSFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Extract Holes Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::ExtractHolesFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::ExtractHolesFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_EXTRACTHOLESFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Fill Holes Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::FillHolesFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::FillHolesFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_FILLHOLESFILTER);
	m_availableChildren.push_back(filterClassInfo);

	// Add the Reconstruct Filter
	filterClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	filterClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::ReconstructFilterObjectType;
	filterClassInfo.m_ClassId = SvPb::ReconstructFilterClassId;
	filterClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_RECONSTRUCTFILTER);
	m_availableChildren.push_back(filterClassInfo);
}
#pragma endregion Private Methods

} //namespace SvOp
