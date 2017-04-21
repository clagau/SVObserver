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
#pragma endregion Includes

SVObjectReference::SVObjectReference( SVObjectClass* pObject ):
	m_ArrayIndex(-1)
{
	m_pObject = pObject;
	m_pValueObject = nullptr;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL;
	m_NameInfo.clear();
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, long lArrayIndex, SVString strDefaultValue ):
	m_ArrayIndex(lArrayIndex)  
{
	m_pObject = pObject;
	m_pValueObject = nullptr;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL;
	if( nullptr != m_pObject )
	{
		m_NameInfo.ParseObjectName( m_NameInfo, m_pObject->GetCompleteName().c_str() );
	}
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.SetIndex( SvUl_SF::Format(_T("%d"), lArrayIndex ));
	m_NameInfo.SetIsDefaultValuePresent(true);
	m_NameInfo.SetDefaultValue( strDefaultValue );
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo )
{
	m_pObject = pObject;
	m_pValueObject = nullptr;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SV_GUID_NULL;
	m_NameInfo = p_rNameInfo;
	m_ArrayIndex = p_rNameInfo.GetIndexValue();
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
	m_NameInfo.SetIndex( SvUl_SF::Format(_T("%d"), lArrayIndex ) );
}

SVString SVObjectReference::GetName() const
{
	SVString Result;
	
	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetName();
		Result += GetZeroBasedIndexString();
	}
	return Result;
}

SVString SVObjectReference::GetCompleteName() const
{
	SVString Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteName();
		Result += GetZeroBasedIndexString();
	}
	return Result;
}

SVString SVObjectReference::GetCompleteObjectNameToObjectType( LPCTSTR lpszCompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	SVString Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteObjectNameToObjectType(lpszCompleteName, objectTypeToInclude);
		Result += GetZeroBasedIndexString();
	}
	return Result;
}

SVString SVObjectReference::GetOneBasedName() const
{
	SVString Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetName();
		Result += GetOneBasedIndexString();
	}
	return Result;
}

SVString SVObjectReference::GetCompleteOneBasedObjectName() const
{
	SVString Result;

	if ( nullptr != m_pObject )
	{
		Result = m_pObject->GetCompleteName();
		Result += GetOneBasedIndexString();
	}
	return Result;
}

const SVObjectNameInfo& SVObjectReference::GetObjectNameInfo() const
{
	return m_NameInfo;
}

SVString SVObjectReference::GetZeroBasedIndexString() const
{
	SVString Result;

	if( isArray() )
	{
		if ( m_ArrayIndex >= 0 )
		{
			Result = SvUl_SF::Format(_T("[%d]"), m_ArrayIndex );
		}
		else if (isEntireArray())
		{
			Result = _T("[ ]");
		}
	}
	return Result;
}

SVString SVObjectReference::GetOneBasedIndexString() const
{
	SVString Result;

	if( isArray() )
	{
		if (  m_ArrayIndex >= 0 )
		{
			Result = SvUl_SF::Format(_T("[%d]"), m_ArrayIndex + 1);
		}
		else if(isEntireArray())
		{
			Result = _T("[ ]");
		}
	}
	return Result;
}

const SVString& SVObjectReference::GetIndex() const
{
	
	return m_NameInfo.GetIndex();
}

const UINT SVObjectReference::ObjectAttributesAllowed() const
{
	assert( nullptr != m_pObject );
	return m_pObject->ObjectAttributesAllowed();
}

const UINT SVObjectReference::ObjectAttributesSet() const
{
	assert( nullptr != m_pObject );
	return m_pObject->ObjectAttributesSet(m_ArrayIndex >= 0 ? m_ArrayIndex : 0  );
}

void SVObjectReference::SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type )
{
	assert( nullptr != m_pObject );
	return m_pObject->SetObjectAttributesAllowed( Attributes, Type );
}

void SVObjectReference::SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type )
{
	assert( nullptr != m_pObject );
	return m_pObject->SetObjectAttributesSet( Attributes, Type,  m_ArrayIndex >= 0 ? m_ArrayIndex : 0 );
}

long SVObjectReference::IncrementIndex()
{
	if( isArray() && m_ArrayIndex > -1) 
	{

		m_ArrayIndex++;
	}

	return m_ArrayIndex;
}

