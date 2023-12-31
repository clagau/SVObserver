//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ExtractHolesFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter ExtractHoles.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BlobReconstructBasicFilter.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class ExtractHolesFilter : public BlobReconstructBasicFilter
{
	SV_DECLARE_CLASS

public:
	ExtractHolesFilter(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_EXTRACTHOLESFILTER);
	virtual ~ExtractHolesFilter();

protected:

private:
	void init();
};
