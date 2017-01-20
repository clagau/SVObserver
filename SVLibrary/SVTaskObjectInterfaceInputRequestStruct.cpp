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

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVGUID& rGuid )
{
	m_Guid = rGuid;
	m_ObjectRef = SVValueObjectReference( SVObjectManagerClass::Instance().GetObject( m_Guid ) );
	m_Name = m_ObjectRef.GetCompleteName();
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVString& rName )
{
	m_Name = rName;
	m_ObjectRef = SVObjectManagerClass::Instance().GetObjectReference( m_Name.c_str() );
	m_Guid = m_ObjectRef.Guid();
}

HRESULT SVInputRequestStructMap::Add( SVValueObjectClass* pValueObject )
{
	ASSERT( pValueObject );
	HRESULT Result( E_FAIL );
	if ( nullptr != pValueObject )
	{
		SVString Value;
		Result = pValueObject->GetValue( Value );
		if ( S_OK == Result )
		{
			(*this)[ SVValueObjectReference( pValueObject ) ] = Value;
		}
	}
	return Result;
}

HRESULT SVInputRequestStructMap::Add( const SVValueObjectReference& rObjectRef )
{
	HRESULT Result( E_FAIL );

	ASSERT( rObjectRef.Object() );
	if ( nullptr != rObjectRef.Object() )
	{
		SVString Value;
		Result = rObjectRef.GetValue( Value );
		if ( S_OK == Result )
		{
			(*this)[ rObjectRef ] = Value;
		}
	}
	return Result;
}

