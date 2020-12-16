//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableAddColumnAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for adding a new column.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableAddColumnAnalyzer.h"
#include "Tools/TableAnalyzerTool.h"
#include "Definitions/TextDefineSvDef.h"
#include "Operators/IndexEquation.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableAddColumnAnalyzer, SvPb::TableAnaylzerAddColumnClassId);


#pragma region Constructor
TableAddColumnAnalyzer::TableAddColumnAnalyzer(LPCTSTR ObjectName)
	: SVTaskObjectClass(ObjectName)
{
	Initialize();
}

TableAddColumnAnalyzer::TableAddColumnAnalyzer(SVObjectClass* pOwner, int StringResourceID)
	: SVTaskObjectClass(pOwner, StringResourceID)
{
	Initialize();
}

TableAddColumnAnalyzer::~TableAddColumnAnalyzer()
{
	SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
	if (nullptr != pTool)
	{
		pTool->removeNewColumn(m_pNewColumn);
	}
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableAddColumnAnalyzer::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = __super::CreateObject(rCreateStructure);

	SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
	auto* pColumnEquation = dynamic_cast<SvOp::IndexEquation*>(getFirstObject({SvPb::SVEquationObjectType, SvPb::IndexEquationObjectType}));
	if (nullptr != pTool && nullptr != pColumnEquation)
	{
		//Replace in old configurations removed index-value with index-keyword.
		std::string Name = "\"" + pTool->GetObjectNameToObjectType() + "." + SvUl::LoadStdString(IDS_OBJECTNAME_INDEXVARIABLE) + "\"";
		auto text = pColumnEquation->GetEquationText();
		if (pColumnEquation->GetEquationText() != SvUl::searchAndReplace(text, Name.c_str(), SvDef::cIndexKeyword))
		{
			pColumnEquation->SetEquationText(text);
		}

		SvVol::DoubleSortValueObject* pNewColumn = m_newColumnInput.getInput<SvVol::DoubleSortValueObject>();;
		if (!m_newColumnInput.IsConnected() || nullptr == pNewColumn)
		{
			m_pNewColumn = pTool->addNewColumn("NewColumn", this);
			m_newColumnInput.SetInputObject(m_pNewColumn.get());
		}
		else
		{
			m_pNewColumn = pTool->setNewColumn(pNewColumn, this);
		}

		pColumnEquation->setResultColumn(m_pNewColumn);
		m_pNewColumn->SetName(pColumnEquation->GetName());
	}
	else
	{
		bOk = false;
	}
	
	return bOk && (nullptr != m_pNewColumn);
}

bool TableAddColumnAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_newColumnInput.validateInput();

	SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
	if (nullptr == pTool || nullptr == m_pNewColumn)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
		Result = false;
	}
	
	return Result;
}
#pragma endregion Public Methods

#pragma region Private Methods
void TableAddColumnAnalyzer::Initialize()
{
	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::TableAnalyzerAddColumnType;

	auto* pColumnEquation = new SvOp::IndexEquation(this);
	pColumnEquation->SetEquationText(SvDef::cIndexKeyword);
	AddFriend(pColumnEquation->getObjectId());

	// New Column Input.
	m_newColumnInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::DoubleSortValueObjectType);
	registerInputObject(&m_newColumnInput, SvDef::cInputTag_NewColumn, SvPb::NewColumnInputEId);
}
#pragma endregion Private Methods

} //namespace SvAo
