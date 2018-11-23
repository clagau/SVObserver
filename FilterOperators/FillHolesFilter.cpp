//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file FillHolesFilter.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter FillHoles.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <milblob.h>
#include "FillHolesFilter.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(FillHolesFilter, SVFillHolesFilterClassGuid)

FillHolesFilter::FillHolesFilter(SVObjectClass* POwner, int StringResourceID)
	: BlobReconstructBasicFilter(POwner, StringResourceID, M_FILL_HOLES)
{
	init();
}

FillHolesFilter::~FillHolesFilter()
{
}

void FillHolesFilter::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::FillHolesFilterObjectType;

}