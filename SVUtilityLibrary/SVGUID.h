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
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

inline bool operator < (const GUID& guid1, const GUID& guid2)
{
	RPC_STATUS l_Status;
	return ::UuidCompare(const_cast<GUID*>(&guid1), const_cast<GUID*>(&guid2), &l_Status) < 0;
}

class SVGUID
{
public:
	SVGUID();
	SVGUID( const SVGUID& rObject );
	SVGUID( const GUID& rGuid );
	SVGUID(const _bstr_t& rString);
	SVGUID(const std::string& rString);
	SVGUID( const _variant_t& rVariant );

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
	std::string ToString() const;

	const SVGUID& operator=(const SVGUID& rGuid);
	const SVGUID& operator=(const GUID& rGuid);
	const SVGUID& operator=(const _bstr_t& rString);
	const SVGUID& operator=(const std::string& rString);

	bool operator==(const SVGUID& rGuid ) const;
	bool operator==(const GUID& rGuid ) const;

	bool operator<(const SVGUID& rGuid) const;
	bool operator<(const GUID& rGuid) const;

	bool operator>(const SVGUID& rGuid) const;
	bool operator>(const GUID& rGuid) const;

protected:
	GUID m_Guid;

};

//This needs to be placed the wrong way due to recursive calling of the method
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

typedef std::set<SVGUID> SVGuidSet;
typedef std::vector<SVGUID> SVGuidVector;