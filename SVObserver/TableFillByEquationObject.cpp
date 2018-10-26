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
#include "InspectionEngine/SVTool.h"
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

	int i = 0;
	for (std::vector<TableColumnEquation*>::iterator forIter = columnList.begin(); columnList.end() != forIter; forIter++, i++)
	{
		//check if column at position i different between old equation list and new one
		if (m_equationList.size() <=i || *forIter != *(m_equationList.begin()+i))
		{
			std::vector<TableColumnEquation*>::iterator findIter = std::find(m_equationList.begin(), m_equationList.end(), *forIter);
			if (m_equationList.end() != findIter)
			{
				//move to new position
				auto oldPos = std::distance(m_equationList.begin(), findIter);
				m_equationList.erase(findIter);
				m_equationList.insert(m_equationList.begin() + i, *forIter);
				MoveValueColumn(static_cast<int>(oldPos), i);
				UpdateColumnValueObject(i, m_equationList[i]->GetName(), maxArray);
			}
			else
			{
				//new column, add it.
				SVGUID newGuid = getNextFreeEmbeddedColumGUID();
				if (GUID_NULL == newGuid)
				{
					SvStl::MessageMgrStd e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValue_NoFreeGUID, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					e.Throw();
				}
				createColumnObject(newGuid, (*forIter)->GetName(), maxArray);
				m_equationList.insert(m_equationList.begin()+i, *forIter);
				//move object from last position to wanted
				MoveValueColumn(static_cast<int>(m_ValueList.size()-1), i);
			}
		}
		else
		{
			UpdateColumnValueObject(i, m_equationList[i]->GetName(), maxArray);
		}
	}

	while (m_equationList.size() > columnList.size())
	{
		//erase not longer used value object
		DoubleSortValuePtr pValueObject = m_ValueList.back();
		m_equationList.pop_back();
		m_ValueList.pop_back();
		RemoveEmbeddedObject(pValueObject.get());
		//Object must be deleted, before SetDefaultInputs is called.
		pValueObject.reset();
		dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::TableObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvDef::TableFillObjectType;
}
#pragma endregion Private Methods
