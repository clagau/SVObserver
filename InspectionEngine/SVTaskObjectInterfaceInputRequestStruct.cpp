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

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef, uint32_t objectId, const std::string& rName )
	: m_ObjectRef(rObjectRef)
	, m_objectId(objectId)
	, m_Name(rName)
{

}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct(const SVObjectReference& rObjectRef )
	: m_ObjectRef(rObjectRef)
{
	if( nullptr != m_ObjectRef.getObject() )
	{
		m_objectId = m_ObjectRef.getObjectId();
		m_Name = m_ObjectRef.GetCompleteName();
	}
}

bool SVTaskObjectInterfaceInputRequestStruct::operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const
{
	if( nullptr != m_ObjectRef.getObject() && nullptr != rhs.m_ObjectRef.getObject() )
	{
		return m_ObjectRef < rhs.m_ObjectRef;
	}
	else if (SvDef::InvalidObjectId != m_objectId && SvDef::InvalidObjectId != rhs.m_objectId )
	{
		return m_objectId < rhs.m_objectId;
	}
	else
	{
		return m_Name < rhs.m_Name;
	}
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct(uint32_t objectId)
	: m_objectId(objectId)
{
	m_ObjectRef = SVObjectReference( SVObjectManagerClass::Instance().GetObject( m_objectId ) );
	m_Name = m_ObjectRef.GetCompleteName();
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const std::string& rName )
	: m_Name(rName)
{
	m_ObjectRef = SVObjectManagerClass::Instance().GetObjectReference( m_Name.c_str() );
	m_objectId = m_ObjectRef.getObjectId();
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
			(*this)[SVTaskObjectInterfaceInputRequestStruct(SVObjectReference( pObject )) ] = Value;
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
			(*this)[SVTaskObjectInterfaceInputRequestStruct(rObjectRef) ] = Value;
		}
	}
	return Result;
}

} //namespace SvIe
