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
#include "SVObjectLibrary/SVClsids.h"
#include "Tools/TableAnalyzerTool.h"
#include "Definitions/TextDefineSvDef.h"
#include "Operators/TableColumnEquation.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableAddColumnAnalyzer, TableAnaylzerAddColumnGuid);


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
	auto* pColumnEquation = dynamic_cast<SvOp::TableAddColumnEquation*>(getFirstObject({SvPb::SVEquationObjectType, SvPb::TableAddColumnEquationObjectType}));
	if (nullptr != pTool && nullptr != pColumnEquation)
	{
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVNotSetObjectType, SvPb::SVNotSetSubObjectType, TableAnalyzerIndexObjectGuid);
		IObjectClass* pIndex = pTool->getFirstObject(info);
		pColumnEquation->setIndexObject(dynamic_cast<SvVol::SVLongValueObjectClass*>(pIndex));

		SvVol::DoubleSortValueObject* pNewColumn = dynamic_cast<SvVol::DoubleSortValueObject*>(m_newColumnObjectInfo.GetInputObjectInfo().getObject());
		if (!m_newColumnObjectInfo.IsConnected() || nullptr == pNewColumn)
		{
			m_pNewColumn = pTool->addNewColumn("NewColumn", this);
			m_newColumnObjectInfo.SetInputObject(m_pNewColumn.get());
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

	SvOl::ValidateInput(m_newColumnObjectInfo);

	SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
	if (nullptr == pTool || nullptr == m_pNewColumn)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::TableAnalyzerAddColumnType;

	auto* pColumnEquation = new SvOp::TableAddColumnEquation(this);
	AddFriend(pColumnEquation->GetUniqueObjectID());

	// New Column Input.
	m_newColumnObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::DoubleSortValueObjectType);
	m_newColumnObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_newColumnObjectInfo, SvDef::cInputTag_NewColumn);

	addDefaultInputObjects();
}
#pragma endregion Private Methods

} //namespace SvAo
