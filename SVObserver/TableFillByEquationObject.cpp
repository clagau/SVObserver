//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableFillObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table and fill it with equation results e.g. for TableTool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableFillByEquationObject.h"
#include "SVOCore/SVTool.h"
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

SV_IMPLEMENT_CLASS(TableFillByEquationObject, TableFillObjectGuid);

#pragma endregion Declarations

#pragma region Constructor
TableFillByEquationObject::TableFillByEquationObject( LPCTSTR ObjectName )
	: TableObject(ObjectName)
{
	Initialize();
}

TableFillByEquationObject::TableFillByEquationObject( SVObjectClass* pOwner, int StringResourceID )
	: TableObject(pOwner, StringResourceID)
{
	Initialize();
}

TableFillByEquationObject::~TableFillByEquationObject()
{
	m_equationList.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
void TableFillByEquationObject::setColumnValueObjects(const std::vector<TableColumnEquation*>& rColumnEquationList, int maxArray)
{
	m_sortContainer.set_capacity(maxArray);

	std::vector<TableColumnEquation*> columnList = rColumnEquationList;

	//after the load the m_equationList can be empty and m_ValueList has already loaded values. In this case copy the "loaded" equation to the m_equationList.
	if (0 == m_equationList.size() && 0 < m_ValueList.size())
	{
		m_equationList.insert(m_equationList.begin(), columnList.begin(), columnList.begin()+m_ValueList.size());
	}
	ASSERT(m_equationList.size() == m_ValueList.size());

	for (int i=0; i<m_equationList.size(); i++)
	{
		std::vector<TableColumnEquation*>::iterator vecIter = std::find(columnList.begin(), columnList.end(), m_equationList[i]);
		if (columnList.end() != vecIter)
		{
			columnList.erase(vecIter);
			//value already exist, update the name and size if needed.
			DoubleSortValueObject* pValueObject = m_ValueList[i].get();
			if (nullptr != pValueObject)
			{
				SVString objectName = m_equationList[i]->GetName();
				if (pValueObject->GetName() != objectName)
				{
					SVString OldName = pValueObject->GetName();
					pValueObject->SetName(objectName.c_str());
					GetInspection()->OnObjectRenamed(*pValueObject, OldName );
				}
				pValueObject->SetArraySize(maxArray);
			}
			else
			{
				SvStl::MessageMgrStd e( SvStl::DataOnly );
				e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValueMapInvalid, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				e.Throw();
			}
		}
		else
		{
			//erase not longer used value object
			DoubleSortValuePtr pValueObject = m_ValueList[i];
			std::vector<TableColumnEquation*>::iterator equationIter = m_equationList.begin()+i;
			std::vector<DoubleSortValuePtr>::iterator valueIter =m_ValueList.begin()+i;
			m_equationList.erase(equationIter);
			m_ValueList.erase(valueIter);
			i--;
			RemoveEmbeddedObject(pValueObject.get());
			//Object must be deleted, before SetDefaultInputs is called.
			pValueObject.reset();
			dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
		}
	}

	//add remaining elements from vector to map
	for (std::vector<TableColumnEquation*>::iterator iter = columnList.begin(); iter != columnList.end(); ++iter )
	{
		SVGUID newGuid = getNextFreeEmbeddedColumGUID();
		if (SV_GUID_NULL == newGuid)
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValue_NoFreeGUID, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			e.Throw();
		}
		createColumnObject(newGuid, (*iter)->GetName(), maxArray);
		m_equationList.push_back(*iter);
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableFillByEquationObject::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	if (returnValue)
	{
		int nextPos = 0;
		if (!m_sortContainer.empty())
		{
			nextPos = (m_sortContainer[0]+1)%(m_sortContainer.capacity());
		}
		m_sortContainer.push_front(nextPos);

		for (int i = 0; i < m_equationList.size(); ++i )
		{
			DoubleSortValueObject* pValueObject = (m_ValueList[i]).get();
			TableColumnEquation* pEquation = m_equationList[i];
			if (nullptr != pValueObject && nullptr != pEquation)
			{
				pValueObject->setSortContainer(m_sortContainer);
				pValueObject->SetValue(pEquation->GetYACCResult(), 0 );
			}
		}

		m_NumberOfRows.SetValue(static_cast<long>(m_sortContainer.size()));
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableFillByEquationObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = TableObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = TableFillObjectType;
}

SVGUID TableFillByEquationObject::getNextFreeEmbeddedColumGUID()
{
	for (int i=0; i < c_maxTableColumn; ++i)
	{
		std::vector<DoubleSortValuePtr>::const_iterator it = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const DoubleSortValuePtr& entry)->bool 
		{ 
			return (nullptr != entry.get() && entry->GetEmbeddedID() == TableColumnValueObjectGuid[i]); 
		} 
		);
		if (m_ValueList.end() == it)
		{	//GUID not used yet
			return TableColumnValueObjectGuid[i];
		}
	}
	return SV_GUID_NULL;
}

#pragma endregion Private Methods
