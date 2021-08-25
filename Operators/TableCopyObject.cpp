//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableCopyObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table and copy the value from another table, used e.g. for TableAnaylzerTool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableCopyObject.h"
#include "Tools/SVTool.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableCopyObject, SvPb::TableCopyObjectId);


#pragma region Constructor
TableCopyObject::TableCopyObject(LPCTSTR ObjectName)
	: TableObject(ObjectName)
	, m_pSourceTable(nullptr)
{
	Initialize();
}

TableCopyObject::TableCopyObject(SVObjectClass* POwner, int StringResourceID)
	: TableObject(POwner, StringResourceID)
	, m_pSourceTable(nullptr)
{
	Initialize();
}

TableCopyObject::~TableCopyObject()
{
	m_NewValueList.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableCopyObject::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool retValue = true;
	if (m_isCreated && nullptr != m_pSourceTable)
	{
		std::vector<SvVol::DoubleSortValuePtr> SourceValues = m_pSourceTable->getValueList();
		if (SourceValues.size() + m_NewValueList.size() <= SvDef::c_maxTableColumn)
		{
			//remove all entries from m_ValueList which are not in sourceTable and not in the m_NewValueList
			RemoveUnusedColumn();

			//Reset the column of the sourceColumn
			int valueListPos = ResetCopyColumn();

			//Reset the column of the newColumn list.
			int arraySize = 1;
			if (0 < SourceValues.size())
			{
				arraySize = SourceValues[0]->getArraySize();
			}
			retValue = ResetNewColumns(valueListPos, arraySize, pErrorMessages);
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValue_NoFreeID, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}

	return retValue && TableObject::ResetObject(pErrorMessages);
}

SvVol::DoubleSortValuePtr TableCopyObject::addNewColumn(LPCTSTR, int pos)
{
	SvVol::DoubleSortValueObject* pObject = nullptr;
	SVObjectManagerClass::Instance().ConstructObject(SvPb::DoubleSortValueClassId, pObject);
	SvVol::DoubleSortValuePtr pSharedObject = SvVol::DoubleSortValuePtr {pObject};

	if (CreateChildObject(pObject))
	{
		AddEntryToNewValueList(pos, pSharedObject);
	}
	else
	{
		pSharedObject.reset();
		assert(false);
	}
	return pSharedObject;
}

SvVol::DoubleSortValuePtr TableCopyObject::setNewColumn(const SVObjectClass* pColumn, int pos)
{
	SvVol::DoubleSortValuePtr pRetColumn;

	auto pIter = find_if(m_ValueList.begin(), m_ValueList.end(), [&pColumn](const SvVol::DoubleSortValuePtr& entry)->bool
	{
		return (pColumn == entry.get());
	}
	);
	if (m_ValueList.end() != pIter)
	{
		pRetColumn = *pIter;
		AddEntryToNewValueList(pos, pRetColumn);
	}

	return pRetColumn;
}

void TableCopyObject::removeNewColumn(const SvVol::DoubleSortValuePtr pColumn)
{
	if (nullptr != pColumn)
	{
		std::vector<SvVol::DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_NewValueList.begin(), m_NewValueList.end(), [&pColumn](const SvVol::DoubleSortValuePtr& entry)->bool
		{
			return (nullptr != entry.get() && entry->GetEmbeddedID() == pColumn->GetEmbeddedID());
		}
		);
		if (m_NewValueList.end() != valueIter)
		{
			SvVol::DoubleSortValuePtr pValueObject = *valueIter;
			m_NewValueList.erase(valueIter);

			//Object must be deleted, before SetDefaultInputs is called.
			pValueObject.reset();
			GetInspectionInterface()->SetDefaultInputs();
		}
	}
}

