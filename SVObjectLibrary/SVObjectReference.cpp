//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

SVObjectReference::SVObjectReference( SVObjectClass* pObject ):
	m_ArrayIndex(-1)
, m_pValueObject(nullptr)
{
	m_pObject = pObject;
	m_objectId = (nullptr != m_pObject) ? m_pObject->getObjectId() : SvDef::InvalidObjectId;
	(nullptr != m_pObject) ? m_NameInfo.ParseObjectName(m_pObject->GetCompleteName().c_str()) : m_NameInfo.clear();
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, long lArrayIndex, std::string strDefaultValue ):
	m_ArrayIndex(lArrayIndex) 
, m_pValueObject(nullptr)
{
	m_pObject = pObject;
	m_objectId = m_pObject ? m_pObject->getObjectId() : SvDef::InvalidObjectId;
	if( nullptr != m_pObject )
	{
		m_NameInfo.ParseObjectName( m_NameInfo, m_pObject->GetCompleteName().c_str() );
	}
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.SetIndex( SvUl::Format(_T("%d"), lArrayIndex ));
	m_NameInfo.SetIsDefaultValuePresent(true);
	m_NameInfo.SetDefaultValue( strDefaultValue );
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo )
	: m_pValueObject (nullptr)
{
	m_pObject = pObject;
	m_objectId = m_pObject ? m_pObject->getObjectId() : SvDef::InvalidObjectId;
	m_NameInfo = p_rNameInfo;
	m_ArrayIndex = p_rNameInfo.GetIndexValue();
}

SVObjectReference::SVObjectReference(const std::string& objectIdAndIndexString)
	: m_pValueObject(nullptr)
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
	}
	else
	{
		m_NameInfo.clear();
		m_ArrayIndex = -1;
	}
}

const SVObjectReference& SVObjectReference::operator = ( const SVObjectReference& rhs )
{
	m_pObject = rhs.m_pObject;
	m_pValueObject = nullptr;
	m_objectId = rhs.m_objectId != SvDef::InvalidObjectId ? rhs.m_objectId : (nullptr != m_pObject ? m_pObject->getObjectId() : SvDef::InvalidObjectId);
	m_NameInfo = rhs.m_NameInfo;
	m_ArrayIndex = rhs.m_ArrayIndex;
	return *this;
}

void  SVObjectReference::clear()
{
	init();
}


SvOi::IValueObject* SVObjectReference::getValueObject(bool forceCast) const
{
	if(nullptr == m_pValueObject || forceCast)
	{
		m_pValueObject = dynamic_cast<SvOi::IValueObject*> (m_pObject);
	}
	return m_pValueObject;
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

const std::string& SVObjectReference::GetIndex() const
{
	
	return m_NameInfo.GetIndex();
}

UINT SVObjectReference::ObjectAttributesAllowed() const
{
	assert( nullptr != m_pObject );
	if (nullptr != m_pObject)
	{
		return m_pObject->ObjectAttributesAllowed();
	}
	else
	{
		return 0;
	}
}

UINT SVObjectReference::ObjectAttributesSet() const
{
	assert( nullptr != m_pObject );
	if (nullptr != m_pObject)
	{
		return m_pObject->ObjectAttributesSet(m_ArrayIndex >= 0 ? m_ArrayIndex : 0);
	}
	else
	{
		return 0;
	}
}

UINT SVObjectReference::SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type )
{
	assert( nullptr != m_pObject );
	if (nullptr != m_pObject)
	{
		return m_pObject->SetObjectAttributesAllowed(Attributes, Type);
	}
	else
	{
		return 0;
	}
}

UINT SVObjectReference::SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type )
{
	assert( nullptr != m_pObject );
	if (nullptr != m_pObject)
	{
		return m_pObject->SetObjectAttributesSet(Attributes, Type, m_ArrayIndex >= 0 ? m_ArrayIndex : 0);
	}
	else
	{
		return 0;
	}
}

long SVObjectReference::IncrementIndex()
{
	if( isArray() && m_ArrayIndex > -1) 
	{

		m_ArrayIndex++;
	}

	return m_ArrayIndex;
}

