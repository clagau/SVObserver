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
#include "StringHelper.h"
#include "SVGUID.h"
#pragma endregion Includes

SVGUID::SVGUID()
: m_Guid( GUID_NULL )
{
}

SVGUID::SVGUID( const SVGUID& rGuid )
: m_Guid( GUID_NULL )
{
	*this = rGuid;
}

SVGUID::SVGUID( const GUID& rGuid )
: m_Guid( GUID_NULL )
{
	*this = rGuid;
}

SVGUID::SVGUID(const _bstr_t& rString)
	: m_Guid(GUID_NULL)
{
	*this = rString;
}

SVGUID::SVGUID(const std::string& rString)
	: m_Guid(GUID_NULL)
{
	*this = rString;
}

SVGUID::SVGUID( const _variant_t& rVariant )
: m_Guid( GUID_NULL )
{
	if( VT_BSTR == rVariant.vt )
	{
		*this = _bstr_t( rVariant.bstrVal );
	}
}

SVGUID::~SVGUID()
{
}

bool SVGUID::empty() const
{
	return ( m_Guid == GUID_NULL ) != FALSE;
}

void SVGUID::clear()
{
	m_Guid = GUID_NULL;
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
	
std::string SVGUID::ToString() const
{
	// Make the String Representation have Enclosing Braces and All Hex Upper case
	return SvUl::Format("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		m_Guid.Data1, m_Guid.Data2, m_Guid.Data3,
		m_Guid.Data4[0], m_Guid.Data4[1], m_Guid.Data4[2], m_Guid.Data4[3],
		m_Guid.Data4[4], m_Guid.Data4[5], m_Guid.Data4[6], m_Guid.Data4[7]
		);
}

SVGUID& SVGUID::operator=( const SVGUID& rGuid )
{
	if( this != &rGuid )
	{
		m_Guid = rGuid.m_Guid;
	}

	return *this;
}

SVGUID& SVGUID::operator=( const GUID& rGuid )
{
	if( &m_Guid != &rGuid )
	{
		m_Guid = rGuid;
	}

	return *this;
}

SVGUID& SVGUID::operator=( const _bstr_t& rString )
{
	*this = SvUl::createStdString( rString );

	return *this;
}

SVGUID& SVGUID::operator=(const std::string& rString)
{
	GUID Guid(GUID_NULL);

	std::string GuidString = rString;
	SvUl::RemoveCharacters(GuidString, _T("{}"));

	RPC_CSTR RpcString(reinterpret_cast<unsigned char*> (const_cast<char*> (GuidString.c_str())));
	if (RPC_S_OK == ::UuidFromString(RpcString, &Guid))
	{
		m_Guid = Guid;
	}
	else
	{
		m_Guid = GUID_NULL;
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


