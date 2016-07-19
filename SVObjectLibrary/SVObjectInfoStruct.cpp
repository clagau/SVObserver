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

SVObjectInfoStruct::SVObjectInfoStruct()
: PObject( nullptr ), UniqueObjectID(), ObjectTypeInfo( SVInvalidObjectTypeInfo ), m_ObjectNameInfo()
{
}

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectInfoStruct& O2 )
: PObject( O2.PObject ), UniqueObjectID( O2.UniqueObjectID ), ObjectTypeInfo( O2.ObjectTypeInfo ), m_ObjectNameInfo( O2.m_ObjectNameInfo )
{
}

SVObjectInfoStruct::SVObjectInfoStruct( SVObjectClass* PO2 )
: PObject( nullptr ), UniqueObjectID(), ObjectTypeInfo( SVInvalidObjectTypeInfo ), m_ObjectNameInfo()
{
	SetObject( PO2 );
}

SVObjectInfoStruct::SVObjectInfoStruct( SVObjectReference ref )
: PObject( nullptr ), UniqueObjectID(), ObjectTypeInfo( SVInvalidObjectTypeInfo ), m_ObjectNameInfo()
{
	SetObject( ref );
}

SVObjectInfoStruct::~SVObjectInfoStruct()
{
}

const SVObjectInfoStruct& SVObjectInfoStruct::operator = ( const SVObjectInfoStruct& O2 )
{
	if( this != &O2 )
	{
		PObject = O2.PObject;
		UniqueObjectID = O2.UniqueObjectID;
		ObjectTypeInfo = O2.ObjectTypeInfo;
		m_ObjectNameInfo = O2.m_ObjectNameInfo;
	}
	return *this;
}

void SVObjectInfoStruct::clear()
{
	PObject = nullptr;
	UniqueObjectID.clear();
	ObjectTypeInfo = SVInvalidObjectTypeInfo;
	m_ObjectNameInfo.clear();
}

void SVObjectInfoStruct::ClearObjectInfo()
{
	PObject = nullptr;
	UniqueObjectID.clear();
}

HRESULT SVObjectInfoStruct::SetObject( const SVGUID& p_rObjectID )
{
	HRESULT l_hrOk = S_OK;

	if( p_rObjectID != UniqueObjectID )
	{
		ClearObjectInfo();
		UniqueObjectID = p_rObjectID;
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVString& p_rName )
{
	HRESULT l_hrOk = S_OK;

	if( p_rName != m_ObjectNameInfo.GetObjectArrayName() )
	{
		ClearObjectInfo();
		l_hrOk = m_ObjectNameInfo.ParseObjectName( p_rName );
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectTypeInfoStruct& p_rTypeInfo )
{
	HRESULT l_hrOk = S_OK;
	clear();
	ObjectTypeInfo = p_rTypeInfo;
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( SVObjectClass* p_psvObject )
{
	HRESULT l_hrOk = S_OK;

	try
	{
		if( nullptr != p_psvObject )
		{
			clear();

			PObject = p_psvObject;

			ObjectTypeInfo = PObject->GetObjectInfo().ObjectTypeInfo;

			UniqueObjectID = PObject->GetUniqueObjectID();

			if( SV_GUID_NULL == UniqueObjectID )
			{
				UniqueObjectID = PObject->GetObjectInfo().UniqueObjectID;
			}

			m_ObjectNameInfo.ParseObjectName( static_cast< LPCTSTR >( PObject->GetCompleteObjectName() ) );
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

HRESULT SVObjectInfoStruct::SetObject( const SVObjectReference& p_svObject )
{
	HRESULT l_hrOk = SetObject( p_svObject.Object() );

	if( S_OK == l_hrOk )
	{
		UniqueObjectID = p_svObject.Guid();
		m_ObjectNameInfo = p_svObject.GetObjectNameInfo();
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectInfoStruct& p_rObject )
{
	HRESULT l_hrOk = S_OK;

	if( this != &p_rObject )
	{
		PObject = p_rObject.PObject;
		UniqueObjectID = p_rObject.UniqueObjectID;
		ObjectTypeInfo = p_rObject.ObjectTypeInfo;
		m_ObjectNameInfo = p_rObject.m_ObjectNameInfo;
	}
	return l_hrOk;
}

bool SVObjectInfoStruct::operator == ( const SVObjectInfoStruct& rhs ) const
{
	bool l_Status = ( UniqueObjectID == rhs.UniqueObjectID );

	if( m_ObjectNameInfo.IsIndexPresent() )
	{
		l_Status = m_ObjectNameInfo.GetIndex() == rhs.m_ObjectNameInfo.GetIndex();
	}
	return l_Status;
}

bool SVObjectInfoStruct::operator == ( const SVObjectReference& rhs ) const
{
	bool l_Status = true;

	if ( nullptr != rhs.Object() )
	{
		l_Status = ( UniqueObjectID == rhs->GetUniqueObjectID() );

		if( m_ObjectNameInfo.IsIndexPresent() )
		{
			l_Status = m_ObjectNameInfo.GetIndex() == rhs.GetObjectNameInfo().GetIndex();
		}
	}
	else
	{
		l_Status = ( SV_GUID_NULL == UniqueObjectID );
	}
	return l_Status;
}

SVObjectReference SVObjectInfoStruct::GetObjectReference() const
{
	SVObjectReference l_Ref( PObject );

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

BOOL SVObjectInfoStruct::CheckExistence()
{
	BOOL l_bOk = true;

	l_bOk = l_bOk && SV_GUID_NULL != UniqueObjectID;

	if( l_bOk )
	{
		SVObjectClass *l_psvObject = SVObjectManagerClass::Instance().GetObject( UniqueObjectID );

		l_bOk = l_bOk && l_psvObject == PObject;
	}

	l_bOk = l_bOk && nullptr != dynamic_cast<SVObjectClass *>(PObject);

	if( l_bOk )
	{
		GUID l_guidId = PObject->GetUniqueObjectID();

		l_bOk = l_bOk && l_guidId == UniqueObjectID;
	}
	return l_bOk;
}

