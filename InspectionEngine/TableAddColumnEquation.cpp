//******************************************************************************
/// \copyright (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TableAddColumnEquation.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for the add column formula in table analyzer tool.
//******************************************************************************

#include "stdafx.h"
#include "TableAddColumnEquation.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVObjectLibrary/SVClsids.h"


SV_IMPLEMENT_CLASS(TableAddColumnEquation, TableAddColumnEquationGuid);

TableAddColumnEquation::TableAddColumnEquation(SVObjectClass* pOwner, int StringResourceID)
	: SVEquationClass(pOwner, StringResourceID)
{
	init();
}

TableAddColumnEquation::~TableAddColumnEquation()
{
}

bool TableAddColumnEquation::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (nullptr == m_pResultColumn || nullptr == m_pIndex)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(message);
		}
	}

	return Result;
}

void TableAddColumnEquation::SetName(LPCTSTR Name)
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

bool TableAddColumnEquation::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool   bRetVal = true;
	if (nullptr != m_pResultColumn && nullptr != m_pIndex)
	{
		size_t arrayIndex = m_pResultColumn->getSortContainer().size();

		for (int i = 1; i <= arrayIndex; i++)
		{
			double result = 0.0;
			// Set current index (is one-based)...
			m_pIndex->SetValue(i);

			// Run equation.. 
			bRetVal &= __super::onRun(rRunStatus, pErrorMessages);

			if (bRetVal && HasCondition() && IsEnabled())
			{
				// Get equation result...
				result = getResult();
			}

			// Put result into column at right position...
			m_pResultColumn->SetValue(result, i - 1);
		}
	}
	else
	{
		bRetVal = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(message);
		}
	}

	if (!bRetVal)
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}

	return bRetVal;
}

void TableAddColumnEquation::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::TableAddColumnEquationObjectType;

	// Set default inputs and outputs
	addDefaultInputObjects();
}