void TableCopyObject::changeEmbeddedId(SvPb::EmbeddedIdEnum oldId, SvPb::EmbeddedIdEnum newId)
{
	std::vector<SvVol::DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&oldId](const SvVol::DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == oldId);
	});
	if (m_ValueList.end() != valueIter)
	{
		auto newIter = std::find(m_NewValueList.begin(), m_NewValueList.end(), *valueIter);
		//only change the ID, if it is not a new column object.
		if (m_NewValueList.end() == newIter)
		{
			//remove newColumn entry from embeddedList, because the copy column need this embeddedID in this list.
			SVObjectPtrVector::iterator Iter = find(m_embeddedList.begin(), m_embeddedList.end(), valueIter->get());
			if (m_embeddedList.end() != Iter)
			{
				m_embeddedList.erase(Iter);
			}

			//check if newColumn use this embedded ID
			newIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&newId](const SvVol::DoubleSortValuePtr& entry)->bool
			{
				return (nullptr != entry.get() && entry->GetEmbeddedID() == newId);
			});
			if (m_ValueList.end() != newIter)
			{	//if new embedded Id already used by a new column object, remove it from the embedded and the valueList. It will be reentered by OnReset.
				SVObjectPtrVector::iterator emListIter = find(m_embeddedList.begin(), m_embeddedList.end(), newIter->get());
				if (m_embeddedList.end() != emListIter)
				{
					m_embeddedList.erase(emListIter);
				}
				m_ValueList.erase(newIter);
			}

			std::string  name = valueIter->get()->GetName();
			RegisterEmbeddedObject(valueIter->get(), newId, valueIter->get()->GetObjectName(), true, SvOi::SVResetItemTool);
			valueIter->get()->SetName(name.c_str());
			sendChangedEmbeddedIDToUser(oldId, newId);
		}
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableCopyObject::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool returnValue = __super::onRun(rRunStatus, pErrorMessages);
	if (returnValue)
	{
		if (nullptr != m_pSourceTable)
		{
			const std::vector<SvVol::DoubleSortValuePtr>& rSourceValues = m_pSourceTable->getValueList();
			if (!rSourceValues.empty())
			{
				setSortContainerDummy(SvVol::DummySortContainer(0));
				
				std::vector<double> values;
				for (std::vector<SvVol::DoubleSortValuePtr>::const_iterator itSource = rSourceValues.begin(); rSourceValues.end() != itSource; ++itSource)
				{
					std::vector<SvVol::DoubleSortValuePtr>::const_iterator itDest = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
					{
						return (nullptr != entry.get() && entry->GetEmbeddedID() == (*itSource)->GetEmbeddedID());
					}
					);
					//copy Values
					if (m_ValueList.end() != itDest && nullptr != itDest->get())
					{
						SvVol::DoubleSortValueObject* object = itDest->get();
						(*itSource)->GetArrayValues(values);
						object->SetArrayValues(values);
					}
				}
				setSortContainerDummy(SvVol::DummySortContainer(values.size()));
			}
			else
			{
				m_NumberOfRows.SetValue(0L);
				returnValue = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_EmptyValueList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			returnValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableCopy_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableCopyObject::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::TableObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::TableCopyObjectType;
}

void TableCopyObject::RemoveUnusedColumn()
{
	std::vector<SvVol::DoubleSortValuePtr> SourceValues = m_pSourceTable->getValueList();
	for (int i = static_cast<int>(m_ValueList.size()) - 1; i >= 0; i--)
	{
		std::vector<SvVol::DoubleSortValuePtr>::iterator findIter = std::find_if(SourceValues.begin(), SourceValues.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
		{
			return (nullptr != entry.get() && nullptr != m_ValueList[i] && entry->GetEmbeddedID() == m_ValueList[i]->GetEmbeddedID());
		}
		);
		if (SourceValues.end() == findIter)
		{
			findIter = std::find(m_NewValueList.begin(), m_NewValueList.end(), m_ValueList[i]);
			if (m_NewValueList.end() == findIter)
			{
				//erase not longer used value object
				SvVol::DoubleSortValuePtr pValueObject = m_ValueList[i];
				m_ValueList.erase(m_ValueList.begin() + i);
				if (nullptr != pValueObject.get())
				{
					RemoveEmbeddedObject(pValueObject.get());
				}
			}
		}
	}
}

int TableCopyObject::ResetCopyColumn()
{
	int valueListPos = -1;
	std::vector<SvVol::DoubleSortValuePtr> SourceValues = m_pSourceTable->getValueList();
	for (const auto pSourceColumn : SourceValues)
	{
		valueListPos++;
		std::vector<SvVol::DoubleSortValuePtr>::iterator findIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
		{
			return (nullptr != entry.get() && nullptr != pSourceColumn && entry->GetEmbeddedID() == pSourceColumn->GetEmbeddedID());
		}
		);

		if (m_ValueList.end() != findIter)
		{
			std::vector<SvVol::DoubleSortValuePtr>::iterator newColumnIter = std::find(m_NewValueList.begin(), m_NewValueList.end(), *findIter);
			if (m_NewValueList.end() == newColumnIter)
			{
				auto oldPos = std::distance(m_ValueList.begin(), findIter);
				if (oldPos != valueListPos)
				{
					MoveValueColumn(static_cast<int>(oldPos), valueListPos);
				}
				UpdateColumnValueObject(valueListPos, pSourceColumn->GetName(), pSourceColumn->getArraySize());
			}
			else
			{
				//remove newColumn entry from embeddedList, because the copy column need this embeddedID in this list.
				SVObjectPtrVector::iterator Iter = find(m_embeddedList.begin(), m_embeddedList.end(), newColumnIter->get());
				if (m_embeddedList.end() != Iter)
				{
					m_embeddedList.erase(Iter);
				}
				//new column, add it.
				createColumnObject(pSourceColumn->GetEmbeddedID(), pSourceColumn->GetName(), pSourceColumn->getArraySize());
				//move object from last position to wanted
				MoveValueColumn(static_cast<int>(m_ValueList.size() - 1), valueListPos);

				//create new embeddedID for newColumn and register it.
				SvPb::EmbeddedIdEnum newEmbeddedId = getNextFreeEmbeddedColumID();
				if (SvPb::NoEmbeddedId != newEmbeddedId)
				{
					std::string  name = newColumnIter->get()->GetName();
					RegisterEmbeddedObject(newColumnIter->get(), newEmbeddedId, newColumnIter->get()->GetObjectName(), true, SvOi::SVResetItemTool);
					newColumnIter->get()->SetName(name.c_str());
					sendChangedEmbeddedIDToUser(pSourceColumn->GetEmbeddedID(), newEmbeddedId);
				}
				else
				{
					assert(false); //the number of column should be checked already in ResetObject, so this part should be never reached. 
				}
			}
		}
		else
		{
			//new column, add it.
			createColumnObject(pSourceColumn->GetEmbeddedID(), pSourceColumn->GetName(), pSourceColumn->getArraySize());
			//move object from last position to wanted
			MoveValueColumn(static_cast<int>(m_ValueList.size() - 1), valueListPos);
		}
	}
	return valueListPos;
}

