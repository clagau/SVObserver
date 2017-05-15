//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:58:30  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVFilterClass.h"
#include "SVCustomFilters.h"
#include "Custom2Filter.h"
#include "SVClosingFilterClass.h"
#include "SVDilationFilterClass.h"
#include "SVEdgeDetect2FilterClass.h"
#include "SVEdgeDetectFilterClass.h"
#include "SVErosionFilterClass.h"
#include "SVFilterClass.h"
#include "SVHorizEdgeFilterClass.h"
#include "SVLaplacianEdge2FilterClass.h"
#include "SVLaplacianEdgeFilterClass.h"
#include "SVMaxMinusMinFilterClass.h"
#include "SVOpeningFilterClass.h"
#include "SVRankingFilters.h"
#include "SVSharpen2FilterClass.h"
#include "SVSharpenFilterClass.h"
#include "SVSkeletonFilter.h"
#include "SVSmoothFilterClass.h"
#include "SVThickeningFilter.h"
#include "SVThinningFilter.h"
#include "SVVertEdgeFilterClass.h"
#include "SVWaterShedFilter.h"

#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVFilterClass, SVFilterClassGuid )
SV_IMPLEMENT_CLASS( Custom2Filter, Custom2FilterClassGuid )
SV_IMPLEMENT_CLASS( SVClosingFilterClass, SVClosingFilterClassGuid )
SV_IMPLEMENT_CLASS( SVCustomFilterClass, SVCustomFilterClassGuid )
SV_IMPLEMENT_CLASS( SVDilationFilterClass, SVDilationFilterClassGuid )
SV_IMPLEMENT_CLASS( SVEdgeDetect2FilterClass, SVEdgeDetect2FilterClassGuid )
SV_IMPLEMENT_CLASS( SVEdgeDetectFilterClass, SVEdgeDetectFilterClassGuid )
SV_IMPLEMENT_CLASS( SVErosionFilterClass, SVErosionFilterClassGuid )
SV_IMPLEMENT_CLASS( SVHorizEdgeFilterClass, SVHorizEdgeFilterClassGuid )
SV_IMPLEMENT_CLASS( SVLaplacianEdge2FilterClass, SVLaplacianEdge2FilterClassGuid )
SV_IMPLEMENT_CLASS( SVLaplacianEdgeFilterClass, SVLaplacianEdgeFilterClassGuid )
SV_IMPLEMENT_CLASS( SVMaxMinusMinFilterClass, SVMaxMinusMinFilterClassGuid )
SV_IMPLEMENT_CLASS( SVOpeningFilterClass, SVOpeningFilterClassGuid )
SV_IMPLEMENT_CLASS( SVRankingFilterClass, SVRankingFilterClassGuid )
SV_IMPLEMENT_CLASS( SVSharpen2FilterClass, SVSharpen2FilterClassGuid )
SV_IMPLEMENT_CLASS( SVSharpenFilterClass, SVSharpenFilterClassGuid )
SV_IMPLEMENT_CLASS( SVSkeletonFilterClass, SVSkeletonFilterClassGuid )
SV_IMPLEMENT_CLASS( SVSmoothFilterClass, SVSmoothFilterClassGuid )
SV_IMPLEMENT_CLASS( SVThickeningFilterClass, SVThickeningFilterClassGuid )
SV_IMPLEMENT_CLASS( SVThinningFilterClass, SVThinningFilterClassGuid )
SV_IMPLEMENT_CLASS( SVVertEdgeFilterClass, SVVertEdgeFilterClassGuid )
SV_IMPLEMENT_CLASS( SVWatershedFilterClass, SVWatershedFilterClassGuid )





SVFilterClass::SVFilterClass( SVObjectClass* POwner, int StringResourceID )
: SVUnaryImageOperatorClass( POwner, StringResourceID )
{
	init();
}

SVFilterClass::~SVFilterClass()
{
}

BOOL SVFilterClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = SVUnaryImageOperatorClass::CreateObject( pCreateStructure );

	//hide valid, status and color for the result picker.
	m_statusTag.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
	m_statusColor.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );

	return bOk;
}

void SVFilterClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults
	
	// Set default inputs and outputs
}

