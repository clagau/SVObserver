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
#include "SVOCore/SVTool.h"
#include "SVInspectionProcess.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(TableCopyObject, TableCopyObjectGuid);

#pragma endregion Declarations

#pragma region Constructor
TableCopyObject::TableCopyObject( LPCTSTR ObjectName )
	: TableObject(ObjectName)
	, m_pSourceTable(nullptr)
{
	Initialize();
}

TableCopyObject::TableCopyObject( SVObjectClass* POwner, int StringResourceID )
	: TableObject(POwner, StringResourceID)
	, m_pSourceTable(nullptr)
{
	Initialize();
}

TableCopyObject::~TableCopyObject()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableCopyObject::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	if (nullptr != m_pSourceTable)
	{
		std::vector<DoubleSortValuePtr> SourceValues = m_pSourceTable->getValueList();
		int i = 0;
		for (std::vector<DoubleSortValuePtr>::iterator forIter = SourceValues.begin(); SourceValues.end() != forIter; forIter++, i++)
		{
			std::vector<DoubleSortValuePtr>::iterator findIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const DoubleSortValuePtr& entry)->bool
			{
				return (nullptr != entry.get() && nullptr != forIter->get() && entry->GetEmbeddedID() == forIter->get()->GetEmbeddedID());
			}
			);

			if (m_ValueList.end() != findIter)
			{
				auto oldPos = std::distance(m_ValueList.begin(), findIter);
				if (oldPos != i)
				{
					MoveValueColumn(static_cast<int>(oldPos), i);
				}
				UpdateColumnValueObject(i, (*forIter)->GetName(), (*forIter)->getArraySize());
			}
			else
			{
				//new column, add it.
				createColumnObject((*forIter)->GetEmbeddedID(), (*forIter)->GetName(), (*forIter)->getArraySize());
				//move object from last position to wanted
				MoveValueColumn(static_cast<int>(m_ValueList.size() - 1), i);
			}
		}

		while (m_ValueList.size() > SourceValues.size())
		{
			//erase not longer used value object
			DoubleSortValuePtr pValueObject = m_ValueList.back();
			m_ValueList.pop_back();
			if (nullptr != pValueObject.get())
			{
				hideEmbeddedObject(*pValueObject.get());
				RemoveEmbeddedObject(pValueObject.get());
			}
		}
	}

	return TableObject::ResetObject(pErrorMessages);
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableCopyObject::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	if (returnValue)
	{
		if (nullptr != m_pSourceTable)
		{
			const std::vector<DoubleSortValuePtr>& rSourceValues = m_pSourceTable->getValueList();
			if (!rSourceValues.empty())
			{
				m_sortContainer = m_pSourceTable->getSortContainer();

				for (std::vector<DoubleSortValuePtr>::const_iterator itSource = rSourceValues.begin();rSourceValues.end() != itSource; ++itSource)
				{
					std::vector<DoubleSortValuePtr>::const_iterator itDest = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const DoubleSortValuePtr& entry)->bool 
					{ 
						return (nullptr != entry.get() && entry->GetEmbeddedID() == (*itSource)->GetEmbeddedID()); 
					} 
					);
					//copy Values
					if (m_ValueList.end() != itDest && nullptr != itDest->get())
					{
						DoubleSortValueObject* object = itDest->get();
						object->setSortContainer(m_sortContainer);
						for (int i=0; i<m_sortContainer.size(); ++i)
						{
							double Value( 0.0 );
							if (S_OK == (*itSource)->GetValue( Value, i ))
							{
								object->SetValue( Value, i );
							}
						}
					}
				}
				
				m_NumberOfRows.SetValue(static_cast<long>(m_sortContainer.size()));
			}
			else
			{
				m_NumberOfRows.SetValue(0L);
				returnValue = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_EmptyValueList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			returnValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableCopy_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = TableObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = TableCopyObjectType;
}
#pragma endregion Private Methods
