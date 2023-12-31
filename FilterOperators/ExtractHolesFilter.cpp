//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ExtractHolesFilter.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter ExtractHoles.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <milblob.h>
#include "ExtractHolesFilter.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(ExtractHolesFilter, SvPb::ExtractHolesFilterClassId)

ExtractHolesFilter::ExtractHolesFilter(SVObjectClass* POwner, int StringResourceID)
	: BlobReconstructBasicFilter(POwner, StringResourceID, M_EXTRACT_HOLES)
{
	init();
}

ExtractHolesFilter::~ExtractHolesFilter()
{
}

void ExtractHolesFilter::init()
{
	m_ObjectTypeInfo.m_SubType = SvPb::ExtractHolesFilterObjectType;

}