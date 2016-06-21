//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableFillObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table and fill it with equation results e.g. for TableTool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableFillObject.h"
#include "SVTool.h"
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

SV_IMPLEMENT_CLASS(TableFillObject, TableFillObjectGuid);

#pragma endregion Declarations

#pragma region Constructor
TableFillObject::TableFillObject( LPCTSTR ObjectName )
	: TableObject(ObjectName)
{
	Initialize();
}

TableFillObject::TableFillObject( SVObjectClass* pOwner, int StringResourceID )
	: TableObject(pOwner, StringResourceID)
{
	Initialize();
}

TableFillObject::~TableFillObject()
{
	m_equationList.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
void TableFillObject::setColumnValueObjects(const std::vector<TableColumnEquation*>& rColumnEquationList, int maxArray)
{
	m_maxArray = maxArray;
	m_sortContainer.set_capacity(maxArray);

	std::vector<TableColumnEquation*> columnList = rColumnEquationList;

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
					::SVSendMessage( GetInspection(), SVM_OBJECT_RENAMED, reinterpret_cast <DWORD_PTR> ( static_cast <SVObjectClass*> (pValueObject) ), reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>( OldName.c_str() )) );
				}
				pValueObject->SetArraySize(maxArray);
			}
			else
			{
				SvStl::MessageMgrNoDisplay e( SvStl::DataOnly );
				e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_columnValueMapInvalid, SvStl::SourceFileParams(StdMessageParams) );
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
			GetInspection()->SetDefaultInputs();
		}
	}

	//add remaining elements from vector to map
	for (std::vector<TableColumnEquation*>::iterator iter = columnList.begin(); iter != columnList.end(); ++iter )
	{
		DoubleSortValueObject* pObject = nullptr;
		// Construct new object...
		SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);

		if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), 0 ) == SVMR_SUCCESS )
		{
			RegisterEmbeddedObject( pObject, getNextFreeEmbeddedColumGUID(), (*iter)->GetName(), true, SVResetItemTool );
			pObject->SetArraySize(maxArray);
			m_ValueList.push_back(pObject);
			m_equationList.push_back(*iter);
		}
		else
		{
			delete pObject;
			SvStl::MessageMgrNoDisplay e( SvStl::DataOnly );
			e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams) );
			e.Throw();
		}
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL TableFillObject::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL returnValue = S_FALSE;

	returnValue = TableObject::onRun( RRunStatus );
	if (returnValue)
	{
		int nextPos = 0;
		if (!m_sortContainer.empty())
		{
			nextPos = (m_sortContainer[0]+1)%(m_maxArray);
		}
		m_sortContainer.push_front(nextPos);

		for (int i = 0; i < m_equationList.size(); ++i )
		{
			DoubleSortValueObject* pValueObject = (m_ValueList[i]).get();
			TableColumnEquation* pEquation = m_equationList[i];
			if (nullptr != pValueObject && nullptr != pEquation)
			{
				pValueObject->setSortContainer(m_sortContainer);
				pValueObject->SetValue(RRunStatus.m_lResultDataIndex, 0, pEquation->GetYACCResult());
			}
		}

		m_NumberOfRows.SetValue(RRunStatus.m_lResultDataIndex, static_cast<long>(m_sortContainer.size()));
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableFillObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = TableFillObjectType;
}

SVGUID TableFillObject::getNextFreeEmbeddedColumGUID()
{
	for (int i=0; i < c_maxTableColumn; ++i)
	{
		std::vector<DoubleSortValuePtr>::const_iterator it = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const DoubleSortValuePtr& entry)->bool 
		{ 
			return (nullptr != entry.get() && entry->GetEmbeddedID() == TableColumnValueObjectGuid[i]); 
		} 
		);
		if (m_ValueList.end() == it)
		{	//GUID not used yes
			return TableColumnValueObjectGuid[i];
		}
	}
	return SV_GUID_NULL;
}

#pragma endregion Private Methods
