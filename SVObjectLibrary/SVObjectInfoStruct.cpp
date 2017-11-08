//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectInfoStruct
//* .File Name       : $Workfile:   SVObjectInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectClass.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectInfoStruct::SVObjectInfoStruct()
: m_pObject( nullptr ), m_UniqueObjectID(), m_ObjectTypeInfo(), m_ObjectNameInfo()
{
}

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectInfoStruct& rObjectInfo )
: m_pObject( rObjectInfo.m_pObject )
, m_UniqueObjectID( rObjectInfo.m_UniqueObjectID )
, m_ObjectTypeInfo( rObjectInfo.m_ObjectTypeInfo )
, m_ObjectNameInfo( rObjectInfo.m_ObjectNameInfo )
{
}

SVObjectInfoStruct::SVObjectInfoStruct( SVObjectClass* pObject )
: m_pObject( nullptr ), 
m_UniqueObjectID(), 
m_ObjectTypeInfo(), 
m_ObjectNameInfo()
{
	SetObject( pObject );
}

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectReference& rObjectRef )
: m_pObject( nullptr )
, m_UniqueObjectID()
, m_ObjectTypeInfo()
, m_ObjectNameInfo()
{
	SetObject( rObjectRef );
}

SVObjectInfoStruct::~SVObjectInfoStruct()
{
}

const SVObjectInfoStruct& SVObjectInfoStruct::operator = ( const SVObjectInfoStruct& rObjectInfo )
{
	if( this != &rObjectInfo )
	{
		m_pObject = rObjectInfo.m_pObject;
		m_UniqueObjectID = rObjectInfo.m_UniqueObjectID;
		m_ObjectTypeInfo = rObjectInfo.m_ObjectTypeInfo;
		m_ObjectNameInfo = rObjectInfo.m_ObjectNameInfo;
	}
	return *this;
}

void SVObjectInfoStruct::clear()
{
	m_pObject = nullptr;
	m_UniqueObjectID.clear();
	m_ObjectTypeInfo = SVObjectTypeInfoStruct();
	m_ObjectNameInfo.clear();
}

void SVObjectInfoStruct::ClearObjectInfo()
{
	m_pObject = nullptr;
	m_UniqueObjectID.clear();
}

HRESULT SVObjectInfoStruct::SetObject( const SVGUID& rObjectID )
{
	HRESULT l_hrOk = S_OK;

	if( rObjectID != m_UniqueObjectID )
	{
		ClearObjectInfo();
		m_UniqueObjectID = rObjectID;
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const std::string& rName )
{
	HRESULT l_hrOk = S_OK;

	if( rName != m_ObjectNameInfo.GetObjectArrayName() )
	{
		ClearObjectInfo();
		l_hrOk = m_ObjectNameInfo.ParseObjectName( rName );
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectTypeInfoStruct& rTypeInfo )
{
	HRESULT l_hrOk = S_OK;
	clear();
	m_ObjectTypeInfo = rTypeInfo;
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( SVObjectClass* pObject )
{
	HRESULT l_hrOk = S_OK;

	try
	{
		if( nullptr != pObject )
		{
			clear();

			m_pObject = pObject;

			m_ObjectTypeInfo = m_pObject->GetObjectInfo().m_ObjectTypeInfo;

			m_UniqueObjectID = m_pObject->GetUniqueObjectID();

			if( SV_GUID_NULL == m_UniqueObjectID )
			{
				m_UniqueObjectID = m_pObject->GetObjectInfo().m_UniqueObjectID;
			}

			m_ObjectNameInfo.ParseObjectName( m_pObject->GetCompleteName().c_str() );
		}
		else
		{
			ClearObjectInfo();
		}
	}
	catch( ... )
	{
		ClearObjectInfo();

		l_hrOk = S_FALSE;
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectReference& rObjectRef )
{
	HRESULT l_hrOk = SetObject( rObjectRef.getObject() );

	if( S_OK == l_hrOk )
	{
		m_UniqueObjectID = rObjectRef.Guid();
		m_ObjectNameInfo = rObjectRef.GetObjectNameInfo();
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectInfoStruct& rObjectInfo )
{
	HRESULT l_hrOk = S_OK;

	if( this != &rObjectInfo )
	{
		m_pObject = rObjectInfo.m_pObject;
		m_UniqueObjectID = rObjectInfo.m_UniqueObjectID;
		m_ObjectTypeInfo = rObjectInfo.m_ObjectTypeInfo;
		m_ObjectNameInfo = rObjectInfo.m_ObjectNameInfo;
	}
	return l_hrOk;
}

bool SVObjectInfoStruct::operator == ( const SVObjectInfoStruct& rRhs ) const
{
	bool l_Status = ( m_UniqueObjectID == rRhs.m_UniqueObjectID );

	if( m_ObjectNameInfo.IsIndexPresent() )
	{
		l_Status = m_ObjectNameInfo.GetIndex() == rRhs.m_ObjectNameInfo.GetIndex();
	}
	return l_Status;
}

bool SVObjectInfoStruct::operator == ( const SVObjectReference& rRhs ) const
{
	bool l_Status = true;

	if ( nullptr != rRhs.getObject() )
	{
		l_Status = ( m_UniqueObjectID == rRhs.getObject()->GetUniqueObjectID() );

		if( m_ObjectNameInfo.IsIndexPresent() )
		{
			l_Status = m_ObjectNameInfo.GetIndex() == rRhs.GetObjectNameInfo().GetIndex();
		}
	}
	else
	{
		l_Status = ( SV_GUID_NULL == m_UniqueObjectID );
	}
	return l_Status;
}

SVObjectReference SVObjectInfoStruct::GetObjectReference() const
{
	SVObjectReference l_Ref( m_pObject );

	if( m_ObjectNameInfo.IsIndexPresent() )
	{
		long l_Index = m_ObjectNameInfo.GetIndexValue();

		if( 0 <= l_Index )
		{
			l_Ref.SetArrayIndex( l_Index );
		}
		else
		{
			l_Ref.SetEntireArray();
		}
	}
	return l_Ref;
}

bool SVObjectInfoStruct::CheckExistence()
{
	bool l_bOk = true;

	l_bOk = l_bOk && SV_GUID_NULL != m_UniqueObjectID;

	if( l_bOk )
	{
		SVObjectClass *l_psvObject = SVObjectManagerClass::Instance().GetObject( m_UniqueObjectID );

		l_bOk = l_bOk && l_psvObject == m_pObject;
	}

	l_bOk = l_bOk && nullptr != dynamic_cast<SVObjectClass *>(m_pObject);

	if( l_bOk )
	{
		GUID l_guidId = m_pObject->GetUniqueObjectID();

		l_bOk = l_bOk && l_guidId == m_UniqueObjectID;
	}
	return l_bOk;
}

