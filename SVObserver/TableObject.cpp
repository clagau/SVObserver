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
#include "SVOCore/SVTool.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
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
bool TableObject::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool l_bOk = SVTaskObjectClass::CreateObject( pCreateStructure );

	m_NumberOfRows.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_NumberOfRows.setSaveValueFlag(false);

	return l_bOk;
}

bool TableObject::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_sortContainer.clear();

	for (std::vector<DoubleSortValuePtr>::iterator iter = m_ValueList.begin(); iter != m_ValueList.end(); ++iter )
	{
		(*iter)->setSortContainer(m_sortContainer);
		(*iter)->setSaveValueFlag(false);
	}
	m_NumberOfRows.SetValue(0L);
	dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();

	return Result;
}

SVObjectClass* TableObject::getNumberOfRowObject() const
{
	SVLongValueObjectClass* pObject = const_cast<SVLongValueObjectClass*>(&m_NumberOfRows);
	return pObject; 
}

void TableObject::setSortContainer(const ValueObjectSortContainer& sortMap, SVRunStatusClass& rRunStatus)
{
	m_sortContainer = sortMap;
	for (int i = 0; i < m_ValueList.size(); ++i)
	{
		m_ValueList[i]->setSortContainer(m_sortContainer);
	}
	m_NumberOfRows.SetValue(static_cast<long>(sortMap.size()));
}

DoubleSortValueObject* TableObject::updateOrCreateColumn(const GUID& rEmbeddedId, int nameId, int arraysize)
{
	SVString newName = SvUl_SF::LoadSVString(nameId);
	std::vector<DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == rEmbeddedId);
	}
	);
	if (m_ValueList.end() != valueIter)
	{
		DoubleSortValueObject* pValueObject = valueIter->get();
		if (nullptr == pValueObject)
		{
			try
			{
				pValueObject = createColumnObject(rEmbeddedId, newName.c_str(), arraysize);
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				SvStl::MessageMgrStd e(SvStl::LogOnly);
				e.setMessage(rSvE.getMessage());
			}
		}
		if (nullptr != pValueObject)
		{
			SVString OldName = pValueObject->GetName();
			if (OldName != newName)
			{
				pValueObject->SetName(newName.c_str());
				GetInspection()->OnObjectRenamed(*pValueObject, OldName);
			}
			pValueObject->SetArraySize(arraysize);
		}
		return pValueObject;
	}
	else
	{
		try
		{
			return createColumnObject(rEmbeddedId, newName.c_str(), arraysize);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageMgrStd e(SvStl::LogOnly);
			e.setMessage(rSvE.getMessage());
		}
	}
	return nullptr;
}

void TableObject::removeColumn(const GUID& rEmbeddedId)
{
	std::vector<DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == rEmbeddedId);
	}
	);
	if (m_ValueList.end() != valueIter)
	{
		DoubleSortValuePtr pValueObject = *valueIter;
		m_ValueList.erase(valueIter);
		if (nullptr != pValueObject.get())
		{
			hideEmbeddedObject(*pValueObject.get());
			RemoveEmbeddedObject(pValueObject.get());
		}

		//Object must be deleted, before SetDefaultInputs is called.
		pValueObject.reset();
		dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
	}
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
		RegisterEmbeddedObject( pObject, rEmbeddedID, _T(""), true, SvOi::SVResetItemTool );
		m_ValueList.push_back(pObject);
	}

	return __super::OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
}
#pragma endregion Public Methods

#pragma region Protected Methods
DoubleSortValueObject* TableObject::createColumnObject(SVGUID embeddedID, LPCTSTR name, int arraySize)
{
	DoubleSortValueObject* pObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);

	if( CreateChildObject(pObject) )
	{
		RegisterEmbeddedObject( pObject, embeddedID, name, true, SvOi::SVResetItemTool );
		pObject->SetArraySize(arraySize);
		m_ValueList.push_back(pObject);
	}
	else
	{
		delete pObject;
		pObject = nullptr;
		ASSERT(FALSE);
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		e.Throw();
	}
	return pObject;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = TableObjectType;

	BuildEmbeddedObjectList();
}

void TableObject::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_NumberOfRows, TableObject_NumberOfRowsGuid, IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS, true, SvOi::SVResetItemTool );
	m_NumberOfRows.SetDefaultValue( 0L, true );
}
#pragma endregion Private Methods
