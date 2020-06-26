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

SV_IMPLEMENT_CLASS(TableObject, SvPb::TableClassId);


#pragma region Constructor
TableObject::TableObject(LPCTSTR ObjectName)
	: SVTaskObjectClass(ObjectName)
{
	Initialize();
}

TableObject::TableObject(SVObjectClass* POwner, int StringResourceID)
	: SVTaskObjectClass(POwner, StringResourceID)
{
	Initialize();
}

TableObject::~TableObject()
{
	while (0 < m_ValueList.size())
	{
		SvVol::DoubleSortValuePtr pObject = m_ValueList.back();
		m_ValueList.pop_back();
		RemoveEmbeddedObject(pObject.get());
	}
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableObject::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	m_NumberOfRows.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_NumberOfRows.setSaveValueFlag(false);

	return l_bOk;
}

void TableObject::Hide(bool hide)
{
	UINT attributeTaskObject = SvPb::taskObject;
	UINT attribute = SvDef::defaultValueObjectAttributes & ~SvPb::printable;
	if (hide)
	{
		attributeTaskObject = SvPb::noAttributes;
		attribute = SvPb::noAttributes;
	}
	SetObjectAttributesAllowed(attributeTaskObject, SvOi::OverwriteAttribute);
	m_NumberOfRows.SetObjectAttributesAllowed(attribute, SvOi::OverwriteAttribute);
	m_statusTag.SetObjectAttributesAllowed(attribute, SvOi::OverwriteAttribute);
	m_statusColor.SetObjectAttributesAllowed(attribute, SvOi::OverwriteAttribute);
}

bool TableObject::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	//Avoid time consuming not necessary function calls 
	if (ObjectAttributesAllowed() & SvPb::taskObject)
	{
		clearTable();

	
	}

	return Result;
}

SVObjectClass* TableObject::getNumberOfRowObject() const
{
	SvVol::SVLongValueObjectClass* pObject = const_cast<SvVol::SVLongValueObjectClass*>(&m_NumberOfRows);
	return pObject;
}

void TableObject::setSortContainer(const SvVol::ValueObjectSortContainer& sortMap, SVRunStatusClass&)
{
	*m_spSortContainer.get() = sortMap;
	for (int i = 0; i < m_ValueList.size(); ++i)
	{
		m_ValueList[i]->setSortContainerPtr(m_spSortContainer);
	}
	m_NumberOfRows.SetValue(static_cast<long>(sortMap.size()));
}

void  TableObject::setSortContainerDummy(const SvVol::DummySortContainer& rDummy)
{
	m_DummySortContainer = rDummy;
	for (int i = 0; i < m_ValueList.size(); ++i)
	{
		m_ValueList[i]->setSortContainerDummy(rDummy);
	}
	m_NumberOfRows.SetValue(static_cast<long>(rDummy.SimpleSize));
}
void TableObject::UpdateNumberOfRows()
{
	if (m_DummySortContainer.bIsActive)
	{
		m_NumberOfRows.SetValue(static_cast<long>(m_DummySortContainer.SimpleSize));
	}
	else
	{
		m_NumberOfRows.SetValue(static_cast<long>(m_spSortContainer->size()));
	}
}

SvVol::DoubleSortValuePtr TableObject::updateOrCreateColumn(SvPb::EmbeddedIdEnum embeddedId, std::string& newName, int arraysize)
{
	std::vector<SvVol::DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == embeddedId);
	}
	);
	if (m_ValueList.end() != valueIter)
	{
		SvVol::DoubleSortValuePtr pValueObject = *valueIter;
		if (nullptr == pValueObject)
		{
			try
			{
				pValueObject = createColumnObject(embeddedId, newName.c_str(), arraysize);
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
			return createColumnObject(embeddedId, newName.c_str(), arraysize);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(rSvE.getMessage());
		}
	}
	return nullptr;
}


SvVol::DoubleSortValuePtr TableObject::updateOrCreateColumn(SvPb::EmbeddedIdEnum embeddedId, int nameId, int arraysize)
{
	std::string newName = SvUl::LoadStdString(nameId);
	return updateOrCreateColumn(embeddedId, newName, arraysize);
}

