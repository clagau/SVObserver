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
#pragma endregion Includes

namespace SvIe
{

struct SVTaskObjectInterfaceInputRequestStruct
{
	SVTaskObjectInterfaceInputRequestStruct() = default;
	SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef, uint32_t objectId, const std::string& rName );
	SVTaskObjectInterfaceInputRequestStruct( const SVObjectReference& rObjectRef );
	explicit SVTaskObjectInterfaceInputRequestStruct(uint32_t objectId );
	SVTaskObjectInterfaceInputRequestStruct( const std::string& rName );

	bool operator < ( const SVTaskObjectInterfaceInputRequestStruct& rRhs ) const;

	SVObjectReference m_ObjectRef;
	uint32_t m_objectId = SvDef::InvalidObjectId;
	std::string m_Name;
};

struct SVInputRequestStructMap : public std::map <SVTaskObjectInterfaceInputRequestStruct, _variant_t>
{
	HRESULT Add( SVObjectClass* pObject );
	HRESULT Add( const SVObjectReference& rObjectRef );
};

} //namespace SvIe
