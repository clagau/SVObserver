//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableAnaylzerTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table Analyzer Tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableAnalyzerTool.h"
#include "Definitions/TextDefineSVDef.h"
#include "Operators/SVResultLong.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "AnalyzerOperators/TableAddColumnAnalyzer.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableAnalyzerTool, SvPb::TableAnalyzerToolClassId);

#pragma region Constructor
TableAnalyzerTool::TableAnalyzerTool(SVObjectClass* pOwner, int stringResourceID)
	:SVToolClass(pOwner, stringResourceID)
{
	LocalInitialize();
}

TableAnalyzerTool::~TableAnalyzerTool()
{

}
#pragma endregion Constructor

#pragma region Public Methods
bool TableAnalyzerTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_pResultTable = dynamic_cast<SvOp::TableCopyObject*>(SvOi::FindObject(getObjectId(), SvDef::SVObjectTypeInfoStruct(SvPb::TableObjectType, SvPb::TableCopyObjectType)));
	if (nullptr == m_pResultTable)
	{
		m_pResultTable = new SvOp::TableCopyObject(this);
		//This task must be the first in the list.
		Add(m_pResultTable, true);
		if (!CreateChildObject(m_pResultTable))
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(message.getMessage());
		}
	}

	// add input of result
	SvOp::SVLongResult* pResult = dynamic_cast<SvOp::SVLongResult*>(SvOi::FindObject(getObjectId(), SvDef::SVObjectTypeInfoStruct(SvPb::SVResultObjectType, SvPb::SVResultLongObjectType)));
	if (nullptr != pResult)
	{
		const SvOl::SVInputInfoListClass& inputInterface = pResult->GetPrivateInputList();
		//find right input
		SvOl::SVInputInfoListClass::const_iterator iter = std::find_if(inputInterface.begin(), inputInterface.end(), [](const SvOl::SVInObjectInfoStruct* rInfo)->bool
		{
			return rInfo->GetInputName() == SvDef::cInputTag_LongResultValue;
		});
		if (inputInterface.end() != iter)
		{
			//connect to input
			ConnectToObject(*iter, m_pResultTable->getNumberOfRowObject());
		}
		else
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(message.getMessage());
		}
	}
	else
	{
		bOk = false;
		SvStl::MessageContainer message;
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(message.getMessage());
	}

	// These values will not be exposed for the this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool TableAnalyzerTool::DoesObjectHaveExtents() const
{
	return false;
}

bool TableAnalyzerTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	Result = ValidateLocal(pErrorMessages) && Result;

	SvOl::ValidateInput(m_sourceTableObjectInfo);

	auto* pTableObject = dynamic_cast<SvOp::TableObject*> (m_sourceTableObjectInfo.GetInputObjectInfo().getFinalObject());
	if (!m_sourceTableObjectInfo.IsConnected() || nullptr == pTableObject)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
	}
	else if (pTableObject->GetAncestorInterface(SvPb::SVInspectionObjectType) != GetAncestorInterface(SvPb::SVInspectionObjectType))
	{
		pTableObject->DisconnectObjectInput(&m_sourceTableObjectInfo);
		m_sourceTableObjectInfo.SetInputObject(nullptr);
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
	}

	if (Result)
	{
		m_pResultTable->setSourecTable(pTableObject);
		Result = m_pResultTable->ResetObject(pErrorMessages);
	}
	else
	{
		m_pResultTable->setSourecTable(nullptr);
	}

	return Result;
}

SvVol::DoubleSortValuePtr TableAnalyzerTool::addNewColumn(LPCTSTR name, const SVTaskObjectClass* pAnalyzer)
{
	SvVol::DoubleSortValuePtr pRet;
	if (nullptr != m_pResultTable)
	{
		int pos = calcNewColumnPosition(pAnalyzer);
		pRet = m_pResultTable->addNewColumn(name, pos);
	}
	return pRet;
}

