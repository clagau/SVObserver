//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TableFillObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table and fill it with equation results e.g. for TableTool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableFillByEquationObject.h"
#include "TableColumnEquation.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableFillByEquationObject, SvPb::TableFillObjectId);


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
	m_spSortContainer->set_capacity(maxArray);

	std::vector<TableColumnEquation*> columnList = rColumnEquationList;

	//after the load the m_equationList can be empty and m_ValueList has already loaded values. In this case copy the "loaded" equation to the m_equationList.
	if (0 == m_equationList.size() && 0 < m_ValueList.size())
	{
		m_equationList.insert(m_equationList.begin(), columnList.begin(), columnList.begin()+m_ValueList.size());
	}
	Log_Assert(m_equationList.size() == m_ValueList.size());

	int i = 0;
	for (std::vector<TableColumnEquation*>::iterator forIter = columnList.begin(); columnList.end() != forIter; ++forIter, i++)
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
				SvPb::EmbeddedIdEnum newId = getNextFreeEmbeddedColumID();
				if (SvPb::NoEmbeddedId == newId)
				{
					SvStl::MessageManager e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValue_NoFreeID, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					e.Throw();
				}
				createColumnObject(newId, (*forIter)->GetName(), maxArray);
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
		SvVol::DoubleSortValuePtr pValueObject = m_ValueList.back();
		m_equationList.pop_back();
		m_ValueList.pop_back();
		RemoveEmbeddedObject(pValueObject.get());
		//Object must be deleted, before SetDefaultInputs is called.
		pValueObject.reset();
		GetInspectionInterface()->SetDefaultInputs();
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableFillByEquationObject::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	if (returnValue)
	{
		int nextPos = 0;
		if (!m_spSortContainer->empty())
		{
			nextPos = (m_spSortContainer->at(0)+1)%(m_spSortContainer->capacity());
		}
		m_spSortContainer->push_front(nextPos);

		for (int i = 0; i < m_equationList.size(); ++i )
		{
			SvVol::DoubleSortValueObject* pValueObject = (m_ValueList[i]).get();
			TableColumnEquation* pEquation = m_equationList[i];
			if (nullptr != pValueObject && nullptr != pEquation)
			{
				pValueObject->setSortContainerPtr(m_spSortContainer);
				pValueObject->SetValue(pEquation->GetYACCResult(), 0 );
			}
		}

		m_NumberOfRows.SetValue(static_cast<long>(m_spSortContainer->size()));
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableFillByEquationObject::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::TableObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::TableFillObjectType;
}
#pragma endregion Private Methods

} //namespace SvOp
