//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file EraseBorderBlobsFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter EraseBorderBlobs.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BlobReconstructBasicFilter.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class EraseBorderBlobsFilter : public BlobReconstructBasicFilter
{
	SV_DECLARE_CLASS

public:
	EraseBorderBlobsFilter(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_ERASEBORDERBLOBSFILTER);
	virtual ~EraseBorderBlobsFilter();

protected:

private:
	void init();
};