void TableObject::removeColumn(SvPb::EmbeddedIdEnum embeddedId)
{
	std::vector<SvVol::DoubleSortValuePtr>::const_iterator valueIter = std::find_if(m_ValueList.begin(), m_ValueList.end(), [&](const SvVol::DoubleSortValuePtr& entry)->bool
	{
		return (nullptr != entry.get() && entry->GetEmbeddedID() == embeddedId);
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
	
	if (m_DummySortContainer.bIsActive)
	{
		setSortContainerDummy(SvVol::DummySortContainer(0));
	}
	m_spSortContainer->clear();
	
	m_NumberOfRows.SetValue(0L);
}

SVObjectClass* TableObject::OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID)
{
	//check if it is an embeddedID from an column-Value object. This will not generated automatically. Create it before it will be overwrite
	bool isColumnValue = false;
	for (int i = 0; i < c_maxTableColumn; ++i)
	{
		if (SvPb::TableColumnValueEId+i == embeddedID)
		{
			isColumnValue = true;
		}
	}
	if (isColumnValue)
	{
		// Construct new object...
		SvVol::DoubleSortValueObject* pObject = dynamic_cast<SvVol::DoubleSortValueObject*>(SvOi::ConstructObject(SvPb::DoubleSortValueClassId));
		RegisterEmbeddedObject(pObject, embeddedID, IDS_OBJECTNAME_TABLEOBJECT_COLUMN, true, SvOi::SVResetItemTool);
		m_ValueList.push_back(SvVol::DoubleSortValuePtr {pObject});
	}

	return __super::OverwriteEmbeddedObject(uniqueID, embeddedID);
}
#pragma endregion Public Methods

#pragma region Protected Methods
SvVol::DoubleSortValuePtr TableObject::createColumnObject(SvPb::EmbeddedIdEnum embeddedId, LPCTSTR name, int arraySize)
{
	SvVol::DoubleSortValueObject* pObject = nullptr;
	SvVol::DoubleSortValuePtr pRetObject = nullptr;
	// Construct new object...
	SVObjectManagerClass::Instance().ConstructObject(SvPb::DoubleSortValueClassId, pObject);

	if (CreateChildObject(pObject))
	{
		RegisterEmbeddedObject(pObject, embeddedId, IDS_OBJECTNAME_TABLEOBJECT_COLUMN, true, SvOi::SVResetItemTool);
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
		SvStl::MessageMgrStd e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_createColumnValueObjectFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_columnValueMapInvalid, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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

SvPb::EmbeddedIdEnum TableObject::getNextFreeEmbeddedColumID()
{
	for (int i = c_maxTableColumn - 1; i >= 0; --i)
	{
		const auto& it = std::find_if(m_embeddedList.begin(), m_embeddedList.end(), [&](const auto* pEntry)->bool
		{
			return (nullptr != pEntry && pEntry->GetEmbeddedID() == SvPb::TableColumnValueEId+i);
		}
		);
		if (m_embeddedList.end() == it)
		{	//ID not used yet
			return SvPb::TableColumnValueEId+i;
		}
	}
	return SvPb::NoEmbeddedId;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableObject::Initialize()
{
	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::TableObjectType;

	BuildEmbeddedObjectList();
	m_spSortContainer = std::make_shared<SvVol::ValueObjectSortContainer>();

}

void TableObject::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_NumberOfRows, SvPb::TableObject_NumberOfRowsEId, IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS, true, SvOi::SVResetItemTool);
	m_NumberOfRows.SetDefaultValue(0L, true);
}
#pragma endregion Private Methods


void  TableObject::getTableValues(_variant_t& rValue, long* pRowCount, long* pColoumnCount) const
{
	CComSafeArrayBound bound[2];
	auto valueList = getValueList();
	long coloumnCount = (long)valueList.size();
	long rowCount = (long)valueList[0]->getSortContainerSize();
	bound[0] = coloumnCount;
	bound[1] = rowCount;

	CComSafeArray<double> tdimsa(bound, 2);

	for (int c = 0; c < coloumnCount; c++)
	{
		for (int r = 0; r < rowCount; r++)
		{
			LONG aIndex[2] = {c,r};
			double val {0.0};
			valueList[c]->GetValue(val, r);
			HRESULT hr = tdimsa.MultiDimSetAt(aIndex, val);
			assert(hr == S_OK);
			UNREFERENCED_PARAMETER(hr);
		}
	}
	rValue.Clear();
	rValue.vt = VT_ARRAY | VT_R8;
	rValue.parray = tdimsa.Detach();
	if (pRowCount)
	{
		*pRowCount = rowCount;
	}
	if (pColoumnCount)
	{
		*pColoumnCount = coloumnCount;
	}
}
bool TableObject::setTableValues(const _variant_t& rValue)
{
	
	unsigned long colNumber = static_cast<unsigned long> (m_ValueList.size());
	if (colNumber == 0)
	{
		return false;
	}
	
	if (rValue.vt == VT_EMPTY)
	{
		setSortContainerDummy(SvVol::DummySortContainer(0));
		return true;
	}

	if ((rValue.vt != (VT_R8 | VT_ARRAY)) || (rValue.parray == nullptr))
	{
		return false;
	}
	
	CComSafeArray<double> sa(rValue.parray);
	int dim = sa.GetDimensions();
	if (dim != 2)
	{
		return false;
	}
	
	int nRows = __min(m_ValueList[0]->getArraySize(), static_cast<int>(sa.GetCount(1)));
	
	setSortContainerDummy(SvVol::DummySortContainer(nRows));

	assert(colNumber == sa.GetCount(0));
	for (unsigned long col = 0; col < colNumber; col++)
	{
		for (long row = 0; row < nRows; row++)
		{
			double val {0};
			try
			{
				long Index[2] = {static_cast<long> (col), row};
				sa.MultiDimGetAt(Index, val);
			}
			catch (...)
			{
				val = 0.0;
			}
			m_ValueList[col]->SetValue(val, row);
		}
	}

	return true;
}

unsigned  TableObject::getColumNames(_variant_t& rValue) const
{
	auto valueList = getValueList();
	CComSafeArray<BSTR> saStr(static_cast<unsigned>(valueList.size()));
	for (unsigned i = 0; i < valueList.size(); i++)
	{
		_bstr_t temp = valueList[i]->GetName();
		saStr.SetAt(i, temp);
	}
	rValue.Clear();
	rValue.vt = VT_ARRAY | VT_BSTR;
	rValue.parray = saStr.Detach();
	return static_cast<unsigned>(valueList.size());
}

SvVol::ValueObjectSortContainer&  TableObject::getSortContainer()  
{ 
	//make a real sortcontainer if we have only a dummy
	if (m_DummySortContainer.bIsActive)
	{
		m_DummySortContainer.bIsActive = false;
		m_spSortContainer->resize(m_DummySortContainer.SimpleSize);
		for (int i = 0; i < m_spSortContainer->size(); i++)
		{
			m_spSortContainer->at(i) = i;
		}
	}
	//make sure all have the same sortcontainerPtr
	for ( auto& rpSortValue : m_ValueList)
	{
		rpSortValue->setSortContainerPtr(m_spSortContainer);
	}
	return *m_spSortContainer.get(); 
};




} //namespace SvOp
