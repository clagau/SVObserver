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
	m_Name = m_ObjectRef.GetCompleteObjectName();
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
	HRESULT hr = S_FALSE;
	if ( nullptr != pValueObject )
	{
		CString strValue;		//@WARNING [gra][7.50][22.11.2016] This should be changed to SVString when GetValue for value objects is changed 
		hr = pValueObject->GetValue( strValue );
		if ( S_OK == hr )
		{
			(*this)[ SVValueObjectReference( pValueObject ) ] = strValue;
		}
	}
	return hr;
}

HRESULT SVInputRequestStructMap::Add( const SVValueObjectReference& rObjectRef )
{
	HRESULT Result( S_FALSE );

	ASSERT( rObjectRef.Object() );
	if ( nullptr != rObjectRef.Object() )
	{
		CString strValue;		//@WARNING [gra][7.50][22.11.2016] This should be changed to SVString when GetValue for value objects is changed 
		Result = rObjectRef.GetValue( strValue );
		if ( S_OK == Result )
		{
			(*this)[ rObjectRef ] = strValue;
		}
	}
	return Result;
}

