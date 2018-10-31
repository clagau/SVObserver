//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file BlobReconstructBasicFilter.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter EraseBorderBlobs.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "BlobReconstructBasicFilter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum SV_BLOB_COLOR_ENUM
{
	SV_BLOB_BLACK = 0,
	SV_BLOB_WHITE = 1
};// end SV_BLOB_COLOR_ENUM

  // Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobColorEnums = _T("Black=0,White=1");


BlobReconstructBasicFilter::BlobReconstructBasicFilter(SVObjectClass* POwner, int StringResourceID, long operatorMode)
	: SVFilterClass(POwner, StringResourceID)
	, m_operatorMode (operatorMode)
{
	init();
}

BlobReconstructBasicFilter::~BlobReconstructBasicFilter()
{
}

void BlobReconstructBasicFilter::init()
{
	RegisterEmbeddedObject(&m_blobColor, SVBlobColorGuid, IDS_BLACK_BLOBS, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_isGrayOn, SVGrayOnGuid, IDS_OBJECTNAME_GRAYON, false, SvOi::SVResetItemNone);

	m_blobColor.SetEnumTypes(g_strBlobColorEnums);
	m_blobColor.SetDefaultValue(SV_BLOB_WHITE, true);
	m_isGrayOn.SetDefaultValue(BOOL(false));


	const UINT cAttributes = SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE;
	m_blobColor.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
	m_isGrayOn.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);

	// Set default inputs and outputs
	addDefaultInputObjects();

}

bool BlobReconstructBasicFilter::onRun(bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	if (nullptr != rInputImageHandle && !rInputImageHandle->empty() && nullptr != rOutputImageHandle && !rOutputImageHandle->empty())
	{
		long proceMode = getProceMode();
		MblobReconstruct(rInputImageHandle->GetBuffer().GetIdentifier(), M_NULL, rOutputImageHandle->GetBuffer().GetIdentifier(), m_operatorMode, proceMode);
		HRESULT hr = SVMatroxApplicationInterface::GetLastStatus();

		if (S_OK != hr)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		// Success...
		return true;
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	SetInvalid();
	rRunStatus.SetInvalid();
	return false;
}

long BlobReconstructBasicFilter::getProceMode()
{
	long lColorTmp;
	BOOL bGrayOn;

	m_isGrayOn.GetValue(bGrayOn);
	m_blobColor.GetValue(lColorTmp);

	long proceMode = bGrayOn ? M_GRAYSCALE : M_BINARY;
	if (SV_BLOB_BLACK == lColorTmp)
	{
		proceMode |= M_FOREGROUND_ZERO;
	}

	return proceMode;
}