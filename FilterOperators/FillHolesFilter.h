//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file FillHolesFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter FillHoles.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BlobReconstructBasicFilter.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class FillHolesFilter : public BlobReconstructBasicFilter
{
	SV_DECLARE_CLASS

public:
	FillHolesFilter(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_FILLHOLESFILTER);
	virtual ~FillHolesFilter();

protected:

private:
	void init();
};
