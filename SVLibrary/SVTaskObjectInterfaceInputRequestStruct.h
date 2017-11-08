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
#include "SVObjectLibrary/SVObjectReference.h"

#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

struct SVTaskObjectInterfaceInputRequestStruct
{
	SVTaskObjectInterfaceInputRequestStruct();
	SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef, const SVGUID& rGuid, const std::string& rName );
	SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef );
	SVTaskObjectInterfaceInputRequestStruct( const SVGUID& rGuid );
	SVTaskObjectInterfaceInputRequestStruct( const std::string& rName );

	bool operator < ( const SVTaskObjectInterfaceInputRequestStruct& rRhs ) const;

	SVObjectReference m_ObjectRef;
	SVGUID m_Guid;
	std::string m_Name;
};

struct SVInputRequestStructMap : public std::map <SVTaskObjectInterfaceInputRequestStruct, _variant_t>
{
	HRESULT Add( SVObjectClass* pObject );
	HRESULT Add( const SVObjectReference& rObjectRef );
};

