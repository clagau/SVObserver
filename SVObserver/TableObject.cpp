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
#include "SVObjectLibrary\SVObjectManagerClass.h"
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
{
	Initialize();
}

TableObject::TableObject( SVObjectClass* POwner, int StringResourceID )
	: SVTaskObjectClass(POwner, StringResourceID)
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

	m_NumberOfRows.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return l_bOk;
}

bool TableObject::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_sortContainer.clear();

	for (std::vector<DoubleSortValuePtr>::iterator iter = m_ValueList.begin(); iter != m_ValueList.end(); ++iter )
	{
		(*iter)->setSortContainer(m_sortContainer);
	}
	m_NumberOfRows.SetValue(0, 0);

	return Result;
}

SVObjectClass* TableObject::getNumberOfRowObject() const
{
	SVLongValueObjectClass* pObject = const_cast<SVLongValueObjectClass*>(&m_NumberOfRows);
	return pObject; 
}

SVObjectClass* TableObject::OverwriteEmbeddedObject(const GUID& rUniqueID, const GUID& rEmbeddedID)
{
	//check if it is an embeddedID from an column-Value object. This will not generated automatically. Create it before it will be overwrite
	bool isColumnValue = false;
	for (int i=0; i < c_maxTableColumn; ++i)
	{
		if (TableColumnValueObjectGuid[i] == rEmbeddedID)
		{
			isColumnValue = true;
		}
	}
	if (isColumnValue)
	{
		DoubleSortValueObject* pObject = nullptr;
		// Construct new object...
		SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);
		RegisterEmbeddedObject( pObject, rEmbeddedID, _T(""), true, SVResetItemTool );
		m_ValueList.push_back(pObject);
	}

	return __super::OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void TableObject::createColumnObject(SVGUID embeddedID, LPCTSTR name, int arraySize)
{
	DoubleSortValueObject* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);

	if( CreateChildObject(pObject) )
	{
		RegisterEmbeddedObject( pObject, embeddedID, name, true, SVResetItemTool );
		pObject->SetArraySize(arraySize);
		m_ValueList.push_back(pObject);
	}
	else
	{
		delete pObject;
		ASSERT(FALSE);
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		e.Throw();
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableObjectType;

	BuildEmbeddedObjectList();
}

void TableObject::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_NumberOfRows, TableObject_NumberOfRowsGuid, IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS, true, SVResetItemTool );
	m_NumberOfRows.SetDefaultValue( 0, TRUE );
}
#pragma endregion Private Methods
