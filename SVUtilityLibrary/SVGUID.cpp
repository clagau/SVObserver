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

#include "stdafx.h"
#include "SVGUID.h"
#include "SVString.h"

const GUID  SVInvalidGUID = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

SVGUID::SVGUID()
: m_Guid( SV_GUID_NULL )
{
}

SVGUID::SVGUID( const SVGUID& p_rObject )
: m_Guid( SV_GUID_NULL )
{
	*this = p_rObject;
}

SVGUID::SVGUID( const GUID& p_rGuid )
: m_Guid( SV_GUID_NULL )
{
	*this = p_rGuid;
}

SVGUID::SVGUID( const BSTR& p_rString )
: m_Guid( SV_GUID_NULL )
{
	*this = p_rString;
}

SVGUID::SVGUID( const VARIANT& p_rVariant )
: m_Guid( SV_GUID_NULL )
{
	*this = p_rVariant;
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
	/*
	_bstr_t l_OutString;

	RPC_WSTR l_RpcString( NULL );

	UUID* l_pUuid( const_cast< UUID* >( &m_Guid ) );

	if( ::UuidToStringW( l_pUuid, &l_RpcString ) == RPC_S_OK )
	{
		wchar_t* l_pTempString( NULL );

		l_pTempString = reinterpret_cast< wchar_t* >( l_RpcString );

		l_OutString = l_pTempString;

		::RpcStringFreeW( &l_RpcString );
	}
	*/

	// Make the String Representation have Enclosing Braces and All Hex Upper case
	return SvUl_SF::Format("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		m_Guid.Data1, m_Guid.Data2, m_Guid.Data3,
		m_Guid.Data4[0], m_Guid.Data4[1], m_Guid.Data4[2], m_Guid.Data4[3],
		m_Guid.Data4[4], m_Guid.Data4[5], m_Guid.Data4[6], m_Guid.Data4[7]
		);
}


const SVGUID& SVGUID::operator=( const SVGUID& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Guid = p_rObject.m_Guid;
	}

	return *this;
}

const SVGUID& SVGUID::operator=( const GUID& p_rGuid )
{
	if( &m_Guid != &p_rGuid )
	{
		m_Guid = p_rGuid;
	}

	return *this;
}

const SVGUID& SVGUID::operator=( const BSTR& p_rString )
{
	GUID l_Guid( SV_GUID_NULL );

	wchar_t* l_pTempString( static_cast< wchar_t* >( p_rString ) );

	if( l_pTempString[ 0 ] == L'{' )
	{
		size_t l_Size( ::wcslen( l_pTempString ) );
		size_t l_DestPos( 0 );

		for( size_t l = 0; l < l_Size; ++l )
		{
			if( l_pTempString[ l ] != L'{' )
			{
				if( l_pTempString[ l ] == L'}' )
				{
					l_pTempString[ l_DestPos ] = NULL;

					break;
				}
				else
				{
					l_pTempString[ l_DestPos++ ] = l_pTempString[ l ];
				}
			}
		}
	}

	RPC_WSTR l_RpcString( reinterpret_cast< RPC_WSTR >( l_pTempString ) );

	if( ::UuidFromStringW( l_RpcString, &l_Guid ) == RPC_S_OK )
	{
		m_Guid = l_Guid;
	}
	else
	{
		m_Guid = SV_GUID_NULL;
	}

	return *this;
}

const SVGUID& SVGUID::operator=( const VARIANT& p_rVariant )
{
	_variant_t l_Variant( p_rVariant );

	*this = static_cast< _bstr_t >( l_Variant );

	return *this;
}

bool SVGUID::operator==( const SVGUID& p_rObject ) const
{
	return ( m_Guid == p_rObject.m_Guid ) != FALSE;
}

bool SVGUID::operator==( const GUID& p_rGuid ) const
{
	return ( m_Guid == p_rGuid ) != FALSE;
}

bool SVGUID::operator<( const SVGUID& p_rObject ) const
{
	return m_Guid < p_rObject.m_Guid;
}

bool SVGUID::operator<( const GUID& p_rGuid ) const
{
	return m_Guid < p_rGuid;
}

bool SVGUID::operator>( const SVGUID& p_rObject ) const
{
	return m_Guid > p_rObject.m_Guid;
}

bool SVGUID::operator>( const GUID& p_rGuid ) const
{
	return m_Guid > p_rGuid;
}


