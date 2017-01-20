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
#include "SVTool.h"
#include "SVInspectionProcess.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVUtilityLibrary\SVString.h"
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
HRESULT TableCopyObject::ResetObject()
{
	if (nullptr != m_pSourceTable)
	{
		std::vector<DoubleSortValuePtr> SourceValues = m_pSourceTable->getValueList();
		for (int i=0; i<m_ValueList.size(); ++i)
		{
			std::vector<DoubleSortValuePtr>::const_iterator it = std::find_if(SourceValues.begin(), SourceValues.end(), [&](const DoubleSortValuePtr& entry)->bool 
			{ 
				return (nullptr != entry.get() && entry->GetEmbeddedID() == m_ValueList[i]->GetEmbeddedID()); 
			} 
			);

			if (SourceValues.end() != it && nullptr != it->get())
			{	//value already exist, rename and resize object if needed.
				if ((*it)->GetName() != SVString(m_ValueList[i]->GetName()))
				{
					SVString OldName = m_ValueList[i]->GetName();
					m_ValueList[i]->SetName((*it)->GetName());
					GetInspection()->OnObjectRenamed(*(m_ValueList[i].get()), OldName );
				}
				if ((*it)->GetArraySize() != m_ValueList[i]->GetArraySize())
				{
					m_ValueList[i]->SetArraySize((*it)->GetArraySize());
				}
				SourceValues.erase(it);
			}
			else
			{	//value not longer needed, delete it.
				DoubleSortValuePtr pValueObject = m_ValueList[i];
				std::vector<DoubleSortValuePtr>::iterator valueIter =m_ValueList.begin()+i;
				m_ValueList.erase(valueIter);
				i--;
				RemoveEmbeddedObject(pValueObject.get());
				//Object must be deleted, before SetDefaultInputs is called.
				pValueObject.reset();
				dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
			}
		}
		
		//Add new value columns
		for (std::vector<DoubleSortValuePtr>::const_iterator iter = SourceValues.cbegin(); iter != SourceValues.cend(); ++iter )
		{
			DoubleSortValueObject* pSourceObject = (*iter).get();
			if (nullptr != pSourceObject)
			{
				try
				{
					createColumnObject(pSourceObject->GetEmbeddedID(), pSourceObject->GetName(), pSourceObject->GetArraySize());
				}
				catch( const SvStl::MessageContainer& rSvE )
				{
					SvStl::MessageMgrStd e( SvStl::LogOnly );
					e.setMessage( rSvE.getMessage() );
				}
				
			}
			else
			{
				SvStl::MessageMgrStd e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams) );
				ASSERT(FALSE);
			}
		}
	}
	else
	{
		while(0 < m_ValueList.size())
		{
			DoubleSortValuePtr pObject = m_ValueList.back();
			m_ValueList.pop_back();
			RemoveEmbeddedObject(pObject.get());
		}
		dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
	}

	HRESULT hr = TableObject::ResetObject();

	return hr;
}

void TableCopyObject::setSortContainer(const ValueObjectSortContainer& sortMap, SVRunStatusClass& rRunStatus)
{
	m_sortContainer = sortMap;
	for (int i=0; i<m_ValueList.size(); ++i)
	{
		m_ValueList[i]->setSortContainer(rRunStatus.m_lResultDataIndex, m_sortContainer);
	}
	m_NumberOfRows.SetValue(rRunStatus.m_lResultDataIndex, static_cast<long>(sortMap.size()));
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL TableCopyObject::onRun( SVRunStatusClass& rRunStatus )
{
	BOOL returnValue = S_FALSE;

	returnValue = TableObject::onRun( rRunStatus );
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
						object->setSortContainer(rRunStatus.m_lResultDataIndex, m_sortContainer);
						for (int i=0; i<m_sortContainer.size(); ++i)
						{
							double value = 0;
							if (S_OK == (*itSource)->GetValue(rRunStatus.m_lResultDataIndex, i, value))
							{
								object->SetValue(rRunStatus.m_lResultDataIndex, i, value);
							}
						}
					}
				}
				
				m_NumberOfRows.SetValue(rRunStatus.m_lResultDataIndex, static_cast<long>(m_sortContainer.size()));
			}
			else
			{
				m_NumberOfRows.SetValue(rRunStatus.m_lResultDataIndex, 0);
				returnValue = E_FAIL;
			}
		}
		else
		{
			returnValue = E_FAIL;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableCopy_Nullptr, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
		}
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableCopyObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = TableCopyObjectType;
}
#pragma endregion Private Methods