SvVol::DoubleSortValuePtr TableAnalyzerTool::setNewColumn(const SVObjectClass* pColumn, const SVTaskObjectClass* pAnalyzer)
{
	SvVol::DoubleSortValuePtr pRet;
	if (nullptr != m_pResultTable)
	{
		int pos = calcNewColumnPosition(pAnalyzer);
		pRet = m_pResultTable->setNewColumn(pColumn, pos);
	}
	return pRet;
}

void TableAnalyzerTool::removeNewColumn(const SvVol::DoubleSortValuePtr pColumn)
{
	if (nullptr != m_pResultTable)
	{
		m_pResultTable->removeNewColumn(pColumn);
	}
}

void TableAnalyzerTool::OnEmbeddedIDChanged(const SVObjectClass* pOwnerObject, SvPb::EmbeddedIdEnum oldEmbeddedID, SvPb::EmbeddedIdEnum newEmbeddedID)
{
	SvOp::TableCopyObject* pSourceTableOwn = dynamic_cast<SvOp::TableCopyObject*>(m_sourceTableObjectInfo.GetInputObjectInfo().getFinalObject());
	if (pOwnerObject == pSourceTableOwn && nullptr != m_pResultTable)
	{
		m_pResultTable->changeEmbeddedId(oldEmbeddedID, newEmbeddedID);
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
bool TableAnalyzerTool::ValidateLocal(SvStl::MessageContainerVector * pErrorMessages) const
{
	bool Result = true;
	if (nullptr == m_pResultTable)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(message);
		}
	}

	return Result;
}

void TableAnalyzerTool::LocalInitialize()
{
	BuildInputObjectList();

	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVTableAnalyzerToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	SvIe::SVClassInfoStruct analyzerClassInfo;

	// Add the Sort Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::TableAnalyzerSortType;
	analyzerClassInfo.m_ClassId = SvPb::TableAnaylzerSortClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_SORT);
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Exclude Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::TableAnalyzerExcludeType;
	analyzerClassInfo.m_ClassId = SvPb::TableAnaylzerExcludeClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_EXCLUDE);
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Limit Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::TableAnalyzerLimitType;
	analyzerClassInfo.m_ClassId = SvPb::TableAnaylzerLimitClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_LIMIT);
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Add Column Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::TableAnalyzerAddColumnType;
	analyzerClassInfo.m_ClassId = SvPb::TableAnaylzerAddColumnClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_ADDCOLUMN);
	m_availableChildren.push_back(analyzerClassInfo);


	// Setup the result
	SvIe::SVClassInfoStruct resultClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;
	// Declare Input Interface
	interfaceInfo.m_EmbeddedID = SvPb::TableObject_NumberOfRowsEId;
	resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultLongObjectType;
	resultClassInfo.m_ClassId = SvPb::LongResultClassId;
	resultClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS);
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT);
	resultClassInfo.m_ClassName += _T(" ") + Title;
	// Construct the result
	SvOp::SVLongResult* pResult = dynamic_cast<SvOp::SVLongResult*> (resultClassInfo.Construct());
	Add(pResult);
}

void TableAnalyzerTool::BuildInputObjectList()
{
	// Source Table.
	m_sourceTableObjectInfo.SetInputObjectType(SvPb::TableObjectType);
	m_sourceTableObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_sourceTableObjectInfo, SvDef::cInputTag_SourceTable);
	
	addDefaultInputObjects();
}

int TableAnalyzerTool::calcNewColumnPosition(const SVTaskObjectClass* pAnalyzer)
{
	int pos = 0;
	for (auto pTask : m_TaskObjectVector)
	{
		if (pAnalyzer == pTask)
		{
			break;
		}
		if (nullptr != dynamic_cast<const SvAo::TableAddColumnAnalyzer*>(pTask))
		{
			pos++;
		}
	}	
	return pos;
}

#pragma endregion Private Methods

} //namespace SvTo
