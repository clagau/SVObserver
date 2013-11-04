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
	return ToString().ToVARIANT();
}

_bstr_t SVGUID::ToBSTR() const
{
	return ToString().ToBSTR();
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

	SVString guidStr;
	
	// Make the String Representation have Enclosing Braces and All Hex Upper case
	guidStr.Format("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		m_Guid.Data1, m_Guid.Data2, m_Guid.Data3,
		m_Guid.Data4[0], m_Guid.Data4[1], m_Guid.Data4[2], m_Guid.Data4[3],
		m_Guid.Data4[4], m_Guid.Data4[5], m_Guid.Data4[6], m_Guid.Data4[7]
		);

	return guidStr;
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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVGUID.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:21:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jun 2012 17:42:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 16:11:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:40:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:28:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

