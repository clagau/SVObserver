//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file Ranking2Filter.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter Ranking2Filter.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Ranking2Filter.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(Ranking2Filter, SvPb::Ranking2FilterClassId)

#pragma region Constructor
Ranking2Filter::Ranking2Filter(SVObjectClass* pOwner, int StringResourceID)
	: SVFilterClass(pOwner, StringResourceID)
{
	init();
}

Ranking2Filter::~Ranking2Filter()
{
	m_milKernel.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
bool Ranking2Filter::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = __super::CreateObject(rCreateStructure);

	RebuildKernel();

	return bOk;
}

bool Ranking2Filter::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	RebuildKernel();

	return Result && ValidateLocal(pErrorMessages);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void Ranking2Filter::RebuildKernel()
{
	// First free old kernel
	m_milKernel.clear();

	long Width = validateKernelSize(m_KernelWidth);
	long Height = validateKernelSize(m_KernelHeight);

	SVMatroxBufferCreateStruct CreateStruct;
	CreateStruct.m_eAttribute = SVBufAttStructElement;
	CreateStruct.m_eType = SV32BitSigned;
	CreateStruct.m_lSizeX = Width;
	CreateStruct.m_lSizeY = Height;
	SVMatroxBufferInterface::Create(m_milKernel, CreateStruct);

	std::vector<long> KernelArray;
	m_KernelArray.GetArrayValues(KernelArray);
	std::vector<long> kernelData;
	kernelData.resize(Width*Height);
	for (long i = 0; i < Height; i++)
	{
		for (long j = 0; j < Width; j++)
		{
			long unsigned Index = i * Width + j;
			long value = 0;
			if (Index < KernelArray.size())
			{
				value = KernelArray[Index];
			}
			kernelData[Index] = value == 1 ? 1 : SVValueDontCare;
		}
	}

	//Put the custom data in it.
	SVMatroxBufferInterface::PutBuffer(m_milKernel, &kernelData[0]);
}

bool Ranking2Filter::onRun(bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	if (m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		long lRank;
		m_RankingRank.GetValue(lRank);
		HRESULT l_Code = SVMatroxImageInterface::Rank(rOutputImageHandle->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
			m_milKernel, lRank);
		if (S_OK != l_Code)
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
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	rRunStatus.SetInvalid();
	return false;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void Ranking2Filter::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::Ranking2FilterObjectType;

	RegisterEmbeddedObject(&m_KernelArray, SvPb::FilterKernelEId, IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL, false, SvOi::SVResetItemOwner);

	RegisterEmbeddedObject(&m_KernelWidth, SvPb::FilterKernelWidthEId, IDS_OBJECTNAME_RANKINGFILTER_RANKINGWIDTH, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_KernelHeight, SvPb::FilterKernelHeightEId, IDS_OBJECTNAME_RANKINGFILTER_RANKINGHEIGHT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_RankingRank, SvPb::RankingFilterRankingRankEId, IDS_OBJECTNAME_RANKINGFILTER_RANKINGRANK, false, SvOi::SVResetItemOwner);

	m_KernelArray.SetArraySize(SvDef::cStandardKernelSize*SvDef::cStandardKernelSize);
	m_KernelArray.SetDefaultValue(1);

	m_KernelWidth.SetDefaultValue(SvDef::cStandardKernelSize);
	m_KernelHeight.SetDefaultValue(SvDef::cStandardKernelSize);
	m_RankingRank.SetDefaultValue(SVValueMedian);
	
	const UINT cAttributes = SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable;
	m_KernelArray.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);

	m_KernelWidth.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
	m_KernelHeight.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
	m_RankingRank.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);

	RebuildKernel();
}

long Ranking2Filter::validateKernelSize(SvVol::SVLongValueObjectClass& rKernelSize)
{
	long Size(0);

	rKernelSize.GetValue(Size);
	//Check if odd otherwise make it odd
	if (1 != Size % 2)
	{
		Size--;
	}
	if (1 > Size)
	{
		Size = 1;
	}
	else if (SvDef::cMaxKernelSize < Size)
	{
		Size = SvDef::cMaxKernelSize;
	}
	rKernelSize.SetValue(Size);

	return Size;
}

bool Ranking2Filter::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result(true);

	//Check that Width and Height are odd and between 1 and MaxKernelSize
	long Width(0);
	m_KernelWidth.GetValue(Width);
	if (1 != Width % 2 || 1 > Width || SvDef::cMaxKernelSize < Width)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), Width));
			msgList.push_back(SvUl::Format(_T("%d"), SvDef::cMaxKernelSize));
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidKernelWidth, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	long Height(0);
	m_KernelHeight.GetValue(Height);
	if (1 != Height % 2 || 1 > Height || SvDef::cMaxKernelSize < Height)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), Height));
			msgList.push_back(SvUl::Format(_T("%d"), SvDef::cMaxKernelSize));
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DataInvalidKernelHeight, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}
#pragma endregion Private Methods

