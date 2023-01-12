//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectReference
//* .File Name       : $Workfile:   SVObjectReference.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 Jan 2015 16:04:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectClass.h"
#include "SVObjectReference.h"
#include "SVObjectManagerClass.h"
#include "ObjectInterfaces/ILinkedObject.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVProtoBuf/GetObjectSelector.pb.h"
#pragma endregion Includes

bool SVObjectReference::operator < (const SVObjectReference& rhs) const
{
	return (m_pObject < rhs.m_pObject) || ((m_pObject == rhs.m_pObject) && (m_ArrayIndex < rhs.m_ArrayIndex));
}

SVObjectReference::SVObjectReference(const SVObjectReference& rhs)
{
	*this = rhs;
}

SVObjectReference::SVObjectReference(SVObjectClass* pObject, long lArrayIndex, std::string strDefaultValue) :
	m_ArrayIndex(lArrayIndex)
	, m_pObject(pObject)
{
	m_objectId = m_pObject ? m_pObject->getObjectId() : SvDef::InvalidObjectId;
	if (nullptr != m_pObject)
	{
		m_NameInfo.ParseObjectName(m_NameInfo, m_pObject->GetCompleteName().c_str());
		registerNotification();
	}
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.SetIndex(SvUl::Format(_T("%d"), lArrayIndex));
	m_NameInfo.SetIsDefaultValuePresent(true);
	m_NameInfo.SetDefaultValue(strDefaultValue);
}

SVObjectReference::SVObjectReference(SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo)
	: m_pObject(pObject)
{
	if (nullptr != m_pObject)
	{
		m_objectId = m_pObject->getObjectId();
		registerNotification();
	}
	m_NameInfo = p_rNameInfo;
	m_ArrayIndex = p_rNameInfo.GetIndexValue();
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject ):
	m_ArrayIndex(-1)
	,m_pObject(pObject)
{
	if (nullptr != m_pObject)
	{
		m_objectId = m_pObject->getObjectId();
		m_NameInfo.ParseObjectName(m_pObject->GetCompleteName().c_str());
		registerNotification();
	}
}

SVObjectReference::SVObjectReference(uint32_t objectId)
	: m_objectId(objectId)
{
	m_pObject = SVObjectManagerClass::Instance().GetObject(m_objectId);
	registerNotification();
}

SVObjectReference::SVObjectReference(const std::string& objectIdAndIndexString)
{
	std::string::size_type Pos = objectIdAndIndexString.find_first_of(_T("["));
	std::string objectIdString = objectIdAndIndexString.substr(0, Pos);
	m_objectId = calcObjectId(objectIdString);

	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_objectId, m_pObject);
	if (nullptr != m_pObject)
	{
		std::string tmpName = m_pObject->GetCompleteName();
		if (std::string::npos != Pos)
		{
			tmpName += objectIdAndIndexString.substr(Pos, std::string::npos);
		}
		m_NameInfo.ParseObjectName(m_NameInfo, tmpName);
		m_ArrayIndex = m_NameInfo.GetIndexValue();
		registerNotification();
	}
	else
	{
		m_NameInfo.clear();
		if (std::string::npos != Pos)
		{
			auto text = objectIdAndIndexString.substr(Pos, std::string::npos);
			int count = sscanf(text.c_str(), "[%ld]", &m_ArrayIndex);
			if (1 == count)
			{
				--m_ArrayIndex;
				m_ArrayIndex = std::max(-1l, m_ArrayIndex);
			}
			else
			{
				Log_Assert(false);
				m_ArrayIndex = -1l;
			}
		}
		else
		{
			m_ArrayIndex = -1l;
		}
	}
}

SVObjectReference::~SVObjectReference()
{
	m_notifyPtr.reset();
}

const SVObjectReference& SVObjectReference::operator = ( const SVObjectReference& rhs )
{
	m_notifyPtr.reset();
	m_pObject = rhs.m_pObject;
	registerNotification();
	m_pValueObject = nullptr;
	m_pFinalObject = nullptr;
	m_pLinkedObject = nullptr;
	m_objectId = rhs.m_objectId != SvDef::InvalidObjectId ? rhs.m_objectId : (nullptr != m_pObject ? m_pObject->getObjectId() : SvDef::InvalidObjectId);
	m_NameInfo = rhs.m_NameInfo;
	m_ArrayIndex = rhs.m_ArrayIndex;
	return *this;
}

