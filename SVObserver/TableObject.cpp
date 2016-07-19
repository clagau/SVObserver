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

SVObjectClass* TableObject::getNumberOfRowObject() const
{
	SVLongValueObjectClass* pObject = const_cast<SVLongValueObjectClass*>(&m_NumberOfRows);
	return pObject; 
}
#pragma endregion Public Methods

#pragma region Protected Methods
DWORD_PTR TableObject::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	// Try to process message by yourself...
	// ( if necessary process here the incoming messages )
	switch (dwPureMessageID)
	{
	case SVMSGID_OVERWRITE_OBJECT:
		{
			//check if it is an embeddedID from an column-Value object. This will not generated automatically. Create it before it will be overwrite
			const GUID embeddedID = * ((GUID*) DwMessageContext);
			bool isColumnValue = false;
			for (int i=0; i < c_maxTableColumn; ++i)
			{
				if (TableColumnValueObjectGuid[i] == embeddedID)
				{
					isColumnValue = true;
				}
			}
			if (isColumnValue)
			{
				DoubleSortValueObject* pObject = nullptr;
				// Construct new object...
				SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);
				RegisterEmbeddedObject( pObject, embeddedID, _T(""), true, SVResetItemTool );
				m_ValueList.push_back(pObject);
			}
			//Don't change DwResult, because SVTaskObjectClass will and have to process the overwrite command.
			break;
		}
	}
	if (DwResult == SVMR_NOT_PROCESSED)
	{
		DwResult = SVTaskObjectClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
	}
	return DwResult;
}

void TableObject::createColumnObject(SVGUID embeddedID, LPCTSTR name, int arraySize)
{
	DoubleSortValueObject* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);

	if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), 0 ) == SVMR_SUCCESS )
	{
		RegisterEmbeddedObject( pObject, embeddedID, name, true, SVResetItemTool );
		pObject->SetArraySize(arraySize);
		m_ValueList.push_back(pObject);
	}
	else
	{
		delete pObject;
		ASSERT(FALSE);
		SvStl::MessageMgrNoDisplay e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams) );
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
