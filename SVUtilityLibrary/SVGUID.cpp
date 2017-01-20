//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGUID
//* .File Name       : $Workfile:   SVGUID.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVGUID.h"
#pragma endregion Includes

const GUID SV_GUID_NULL = GUID_NULL;

SVGUID::SVGUID()
: m_Guid( SV_GUID_NULL )
{
}

SVGUID::SVGUID( const SVGUID& rObject )
: m_Guid( SV_GUID_NULL )
{
	*this = rObject;
}

SVGUID::SVGUID( const GUID& rGuid )
: m_Guid( SV_GUID_NULL )
{
	*this = rGuid;
}

SVGUID::SVGUID( const _bstr_t& rString )
: m_Guid( SV_GUID_NULL )
{
	*this = rString;
}

SVGUID::SVGUID( const _variant_t& rVariant )
: m_Guid( SV_GUID_NULL )
{
	if( VT_BSTR == rVariant.vt )
	{
		*this = _bstr_t( rVariant.bstrVal );
	}
}

SVGUID::~SVGUID()
{
	m_Guid = SV_GUID_NULL;
}

bool SVGUID::empty() const
{
	return ( m_Guid == SV_GUID_NULL ) != FALSE;
}

void SVGUID::clear()
{
	m_Guid = SV_GUID_NULL;
}

SVGUID::operator const GUID& () const
{
	return m_Guid;
}

SVGUID::operator GUID& ()
{
	return m_Guid;
}

GUID* SVGUID::operator&()
{
	return &m_Guid;
}
	
const GUID& SVGUID::ToGUID() const
{
	return m_Guid;
}

GUID& SVGUID::ToGUID()
{
	return m_Guid;
}

_variant_t SVGUID::ToVARIANT() const
{
	return _variant_t( ToString().c_str());
}

_bstr_t SVGUID::ToBSTR() const
{
	return _bstr_t( ToString().c_str() );
}

SVString SVGUID::ToString() const
{
	// Make the String Representation have Enclosing Braces and All Hex Upper case
	return SvUl_SF::Format("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		m_Guid.Data1, m_Guid.Data2, m_Guid.Data3,
		m_Guid.Data4[0], m_Guid.Data4[1], m_Guid.Data4[2], m_Guid.Data4[3],
		m_Guid.Data4[4], m_Guid.Data4[5], m_Guid.Data4[6], m_Guid.Data4[7]
		);
}

const SVGUID& SVGUID::operator=( const SVGUID& rObject )
{
	if( this != &rObject )
	{
		m_Guid = rObject.m_Guid;
	}

	return *this;
}

const SVGUID& SVGUID::operator=( const GUID& rGuid )
{
	if( &m_Guid != &rGuid )
	{
		m_Guid = rGuid;
	}

	return *this;
}

const SVGUID& SVGUID::operator=( const _bstr_t& rString )
{
	GUID Guid( SV_GUID_NULL );

	SVString GuidString = SvUl_SF::createSVString( rString );
	SvUl_SF::RemoveCharacters(GuidString, _T("{}") );

	RPC_CSTR RpcString( reinterpret_cast<unsigned char*> ( const_cast<char*> ( GuidString.c_str() )  ) );
	if( RPC_S_OK == ::UuidFromString( RpcString , &Guid ) )
	{
		m_Guid = Guid;
	}
	else
	{
		m_Guid = SV_GUID_NULL;
	}

	return *this;
}

bool SVGUID::operator==( const SVGUID& rObject ) const
{
	return ( m_Guid == rObject.m_Guid ) != false;
}

bool SVGUID::operator==( const GUID& rGuid ) const
{
	return ( m_Guid == rGuid ) != false;
}

bool SVGUID::operator<( const SVGUID& rObject ) const
{
	return m_Guid < rObject.m_Guid;
}

bool SVGUID::operator<( const GUID& rGuid ) const
{
	return m_Guid < rGuid;
}

bool SVGUID::operator>( const SVGUID& rObject ) const
{
	return m_Guid > rObject.m_Guid;
}

bool SVGUID::operator>( const GUID& rGuid ) const
{
	return m_Guid > rGuid;
}


