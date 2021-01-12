//******************************************************************************
/// \copyright (c) 2018 by Seidenader Maschinenbau GmbH
/// \file IndexEquation.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class to fill a DoubleSortValueObject (an array) by using an index value (e.g. used for the add column formula in table analyzer tool or custom feature in BlobAnalyzer2).
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "IndexEquation.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(IndexEquation, SvPb::IndexEquationId);

IndexEquation::IndexEquation(SVObjectClass* pOwner, int StringResourceID)
	: EquationArray(pOwner, StringResourceID)
{
	init();
}

IndexEquation::~IndexEquation()
{
}

bool IndexEquation::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (nullptr == m_pResultColumn)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
	}

	return Result;
}

void IndexEquation::SetName(LPCTSTR Name)
{
	__super::SetName(Name);

	if (nullptr != m_pResultColumn)
	{
		std::string OldName = m_pResultColumn->GetName();
		if (OldName != Name)
		{
			m_pResultColumn->SetName(Name);
			GetInspection()->OnObjectRenamed(*m_pResultColumn, OldName);
		}
	}
}

bool IndexEquation::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool   bRetVal = true;
	if (nullptr != m_pResultColumn)
	{
		size_t arrayIndex = m_pResultColumn->getSortContainerSize();
		if (0 < arrayIndex)
		{
			bRetVal = setRunValues(1, static_cast<int>(arrayIndex));

			// Run equation.. 
			bRetVal &= __super::onRun(rRunStatus, pErrorMessages);

			if (bRetVal)
			{
				// Get equation result...
				auto values = getValueArray();
				m_pResultColumn->SetArrayValues(values);
			}
		}
	}
	else
	{
		bRetVal = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
	}

	if (!bRetVal)
	{
		rRunStatus.SetInvalid();
	}

	return bRetVal;
}

void IndexEquation::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::IndexEquationObjectType;
}

} //namespace SvOp
