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

#pragma region Includes
#include "SVValueObjectLibrary/SVValueObjectReference.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

struct SVTaskObjectInterfaceInputRequestStruct
{
	SVValueObjectReference m_ObjectRef;
	SVGUID m_Guid;
	SVString m_Name;

	SVTaskObjectInterfaceInputRequestStruct();
	SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& rObjectRef, const SVGUID& rGuid, const SVString& rName );
	SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& rObjectRef );
	SVTaskObjectInterfaceInputRequestStruct( const SVGUID& rGuid );
	SVTaskObjectInterfaceInputRequestStruct( const SVString& rName );

	bool operator < ( const SVTaskObjectInterfaceInputRequestStruct& rRhs ) const;
};

struct SVInputRequestStructMap : public std::map <SVTaskObjectInterfaceInputRequestStruct, SVString>
{
	HRESULT Add( SVValueObjectClass* pValueObject );
	HRESULT Add( const SVValueObjectReference& rObjectRef );
};

#include "SVTaskObjectInterfaceInputRequestStruct.inl"

