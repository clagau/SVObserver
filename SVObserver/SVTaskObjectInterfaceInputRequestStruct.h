//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceInputRequestStruct
//* .File Name       : $Workfile:   SVTaskObjectInterfaceInputRequestStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:33:52  $
//******************************************************************************

#pragma once

#include "SVValueObjectReference.h"

struct SVTaskObjectInterfaceInputRequestStruct
{
	SVValueObjectReference ref;
	GUID guid;
	CString strName;

	SVTaskObjectInterfaceInputRequestStruct();
	SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref, GUID p_guid, const CString& p_strName );
	SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref );
	SVTaskObjectInterfaceInputRequestStruct( GUID p_guid );
	SVTaskObjectInterfaceInputRequestStruct( const CString& p_strName );

	bool operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const;
};

struct SVInputRequestStructMap : public std::map <SVTaskObjectInterfaceInputRequestStruct, CString>
{
	HRESULT Add( SVValueObjectClass* p_pValueObject );
	HRESULT Add( SVValueObjectReference p_refValueObject );
};

#include "SVTaskObjectInterfaceInputRequestStruct.inl"