bool SVObjectReference::operator == (const SVObjectReference& rhs) const
{
	bool res = (m_pObject == rhs.m_pObject) && (m_ArrayIndex == rhs.m_ArrayIndex);
	return res;
}

void  SVObjectReference::clear()
{
	m_notifyPtr.reset();
	m_pObject = nullptr;
	m_pValueObject = nullptr;
	m_pFinalObject = nullptr;
	m_pLinkedObject = nullptr;
	m_objectId = SvDef::InvalidObjectId;
	m_NameInfo.clear();
	m_ArrayIndex = -1;
}

void SVObjectReference::update()
{
	auto* pObject = SVObjectManagerClass::Instance().GetObject(m_objectId);
	if (m_pObject != pObject)
	{
		m_notifyPtr.reset();
		m_pObject = pObject;
		registerNotification();
		m_pValueObject = nullptr;
		m_pFinalObject = nullptr;
	}
}

SVObjectClass* SVObjectReference::getFinalObject() const
{
	if (nullptr == m_pFinalObject)
	{
		m_pLinkedObject = dynamic_cast<SvOi::ILinkedObject*> (m_pObject);
		if (nullptr == m_pLinkedObject)
		{
			m_pFinalObject = m_pObject;
		}
	}

	if (nullptr != m_pLinkedObject)
	{
		try
		{
			m_pFinalObject = static_cast<SVObjectClass*>(const_cast<SvOi::IObjectClass*>(m_pLinkedObject->getLinkedObject()));
		}
		catch (...)
		{
			m_pFinalObject = nullptr;
		}
		if (nullptr == m_pFinalObject)
		{
			m_pFinalObject = m_pObject;
		}
	}
	return m_pFinalObject;
}

SvOi::IValueObject* SVObjectReference::getValueObject(bool forceCast) const
{
	if (nullptr == m_pValueObject || forceCast)
	{
		m_pValueObject = dynamic_cast<SvOi::IValueObject*> (m_pObject);
	}
	return m_pValueObject;
}

long SVObjectReference::ArrayIndex() const
{
	long Index = -1;

	if (isArray() && m_ArrayIndex > -1)
	{
		Index = m_ArrayIndex;
	}

	return Index;
}

long SVObjectReference::getValidArrayIndex() const
{
	return (-1 != ArrayIndex()) ? ArrayIndex() : 0;
}

std::string SVObjectReference::DefaultValue() const
{
	return m_NameInfo.GetDefaultValue();
}

bool SVObjectReference::isArray() const
{
	const SvOi::IValueObject* pValueObject = getValueObject(true);
	// the parameter forceCast was added to the getValueObject() call since problems were cause otherwise
	// when SVArchiveRecordsArray::RemoveDisconnectedObject() is called by the BasicValueObject destructor
	// which can happen because of the changes in SVO-2297
	return nullptr != pValueObject ? pValueObject->isArray() : false;
}

bool SVObjectReference::isEntireArray() const
{
	return isArray() && m_ArrayIndex == -1 && m_NameInfo.IsIndexPresent();

}

std::string SVObjectReference::objectIdToString() const
{
	if (nullptr != m_pObject)
	{
		return convertObjectIdToString(m_pObject->getObjectId());
	}
	return {};
}

void SVObjectReference::SetEntireArray()
{
	m_ArrayIndex = -1;  
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.ClearIndex();
}

void SVObjectReference::SetArrayIndex( long lArrayIndex )
{
	m_ArrayIndex = lArrayIndex;  
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.SetIndex( SvUl::Format(_T("%d"), lArrayIndex ) );
}

std::string SVObjectReference::GetName(bool OneBased /*=false*/) const
{
	std::string Result;
	
	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetName();
		Result += GetIndexString(OneBased);
	}
	return Result;
}

std::string SVObjectReference::GetCompleteName(bool OneBased /*=false*/) const
{
	std::string Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteName();
		Result += GetIndexString(OneBased);
	}
	return Result;
}

std::string SVObjectReference::GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude, bool OneBased /*=false*/, bool arrayGroupName /*= false*/) const
{
	std::string Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetObjectNameToObjectType(objectTypeToInclude);
		//For object selector an extra array group name is added
		if (arrayGroupName)
		{
			Result += _T(".");
			Result += m_pObject->GetName();
		}
		Result += GetIndexString(OneBased);
	}
	return Result;
}

std::string SVObjectReference::GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeBefore, bool OneBased /*=false*/) const
{
	std::string Result;

	if (nullptr != m_pObject)
	{
		Result = m_pObject->GetObjectNameBeforeObjectType(objectTypeBefore);
		Result += GetIndexString(OneBased);
	}
	return Result;
}

