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
	m_ArrayIndex(-1), 
	m_IsArray(false)
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SVInvalidGUID;
	m_NameInfo.clear();
	
}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue ):
	m_ArrayIndex(lArrayIndex),  
	m_IsArray(true)
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SVInvalidGUID;
	if( m_pObject != NULL )
	{
		m_NameInfo.ParseObjectName( m_NameInfo, static_cast< LPCTSTR >( m_pObject->GetCompleteObjectName() ) );
	}
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.SetIndex( SvUl_SF::Format(_T("%d"), lArrayIndex ));
	m_NameInfo.SetIsDefaultValuePresent(true);
	m_NameInfo.SetDefaultValue(static_cast< LPCTSTR >( strDefaultValue ));
	

}

SVObjectReference::SVObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo )
{
	m_pObject = pObject;
	m_Guid = m_pObject ? m_pObject->GetUniqueObjectID() : SVInvalidGUID;
	m_NameInfo = p_rNameInfo;
	m_IsArray =  p_rNameInfo.IsIndexPresent();
	m_ArrayIndex = p_rNameInfo.GetIndexValue();

}

void SVObjectReference::SetEntireArray()
{
	m_ArrayIndex = -1;  
	m_IsArray = true;
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.ClearIndex();
}

void SVObjectReference::SetArrayIndex( long lArrayIndex )
{
	m_ArrayIndex = lArrayIndex;  
	m_IsArray = true;
	m_NameInfo.SetIsIndexPresent(true);
	m_NameInfo.SetIndex( SvUl_SF::Format(_T("%d"), lArrayIndex ) );
}

CString SVObjectReference::GetName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetName();
		strName += GetZeroBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteObjectName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectName();
		strName += GetZeroBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteObjectNameToObjectType( LPCSTR lpszCompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectNameToObjectType(lpszCompleteName, objectTypeToInclude);
		strName += GetZeroBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetOneBasedName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetName();
		strName += GetOneBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteOneBasedObjectName() const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectName();
		strName += GetOneBasedIndexString();
	}
	return strName;
}

CString SVObjectReference::GetCompleteOneBasedObjectNameToObjectType( LPCSTR lpszCompleteName, SVObjectTypeEnum objectTypeToInclude ) const
{
	CString strName;
	if ( m_pObject )
	{
		strName = m_pObject->GetCompleteObjectNameToObjectType(lpszCompleteName, objectTypeToInclude);
		strName += GetOneBasedIndexString();
	}
	return strName;
}

const SVObjectNameInfo& SVObjectReference::GetObjectNameInfo() const
{
	return m_NameInfo;
}


CString SVObjectReference::GetZeroBasedIndexString() const
{
	CString s;
	if( m_IsArray)
	{
		if ( m_ArrayIndex >= 0 )
		{
			s.Format(_T("[%d]"), m_ArrayIndex );
		}
		else
		{
			s = _T("[ ]");
		}
	}
	return s;
}


CString SVObjectReference::GetOneBasedIndexString() const
{
	CString s;
	if( m_IsArray)
	{
		if (  m_ArrayIndex >= 0 )
		{
			s.Format(_T("[%d]"), m_ArrayIndex + 1);
		}
		else
		{
			s = _T("[ ]");
		}
	}
	return s;
}

const SVString& SVObjectReference::GetIndex() const
{
	
	return m_NameInfo.GetIndex();
}

const UINT SVObjectReference::ObjectAttributesAllowed() const
{
	ASSERT( m_pObject != nullptr );
	return m_pObject->ObjectAttributesAllowed();
}

const UINT SVObjectReference::ObjectAttributesSet() const
{
	ASSERT( m_pObject != nullptr );
	return m_pObject->ObjectAttributesSet(m_ArrayIndex >= 0 ? m_ArrayIndex:0  );

}

UINT& SVObjectReference::ObjectAttributesAllowedRef()
{
	ASSERT( m_pObject != nullptr );
	return m_pObject->ObjectAttributesAllowedRef();
}

SVObjectAttributeShim SVObjectReference::ObjectAttributesSetRef()
{
	ASSERT( m_pObject != nullptr );
	return m_pObject->ObjectAttributesSetRef(m_ArrayIndex >= 0 ? m_ArrayIndex:0 );

}

GUID SVObjectReference::GetObjectGuid( SVObjectClass* pObject )
{
	return pObject->GetUniqueObjectID();
}

int SVObjectReference::IncrementIndex()
{
	if(m_IsArray && m_ArrayIndex > -1) 
	{

		m_ArrayIndex++;
	}

	return m_ArrayIndex;
}

