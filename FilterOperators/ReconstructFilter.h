//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ReconstructFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter Reconstruct.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BlobReconstructBasicFilter.h"
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class ReconstructFilter : public BlobReconstructBasicFilter
{
	SV_DECLARE_CLASS

public:
	ReconstructFilter(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RECONSTRUCTFILTER);
	virtual ~ReconstructFilter();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

protected:
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

private:
	void init();

	SvOl::InputObject m_SeedImageInput;
};
