//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file ReconstructFilter.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter Reconstruct.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <milblob.h>
#include "ReconstructFilter.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(ReconstructFilter, SvPb::ReconstructFilterClassId)

ReconstructFilter::ReconstructFilter(SVObjectClass* POwner, int StringResourceID)
	: BlobReconstructBasicFilter(POwner, StringResourceID, M_FILL_HOLES)
{
	init();
}

ReconstructFilter::~ReconstructFilter()
{
}

bool ReconstructFilter::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_SeedImageInfo);

	if (nullptr == SvOl::getInput<SvIe::SVImageClass>(m_SeedImageInfo))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

bool ReconstructFilter::onRun(bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	auto* pSeedImage = SvOl::getInput<SvIe::SVImageClass>(m_SeedImageInfo);
	if (nullptr != rInputImageHandle && !rInputImageHandle->empty() && nullptr != rOutputImageHandle && !rOutputImageHandle->empty() && nullptr != pSeedImage )
	{
		SvTrc::IImagePtr pSeedImageBuffer = pSeedImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		SvOi::SVImageBufferHandlePtr pSeedBuffer = (nullptr != pSeedImageBuffer) ? pSeedImageBuffer->getHandle() : nullptr;
		if (nullptr != pSeedBuffer && !pSeedBuffer->empty())
		{
			long proceMode = getProceMode();
			SVMatroxBuffer& rInBuffer = rInputImageHandle->GetBuffer();
			SVMatroxBuffer& rOutBuffer = rOutputImageHandle->GetBuffer();
			MblobReconstruct(rInBuffer.GetIdentifier(), pSeedBuffer->GetBuffer().GetIdentifier(), rOutBuffer.GetIdentifier(), M_RECONSTRUCT_FROM_SEED, proceMode);
			HRESULT hr = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK != hr)
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
				// Signal that something was wrong...
				rRunStatus.SetInvalid();
				return false;
			}

			// Success...
			return true;
		}
	}

	if (nullptr != pErrorMessages)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		pErrorMessages->push_back(Msg);
	}

	// Signal that something was wrong...
	rRunStatus.SetInvalid();
	return false;
}

void ReconstructFilter::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::ReconstructFilterObjectType;

	m_SeedImageInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_SeedImageInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_SeedImageInfo, SvDef::SeedImageConnectionName);

}