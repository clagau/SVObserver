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
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SVObjectReference::SVObjectReference( SVObjectClass* pObject ):
	m_ArrayIndex(-1)
, m_pValueObject(nullptr)
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL;
	m_NameInfo.clear();
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, long lArrayIndex, std::string strDefaultValue ):
	m_ArrayIndex(lArrayIndex) 
, m_pValueObject(nullptr)
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL;
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
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL;
	m_NameInfo = p_rNameInfo;
	m_ArrayIndex = p_rNameInfo.GetIndexValue();
}

SVObjectReference::SVObjectReference(const std::string& guidAndIndexString)
	: m_pValueObject(nullptr)
{
	std::string::size_type Pos = guidAndIndexString.find_first_of(_T("["));
	std::string guidString = guidAndIndexString.substr(0, Pos);
	m_Guid = SVGUID(_bstr_t(guidString.c_str()));
	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_Guid, m_pObject);
	if (nullptr != m_pObject)
	{
		std::string tmpName = m_pObject->GetCompleteName();
		if (std::string::npos != Pos)
		{
			tmpName += guidAndIndexString.substr(Pos, std::string::npos);
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
	m_Guid = rhs.m_Guid != SV_GUID_NULL ? rhs.m_Guid : (nullptr != m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL);
	m_NameInfo = rhs.m_NameInfo;
	m_ArrayIndex = rhs.m_ArrayIndex;
	return *this;
}

SvOi::IValueObject* SVObjectReference::getValueObject() const
{
	if(nullptr == m_pValueObject)
	{
		m_pValueObject = dynamic_cast<SvOi::IValueObject*> (m_pObject);
	}
	return m_pValueObject;
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

std::string SVObjectReference::GetName() const
{
	std::string Result;
	
	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetName();
		Result += GetZeroBasedIndexString();
	}
	return Result;
}

std::string SVObjectReference::GetCompleteName() const
{
	std::string Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteName();
		Result += GetZeroBasedIndexString();
	}
	return Result;
}

std::string SVObjectReference::GetCompleteObjectNameToObjectType( LPCTSTR lpszCompleteName, SvDef::SVObjectTypeEnum objectTypeToInclude ) const
{
	std::string Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteObjectNameToObjectType(lpszCompleteName, objectTypeToInclude);
		Result += GetZeroBasedIndexString();
	}
	return Result;
}

std::string SVObjectReference::GetOneBasedName() const
{
	std::string Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetName();
		Result += GetOneBasedIndexString();
	}
	return Result;
}

std::string SVObjectReference::GetCompleteOneBasedObjectName() const
{
	std::string Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteName();
		Result += GetOneBasedIndexString();
	}
	return Result;
}

std::string SVObjectReference::GetGuidAndIndexOneBased() const
{
	std::string Result;

	if (nullptr != m_pObject)
	{
		Result = m_pObject->GetUniqueObjectID().ToString();
		Result += GetOneBasedIndexString();
	}
	return Result;
}

const SVObjectNameInfo& SVObjectReference::GetObjectNameInfo() const
{
	return m_NameInfo;
}

std::string SVObjectReference::GetZeroBasedIndexString() const
{
	std::string Result;

	if( isArray() )
	{
		if ( m_ArrayIndex >= 0 )
		{
			Result = SvUl::Format(_T("[%d]"), m_ArrayIndex );
		}
		else if (isEntireArray())
		{
			Result = _T("[ ]");
		}
	}
	return Result;
}

std::string SVObjectReference::GetOneBasedIndexString() const
{
	std::string Result;

	if( isArray() )
	{
		if (  m_ArrayIndex >= 0 )
		{
			Result = SvUl::Format(_T("[%d]"), m_ArrayIndex + 1);
		}
		else if(isEntireArray())
		{
			Result = _T("[ ]");
		}
	}
	return Result;
}

HRESULT SVObjectReference::getValue(_variant_t& rValue, int Bucket) const
{
	SvOi::IValueObject* pValueObject = getValueObject();
	if (nullptr != pValueObject)
	{
		return pValueObject->getValue(rValue, ArrayIndex(), Bucket);
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

