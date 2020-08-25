//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file ReconstructFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter Reconstruct.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BlobReconstructBasicFilter.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class ReconstructFilter : public BlobReconstructBasicFilter
{
	SV_DECLARE_CLASS(ReconstructFilter);

public:
	ReconstructFilter(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RECONSTRUCTFILTER);
	virtual ~ReconstructFilter();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

protected:
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

private:
	void init();

	SvOl::SVInObjectInfoStruct m_SeedImageInfo;
};