std::string SVObjectReference::GetObjectIdAndIndexOneBased() const
{
	std::string Result;

	if (nullptr != m_pObject)
	{
		Result = objectIdToString();
		Result += GetIndexString(true);
	}
	return Result;
}

const SVObjectNameInfo& SVObjectReference::GetObjectNameInfo() const
{
	return m_NameInfo;
}

UINT SVObjectReference::ObjectAttributesAllowed() const
{
	Log_Assert(nullptr != m_pObject);
	if (nullptr != m_pObject)
	{
		return m_pObject->ObjectAttributesAllowed();
	}
	else
	{
		return 0;
	}
}

UINT SVObjectReference::SetObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type)
{
	Log_Assert(nullptr != m_pObject);
	if (nullptr != m_pObject)
	{
		return m_pObject->SetObjectAttributesAllowed(Attributes, Type);
	}
	else
	{
		return 0;
	}
}

std::string SVObjectReference::GetIndexString(bool OneBased /*=false*/) const
{
	std::string Result;

	if( isArray() )
	{
		if ( m_ArrayIndex >= 0 )
		{
			Result = SvUl::Format(_T("[%d]"), OneBased ? m_ArrayIndex + 1 : m_ArrayIndex );
		}
		else if (isEntireArray())
		{
			Result = _T("[ ]");
		}
	}
	return Result;
}

HRESULT SVObjectReference::getValue(_variant_t& rValue) const
{
	SvOi::IValueObject* pValueObject = getValueObject();
	if (nullptr != pValueObject)
	{
		return pValueObject->getValue(rValue, ArrayIndex());
	}
	return E_POINTER;
}

long SVObjectReference::IncrementIndex()
{
	if( isArray() && m_ArrayIndex > -1) 
	{

		m_ArrayIndex++;
	}

	return m_ArrayIndex;
}

void SVObjectReference::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, bool wholeArray, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, SvPb::SVObjectTypeEnum nameToType)
{
	SvPb::TreeItem insertItem;
	auto* pValueObject = getValueObject(false);
	if (nullptr != pValueObject)
	{
		insertItem.set_type(pValueObject->getTypeName());
	}

	if (isArray())
	{
		if (wholeArray && pFunctor(getObject(), attribute, 0))
		{
			SetEntireArray();
			insertItem.set_name(GetName(true));
			insertItem.set_location(GetObjectNameToObjectType(nameToType, true, true));
			insertItem.set_objectidindex(GetObjectIdAndIndexOneBased());
			treeInserter = insertItem;
		}

		// add array elements
		if (nullptr != pValueObject)
		{
			int iArraySize = pValueObject->getArraySize();
			for (int i = 0; i < iArraySize; i++)
			{
				if (pFunctor(getObject(), attribute, i))
				{
					SetArrayIndex(i);
					insertItem.set_name(GetName(true));
					insertItem.set_location(GetObjectNameToObjectType(nameToType, true, true));
					insertItem.set_objectidindex(GetObjectIdAndIndexOneBased());
					treeInserter = insertItem;
				}
			}
		}
	}
	else if (pFunctor(getObject(), attribute, 0))
	{
		insertItem.set_name(GetName());
		insertItem.set_location(GetObjectNameToObjectType(nameToType, true));
		insertItem.set_objectidindex(GetObjectIdAndIndexOneBased());
		treeInserter = insertItem;
	}
}

void SVObjectReference::onChangeNotification(SvOi::ObjectNotificationType type, uint32_t objectId)
{
	switch (type)
	{
		case SvOi::ObjectNotificationType::Deleting:
			clear();
			break;
		case SvOi::ObjectNotificationType::ObjectIdChange:
			if (nullptr != m_pObject && objectId == m_objectId)
			{
				m_objectId = m_pObject->getObjectId();
			}
			break;
		case SvOi::ObjectNotificationType::SwitchObject:
			setObjectId(objectId);
			update();
			break;
		default:
			break;
	}
}

const std::string& SVObjectReference::GetIndex() const
{

	return m_NameInfo.GetIndex();
}

void SVObjectReference::registerNotification()
{
	if (nullptr != m_pObject)
	{
		m_notifyPtr = m_pObject->registerNotification(std::make_shared<SvOi::ObjectNotificationFunction>([this](SvOi::ObjectNotificationType type, uint32_t objectId) { return onChangeNotification(type, objectId); }));
	}
}