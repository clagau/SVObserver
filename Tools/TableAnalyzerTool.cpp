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
#include "SVObjectLibrary/SVClsids.h"
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

SV_IMPLEMENT_CLASS(TableAnalyzerTool, TableAnalyzerToolGuid);

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

	m_pResultTable = dynamic_cast<SvOp::TableCopyObject*>(SvOi::FindObject(GetUniqueObjectID(), SvDef::SVObjectTypeInfoStruct(SvPb::TableObjectType, SvPb::TableCopyObjectType)));
	if (nullptr == m_pResultTable)
	{
		m_pResultTable = new SvOp::TableCopyObject(this);
		//This task must be the first in the list.
		Add(m_pResultTable, true);
		if (!CreateChildObject(m_pResultTable))
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
			Msg.setMessage(message.getMessage());
		}
	}

	// add input of result
	SvOp::SVLongResultClass* pResult = dynamic_cast<SvOp::SVLongResultClass*>(SvOi::FindObject(GetUniqueObjectID(), SvDef::SVObjectTypeInfoStruct(SvPb::SVResultObjectType, SvPb::SVResultLongObjectType)));
	if (nullptr != pResult)
	{
		SvOl::SVInputInfoListClass inputInterface;
		pResult->GetPrivateInputList(inputInterface);
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
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
			Msg.setMessage(message.getMessage());
		}
	}
	else
	{
		bOk = false;
		SvStl::MessageContainer message;
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
		Msg.setMessage(message.getMessage());
	}

	// These values will not be exposed for the this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	m_Index.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);

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

	SVObjectClass* pObject = m_sourceTableObjectInfo.GetInputObjectInfo().getObject();
	if (!m_sourceTableObjectInfo.IsConnected() || nullptr == dynamic_cast<SvOp::TableObject*> (pObject))
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(message);
		}
	}
	else if (pObject->GetAncestorInterface(SvPb::SVInspectionObjectType) != GetAncestorInterface(SvPb::SVInspectionObjectType))
	{
		pObject->DisconnectObjectInput(&m_sourceTableObjectInfo);
		m_sourceTableObjectInfo.SetInputObject(nullptr);
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(message);
		}
	}

	if (Result)
	{
		m_pResultTable->setSourecTable(dynamic_cast<SvOp::TableObject*> (m_sourceTableObjectInfo.GetInputObjectInfo().getObject()));
		Result = m_pResultTable->ResetObject(pErrorMessages);
	}
	else
	{
		m_pResultTable->setSourecTable(nullptr);
	}

	return Result;
}

void TableAnalyzerTool::InsertAt(int index, ITaskObject& rObject, int count)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	SVTaskObjectListClass::InsertAt(index + 1, pObject, count);
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

void TableAnalyzerTool::OnEmbeddedIDChanged(const SVObjectClass* pOwnerObject, const SVGUID& rOldEmbeddedID, const SVGUID& rNewEmbeddedID)
{
	SvOp::TableCopyObject* pSourceTableOwn = dynamic_cast<SvOp::TableCopyObject*>(m_sourceTableObjectInfo.GetInputObjectInfo().getObject());
	if (pOwnerObject == pSourceTableOwn && nullptr != m_pResultTable)
	{
		m_pResultTable->changeEmbeddedId(rOldEmbeddedID, rNewEmbeddedID);
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
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(message);
		}
	}

	return Result;
}

void TableAnalyzerTool::LocalInitialize()
{
	// Override base class exposure of the auxiliary extent variables
	// These values will not be exposed for this Tool.
	m_hasToolExtents = false;

	BuildInputObjectList();
	RegisterEmbeddedObject(&m_Index, TableAnalyzerIndexObjectGuid, IDS_OBJECTNAME_INDEXVARIABLE, false, SvOi::SVResetItemNone);
	m_Index.setSaveValueFlag(false);

	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVTableAnalyzerToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	SvIe::SVClassInfoStruct analyzerClassInfo;

	// Add the Sort Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::TableAnalyzerSortType;
	analyzerClassInfo.m_ClassId = TableAnaylzerSortGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_SORT);
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Exclude Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::TableAnalyzerExcludeType;
	analyzerClassInfo.m_ClassId = TableAnaylzerExcludeGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_EXCLUDE);
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Limit Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::TableAnalyzerLimitType;
	analyzerClassInfo.m_ClassId = TableAnaylzerLimitGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_LIMIT);
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Add Column Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = SvPb::TableAnalyzerAddColumnType;
	analyzerClassInfo.m_ClassId = TableAnaylzerAddColumnGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString(IDS_CLASSNAME_TABLEANALYZER_ADDCOLUMN);
	m_availableChildren.push_back(analyzerClassInfo);


	// Setup the result
	SvIe::SVClassInfoStruct resultClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;
	// Declare Input Interface
	interfaceInfo.EmbeddedID = TableObject_NumberOfRowsGuid;
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVResultLongObjectType;
	resultClassInfo.m_ClassId = SVLongResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS);
	std::string Title = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT);
	resultClassInfo.m_ClassName += _T(" ") + Title;
	// Construct the result
	SvOp::SVLongResultClass* pResult = dynamic_cast<SvOp::SVLongResultClass*> (resultClassInfo.Construct());
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
