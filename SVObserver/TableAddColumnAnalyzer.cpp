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
#include "SVObjectLibrary\SVClsids.h"
#include "TableAnalyzerTool.h"
#include "TextDefinesSvO.h"
#include "InspectionEngine\TableColumnEquation.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(TableAddColumnAnalyzer, TableAnaylzerAddColumnGuid);

#pragma endregion Declarations

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
	TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
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

	TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
	if (nullptr != pTool && nullptr != m_pColumnEquation)
	{
		SvDef::SVObjectTypeInfoStruct info(SvDef::SVNotSetObjectType, SvDef::SVNotSetSubObjectType, TableAnalyzerIndexObjectGuid);
		IObjectClass* pIndex = pTool->getFirstObject(info);
		m_pColumnEquation->setIndexObject(dynamic_cast<SVLongValueObjectClass*>(pIndex));

		DoubleSortValueObject* pNewColumn = dynamic_cast<DoubleSortValueObject*>(m_newColumnObjectInfo.GetInputObjectInfo().getObject());
		if (!m_newColumnObjectInfo.IsConnected() || nullptr == pNewColumn)
		{
			m_pNewColumn = pTool->addNewColumn("NewColumn");
			m_newColumnObjectInfo.SetInputObject(m_pNewColumn.get());
		}
		else
		{
			m_pNewColumn = pTool->setNewColumn(pNewColumn);
		}

		m_pColumnEquation->setResultColumn(m_pNewColumn);
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

	TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
	if (nullptr == pTool || nullptr == m_pColumnEquation || nullptr == m_pNewColumn)
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::TableAnalyzerAddColumnType;

	m_pColumnEquation = new TableAddColumnEquation(this);
	AddFriend(m_pColumnEquation->GetUniqueObjectID());

	// New Column Input.
	m_newColumnObjectInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::DoubleSortValueObjectType);
	m_newColumnObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_newColumnObjectInfo, SvO::cInputTag_NewColumn);

	addDefaultInputObjects();
}
#pragma endregion Private Methods