bool TableCopyObject::ResetNewColumns(int valueListPos, int arraySize, SvStl::MessageContainerVector * pErrorMessages)
{
	bool retValue = true;
	for (const auto pNewColumn : m_NewValueList)
	{
		valueListPos++;
		//check if name is unique
		auto searchEnd = m_ValueList.begin() + valueListPos;
		auto nameIter = std::find_if(m_ValueList.begin(), searchEnd, [&pNewColumn](const SvVol::DoubleSortValuePtr& entry)->bool
		{
			return (std::string(pNewColumn->GetName()) == entry->GetName());
		});
		if (searchEnd != nameIter)
		{
			if (nullptr != pErrorMessages)
			{
				SvDef::StringVector msgList;
				msgList.push_back(pNewColumn->GetName());
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableColumnName_NotUnique, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			retValue = false;
		}

		//check if new Column object already in m_ValueList
		std::vector<SvVol::DoubleSortValuePtr>::iterator findIter = std::find(m_ValueList.begin(), m_ValueList.end(), pNewColumn);
		if (m_ValueList.end() != findIter)
		{
			auto oldPos = std::distance(m_ValueList.begin(), findIter);
			if (oldPos > valueListPos)
			{
				MoveValueColumn(static_cast<int>(oldPos), valueListPos);
			}

			UpdateColumnValueObject(valueListPos, pNewColumn->GetName(), arraySize);
		}
		else
		{
			SvPb::EmbeddedIdEnum embeddedId = getNextFreeEmbeddedColumID();
			std::string name = pNewColumn.get()->GetName();
			RegisterEmbeddedObject(pNewColumn.get(), embeddedId, pNewColumn.get()->GetObjectName(), true, SvOi::SVResetItemTool);
			pNewColumn.get()->SetName(name.c_str());
			m_ValueList.push_back(pNewColumn);
			//move object from last position to wanted
			MoveValueColumn(static_cast<int>(m_ValueList.size() - 1), valueListPos);
		}
	}
	return retValue;
}

void TableCopyObject::sendChangedEmbeddedIDToUser(SvPb::EmbeddedIdEnum oldEmbeddedId, SvPb::EmbeddedIdEnum newEmbeddedId)
{
	std::set<uint32_t> dependencyList;
	std::set<uint32_t> SourceSet;
	SourceSet.insert(getObjectId());
	SvOi::getToolDependency(std::inserter(dependencyList, dependencyList.begin()), SourceSet);
	for (auto id : dependencyList)
	{
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(SvOi::getObject(id));
		if(nullptr != pTool)
		{
			pTool->OnEmbeddedIDChanged(this, oldEmbeddedId, newEmbeddedId);
		}
	}
}

void TableCopyObject::AddEntryToNewValueList(int pos, SvVol::DoubleSortValuePtr pColumn)
{
	if (0 <= pos && m_NewValueList.size() > pos)
	{
		m_NewValueList.insert(m_NewValueList.begin() + pos, pColumn);
	}
	else
	{
		m_NewValueList.push_back(pColumn);
	}
}

#pragma endregion Private Methods

} //namespace SvOp
