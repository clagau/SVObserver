//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the table use e.g. for TableTool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableObject.h"
#include "SVTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(TableObject, TableObjectGuid);

#pragma endregion Declarations

#pragma region Constructor
TableObject::TableObject( LPCTSTR ObjectName )
	: SVTaskObjectClass(ObjectName)
	, m_maxArray(0)
{
	Initialize();
}

TableObject::TableObject( SVObjectClass* POwner, int StringResourceID )
	: SVTaskObjectClass(POwner, StringResourceID)
	, m_maxArray(0)
{
	Initialize();
}

TableObject::~TableObject()
{
	while(0 < m_ValueList.size())
	{
		DoubleSortValuePtr pObject = m_ValueList.back();
		m_ValueList.pop_back();
		RemoveEmbeddedObject(pObject.get());
	}
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL TableObject::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = SVTaskObjectClass::CreateObject( pCreateStructure );

	m_NumberOfRows.ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_PRINTABLE | SV_SELECTABLE_FOR_EQUATION;

	return l_bOk;
}

HRESULT TableObject::ResetObject()
{
	HRESULT hr = SVTaskObjectClass::ResetObject();

	m_sortContainer.clear();

	for (std::vector<DoubleSortValuePtr>::iterator iter = m_ValueList.begin(); iter != m_ValueList.end(); ++iter )
	{
		(*iter)->setSortContainer(m_sortContainer);
	}
	m_NumberOfRows.SetValue(0, 0);

	return hr;
}
#pragma endregion Public Methods

#pragma region Protected Methods
#pragma endregion Protected Methods

#pragma region Private Methods
void TableObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableObjectType;
	//m_outObjectInfo.ObjectTypeInfo.SubType    = SVTableToolObjectType;

	BuildEmbeddedObjectList();
}

void TableObject::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_NumberOfRows, TableObject_NumberOfRowsGuid, IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS, true, SVResetItemTool );
	m_NumberOfRows.SetDefaultValue( 0, TRUE );
}
#pragma endregion Private Methods
