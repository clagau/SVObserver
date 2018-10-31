//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file BlobReconstructBasicFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The basic class for the filter Blob reconstruct method (MblobReconstruct). 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}
namespace SvOi
{
class SVImageBufferHandleInterface;
typedef std::shared_ptr< SVImageBufferHandleInterface > SVImageBufferHandlePtr;
}
#pragma endregion Declarations

class BlobReconstructBasicFilter : public SVFilterClass
{
public:
	BlobReconstructBasicFilter(SVObjectClass* POwner, int StringResourceID, long operatorMode);
	virtual ~BlobReconstructBasicFilter();

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

protected:
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	long getProceMode();

private:
	void init();

	SVEnumerateValueObjectClass  m_blobColor;
	SVBoolValueObjectClass m_isGrayOn;

	long m_operatorMode;
};