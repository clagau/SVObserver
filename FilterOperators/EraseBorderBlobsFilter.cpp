//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file EraseBorderBlobsFilter.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter EraseBorderBlobs.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <milblob.h>
#include "EraseBorderBlobsFilter.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(EraseBorderBlobsFilter, SvPb::EraseBorderBlobsFilterClassId)

EraseBorderBlobsFilter::EraseBorderBlobsFilter(SVObjectClass* POwner, int StringResourceID)
	: BlobReconstructBasicFilter(POwner, StringResourceID, M_ERASE_BORDER_BLOBS)
{
	init();
}

EraseBorderBlobsFilter::~EraseBorderBlobsFilter()
{
}

void EraseBorderBlobsFilter::init()
{
	m_ObjectTypeInfo.m_SubType = SvPb::EraseBorderBlobsFilterObjectType;

}