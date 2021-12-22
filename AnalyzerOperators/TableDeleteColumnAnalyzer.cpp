//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableDeleteColumnAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for delete a old column.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableDeleteColumnAnalyzer.h"
#include "Tools/TableAnalyzerTool.h"
#include "Definitions/TextDefineSvDef.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableDeleteColumnAnalyzer, SvPb::TableAnaylzerDeleteColumnClassId);


#pragma region Constructor
TableDeleteColumnAnalyzer::TableDeleteColumnAnalyzer(LPCTSTR ObjectName)
	: SVTaskObjectClass(ObjectName)
{
	Initialize();
}

TableDeleteColumnAnalyzer::TableDeleteColumnAnalyzer(SVObjectClass* pOwner, int StringResourceID)
	: SVTaskObjectClass(pOwner, StringResourceID)
{
	Initialize();
}

TableDeleteColumnAnalyzer::~TableDeleteColumnAnalyzer()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableDeleteColumnAnalyzer::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_deleteColumnInput.validateInput();

	SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*> (GetParent());
	SvVol::DoubleSortValueObject* pColumnValues = m_deleteColumnInput.getInput<SvVol::DoubleSortValueObject>(false);
	if (nullptr == pTool || nullptr == pColumnValues)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
		Result = false;
	}
	else
	{
		pTool->removeSourceColumn(pColumnValues);
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Private Methods
void TableDeleteColumnAnalyzer::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	m_ObjectTypeInfo.m_SubType = SvPb::TableAnalyzerDeleteColumnType;

	// New Column Input.
	m_deleteColumnInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::DoubleSortValueObjectType);
	registerInputObject(&m_deleteColumnInput, SvDef::cInputTag_DeleteColumn, SvPb::DeleteColumnInputEId);
}
#pragma endregion Private Methods

} //namespace SvAo
