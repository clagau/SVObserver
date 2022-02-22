//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobReconstructBasicFilter.h
/// All Rights Reserved
//*****************************************************************************
/// The basic class for the filter Blob reconstruct method (MblobReconstruct). 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
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
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	long getProceMode();

private:
	void init();

	SvVol::SVEnumerateValueObjectClass  m_blobColor;
	SvVol::SVBoolValueObjectClass m_isGrayOn;

	long m_operatorMode;
};