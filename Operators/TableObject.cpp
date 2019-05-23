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
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableObject, TableObjectGuid);


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
		SvVol::DoubleSortValuePtr pObject = m_ValueList.back();
		m_ValueList.pop_back();
		RemoveEmbeddedObject(pObject.get());
	}
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableObject::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	m_NumberOfRows.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_NumberOfRows.setSaveValueFlag(false);

	return l_bOk;
}

bool TableObject::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	clearTable();

	GetInspectionInterface()->SetDefaultInputs();

	return Result;
}

SVObjectClass* TableObject::getNumberOfRowObject() const
{
	SvVol::SVLongValueObjectClass* pObject = const_cast<SvVol::SVLongValueObjectClass*>(&m_NumberOfRows);
	return pObject; 
}

void TableObject::setSortContainer(const SvVol::ValueObjectSortContainer& sortMap, SVRunStatusClass& rRunStatus)
{
	m_sortContainer = sortMap;
	for (int i = 0; i < m_ValueList.size(); ++i)
	{
		m_ValueList[i]->setSortContainer(m_sortContainer);
	}
	m_NumberOfRows.SetValue(static_cast<long>(sortMap.size()));
}

SvVol::DoubleSortValuePtr TableObject::updateOrCreateColumn(const GUID& rEmbeddedId, int nameId, int arraysize)
{
	std::string newName = SvUl::LoadStdString(nameId);
	std::vector<SvVol::DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == rEmbeddedId);
	}
	);
	if (m_ValueList.end() != valueIter)
	{
		SvVol::DoubleSortValuePtr pValueObject = *valueIter;
		if (nullptr == pValueObject)
		{
			try
			{
				pValueObject = createColumnObject(rEmbeddedId, newName.c_str(), arraysize);
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(rSvE.getMessage());
			}
		}
		if (nullptr != pValueObject)
		{
			std::string OldName = pValueObject->GetName();
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
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(rSvE.getMessage());
		}
	}
	return nullptr;
}

void TableObject::removeColumn(const GUID& rEmbeddedId)
{
	std::vector<SvVol::DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == rEmbeddedId);
	}
	);
	if (m_ValueList.end() != valueIter)
	{
		SvVol::DoubleSortValuePtr pValueObject = *valueIter;
		m_ValueList.erase(valueIter);
		if (nullptr != pValueObject.get())
		{
			RemoveEmbeddedObject(pValueObject.get());
		}

		//Object must be deleted, before SetDefaultInputs is called.
		pValueObject.reset();
		GetInspectionInterface()->SetDefaultInputs();
	}
}

void TableObject::clearTable()
{
	m_sortContainer.clear();

	for (std::vector<SvVol::DoubleSortValuePtr>::iterator iter = m_ValueList.begin(); iter != m_ValueList.end(); ++iter)
	{
		(*iter)->setSortContainer(m_sortContainer);
		(*iter)->setSaveValueFlag(false);
	}
	m_NumberOfRows.SetValue(0L);
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
		// Construct new object...
		SvVol::DoubleSortValueObject* pObject = dynamic_cast<SvVol::DoubleSortValueObject*>(SvOi::ConstructObject(DoubleSortValueObjectGuid));
		RegisterEmbeddedObject( pObject, rEmbeddedID, IDS_OBJECTNAME_TABLEOBJECT_COLUMN, true, SvOi::SVResetItemTool );
		m_ValueList.push_back(SvVol::DoubleSortValuePtr{ pObject });
	}

	return __super::OverwriteEmbeddedObject(rUniqueID, rEmbeddedID);
}
#pragma endregion Public Methods

#pragma region Protected Methods
SvVol::DoubleSortValuePtr TableObject::createColumnObject(SVGUID embeddedID, LPCTSTR name, int arraySize)
{
	SvVol::DoubleSortValueObject* pObject = nullptr;
	SvVol::DoubleSortValuePtr pRetObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(DoubleSortValueObjectGuid, pObject);

	if( CreateChildObject(pObject) )
	{
		RegisterEmbeddedObject( pObject, embeddedID, IDS_OBJECTNAME_TABLEOBJECT_COLUMN, true, SvOi::SVResetItemTool );
		pObject->SetName(name);
		pObject->SetArraySize(arraySize);
		pRetObject = SvVol::DoubleSortValuePtr {pObject};
		m_ValueList.push_back(pRetObject);
	}
	else
	{
		delete pObject;
		pObject = nullptr;
		assert(false);
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		e.Throw();
	}
	return pRetObject;
}

void TableObject::UpdateColumnValueObject(int pos, std::string objectName, int maxArray)
{
	assert(0 <= pos && m_ValueList.size() > pos);
	//value already exist, update the name and size if needed.
	SvVol::DoubleSortValueObject* pValueObject = m_ValueList[pos].get();
	if (nullptr != pValueObject)
	{
		if (pValueObject->GetName() != objectName)
		{
			std::string OldName = pValueObject->GetName();
			pValueObject->SetName(objectName.c_str());
			GetInspection()->OnObjectRenamed(*pValueObject, OldName);
		}
		pValueObject->SetArraySize(maxArray);
	}
	else
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValueMapInvalid, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		e.Throw();
	}
}

void TableObject::MoveValueColumn(int oldPos, int newPos)
{
	if (oldPos != newPos && 0 <= oldPos  && m_ValueList.size() > oldPos && 0 <= newPos && m_ValueList.size() > newPos)
	{
		auto value = *(m_ValueList.begin() + oldPos);
		auto posObject = *(m_ValueList.begin() + newPos);
		m_ValueList.erase(m_ValueList.begin() + oldPos);
		m_ValueList.insert(m_ValueList.begin() + newPos, value);
		MovedEmbeddedObject(value.get(), posObject.get());
	}
}

SVGUID TableObject::getNextFreeEmbeddedColumGUID()
{
	for (int i = c_maxTableColumn - 1; i >= 0; --i)
	{
		const auto& it = std::find_if(m_embeddedList.begin(), m_embeddedList.end(), [&](const auto* pEntry)->bool
		{
			return (nullptr != pEntry && pEntry->GetEmbeddedID() == TableColumnValueObjectGuid[i]);
		}
		);
		if (m_embeddedList.end() == it)
		{	//GUID not used yet
			return TableColumnValueObjectGuid[i];
		}
	}
	return GUID_NULL;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableObjectType;

	BuildEmbeddedObjectList();
}

void TableObject::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_NumberOfRows, TableObject_NumberOfRowsGuid, IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS, true, SvOi::SVResetItemTool );
	m_NumberOfRows.SetDefaultValue( 0L, true );
}
#pragma endregion Private Methods

} //namespace SvOp
