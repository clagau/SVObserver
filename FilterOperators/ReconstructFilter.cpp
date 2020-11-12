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
#include "SVStatusLibrary/RunStatus.h"
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

	auto* pSeedImage = SvOl::getInput<SvIe::SVImageClass>(m_SeedImageInfo);
	auto* pOwner = m_ownerObjectInfo.getObject();
	SvIe::SVImageClass* pSourceImage = nullptr;
	if (nullptr != pOwner)
	{
		pSourceImage = dynamic_cast<SvIe::SVImageClass*>(pOwner->getFirstObject({ SvPb::SVImageObjectType }));
	}
	if (nullptr == pSeedImage || nullptr == pSourceImage)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}
	else
	{
		const auto& rSeedInfo = pSeedImage->GetImageInfo();
		const auto& rSourceInfo = pSourceImage->GetImageInfo();
		double seedWidth = 0.0;
		double sourceWidth = 0.0;
		rSeedInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputWidth, seedWidth);
		rSourceInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputWidth, sourceWidth);
		double seedHeight = 0.0;
		double sourceHeight = 0.0;
		rSeedInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputHeight, seedHeight);
		rSourceInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputHeight, sourceHeight);
		if (seedWidth != sourceWidth || seedHeight != sourceHeight)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SeedImageDifferentSize, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

bool ReconstructFilter::onRun(bool, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	auto* pSeedImage = SvOl::getInput<SvIe::SVImageClass>(m_SeedImageInfo);
	if (nullptr != rInputImageHandle && !rInputImageHandle->empty() && nullptr != rOutputImageHandle && !rOutputImageHandle->empty() && nullptr != pSeedImage )
	{
		SvOi::ITRCImagePtr pSeedImageBuffer = pSeedImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
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
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		pErrorMessages->push_back(Msg);
	}

	// Signal that something was wrong...
	rRunStatus.SetInvalid();
	return false;
}

void ReconstructFilter::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::ReconstructFilterObjectType;

	m_SeedImageInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_SeedImageInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_SeedImageInfo, SvDef::SeedImageConnectionName);

}