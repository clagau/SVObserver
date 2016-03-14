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

#include "stdafx.h"
#include "SVTaskObjectInterfaceInputRequestStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( GUID p_guid )
{
	guid = p_guid;
	ref = SVValueObjectReference( SVObjectManagerClass::Instance().GetObject( guid ) );
	strName = ref.GetCompleteObjectName();
}

SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const CString& p_strName )
{
	strName = p_strName;
	ref = SVObjectManagerClass::Instance().GetObjectReference( strName );
	guid = ref.Guid();
}

HRESULT SVInputRequestStructMap::Add( SVValueObjectClass* p_pValueObject )
{
	ASSERT( p_pValueObject );
	HRESULT hr = S_FALSE;
	if ( p_pValueObject )
	{
		CString strValue;
		hr = p_pValueObject->GetValue( strValue );
		if ( hr == S_OK )
		{
			(*this)[ SVValueObjectReference( p_pValueObject ) ] = strValue;
		}
	}
	return hr;
}

HRESULT SVInputRequestStructMap::Add( SVValueObjectReference p_refValueObject )
{
	ASSERT( p_refValueObject.Object() );
	HRESULT hr = S_FALSE;
	if ( p_refValueObject.Object() )
	{
		CString strValue;
		hr = p_refValueObject.GetValue( strValue );
		if ( hr == S_OK )
		{
			(*this)[ p_refValueObject ] = strValue;
		}
	}
	return hr;
}

