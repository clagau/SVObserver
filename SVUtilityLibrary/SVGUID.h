//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGUID
//* .File Name       : $Workfile:   SVGUID.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:26  $
//******************************************************************************

#ifndef SVGUID_H
#define SVGUID_H

#include <comdef.h>

#include "SVString.h"
#include "SVUtilityGlobals.h"

extern const GUID  SVInvalidGUID;

/**
@SVObjectName Global Unique Identifier (GUID)

@SVObjectOverview

@SVObjectOperations

*/
class SVGUID
{
public:
	SVGUID();
	SVGUID( const SVGUID& p_rObject );
	SVGUID( const GUID& p_rGuid );
	SVGUID( const BSTR& p_rString );
	SVGUID( const VARIANT& p_rVariant );

	virtual ~SVGUID();

	bool empty() const;

	void clear();

	operator const GUID& () const;
	operator GUID& ();

	GUID* operator&();
	
	const GUID& ToGUID() const;
	GUID& ToGUID();

	_variant_t ToVARIANT() const;
	_bstr_t ToBSTR() const;
	SVString ToString() const;

	const SVGUID& operator=( const SVGUID& p_rObject );
	const SVGUID& operator=( const GUID& p_rGuid );
	const SVGUID& operator=( const BSTR& p_rString );
	const SVGUID& operator=( const VARIANT& p_rVariant );

	bool operator==( const SVGUID& p_rObject ) const;
	bool operator==( const GUID& p_rGuid ) const;

	bool operator<( const SVGUID& p_rObject ) const;
	bool operator<( const GUID& p_rGuid ) const;

	bool operator>( const SVGUID& p_rObject ) const;
	bool operator>( const GUID& p_rGuid ) const;

protected:
	GUID m_Guid;

};

inline bool operator==( const GUID& p_rLeft, const SVGUID& p_rRight )
{
	return ( p_rRight == p_rLeft );
}

inline bool operator<( const GUID& p_rLeft, const SVGUID& p_rRight )
{
	return ( p_rRight > p_rLeft );
}

inline bool operator>( const GUID& p_rLeft, const SVGUID& p_rRight )
{
	return ( p_rRight < p_rLeft );
}

#endif // #ifndef SVGUID_H

