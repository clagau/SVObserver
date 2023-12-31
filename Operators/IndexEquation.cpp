//******************************************************************************
/// \copyright (c) 2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file IndexEquation.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class to fill a DoubleSortValueObject (an array) by using an index value (e.g. used for the add column formula in table analyzer tool or custom feature in BlobAnalyzer2).
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "IndexEquation.h"
#include "InspectionEngine/RunStatus.h"
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
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(message);
		}
	}

	return Result;
}

void IndexEquation::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	__super::OnObjectRenamed(rRenamedObject, rOldName);
	if (&rRenamedObject == this && nullptr != m_pResultColumn)
	{
		//Do not use method verifyAndSetName, because name have to be set even if names double. The check will be done by TableTool and leads to an error.
		std::string OldName = m_pResultColumn->GetName();
		if (OldName != GetName())
		{
			m_pResultColumn->SetName(GetName());
			GetInspection()->OnObjectRenamed(*m_pResultColumn, OldName);
		}
	}
}

bool IndexEquation::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
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
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
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
