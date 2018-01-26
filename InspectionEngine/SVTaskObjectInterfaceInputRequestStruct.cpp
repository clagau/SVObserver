//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceInputRequestStruct
//* .File Name       : $Workfile:   SVTaskObjectInterfaceInputRequestStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:26:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTaskObjectInterfaceInputRequestStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct()
{
	m_Guid = GUID_NULL;
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef, const SVGUID& rGuid, const std::string& rName )
{
	m_ObjectRef = rObjectRef;
	m_Guid = rGuid;
	m_Name = rName;
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef )
{
	m_ObjectRef = rObjectRef;
	if( nullptr != m_ObjectRef.getObject() )
	{
		m_Guid = m_ObjectRef.Guid();
		m_Name = m_ObjectRef.GetCompleteName();
	}
}

inline bool SVTaskObjectInterfaceInputRequestStruct::operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const
{
	if( nullptr != m_ObjectRef.getObject() && nullptr != rhs.m_ObjectRef.getObject() )
	{
		return m_ObjectRef < rhs.m_ObjectRef;
	}
	else if ( GUID_NULL != m_Guid && GUID_NULL != rhs.m_Guid )
	{
		return m_Guid < rhs.m_Guid;
	}
	else
	{
		return m_Name < rhs.m_Name;
	}
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVGUID& rGuid )
{
	m_Guid = rGuid;
	m_ObjectRef = SVObjectReference( SVObjectManagerClass::Instance().GetObject( m_Guid ) );
	m_Name = m_ObjectRef.GetCompleteName();
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const std::string& rName )
{
	m_Name = rName;
	m_ObjectRef = SVObjectManagerClass::Instance().GetObjectReference( m_Name.c_str() );
	m_Guid = m_ObjectRef.Guid();
}

HRESULT SVInputRequestStructMap::Add( SVObjectClass* pObject )
{
	assert( pObject );
	HRESULT Result( E_FAIL );
	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
	if ( nullptr != pValueObject )
	{
		_variant_t Value;
		Result = pValueObject->getValue( Value );
		if ( S_OK == Result )
		{
			(*this)[ SVObjectReference( pObject ) ] = Value;
		}
	}
	return Result;
}

HRESULT SVInputRequestStructMap::Add( const SVObjectReference& rObjectRef )
{
	HRESULT Result( E_FAIL );

	SvOi::IValueObject* pValueObject( rObjectRef.getValueObject() );
	assert( pValueObject );
	if ( nullptr != pValueObject )
	{
		_variant_t Value;
		Result = pValueObject->getValue( Value, rObjectRef.ArrayIndex() );
		if ( S_OK == Result )
		{
			(*this)[ rObjectRef ] = Value;
		}
	}
	return Result;
}

