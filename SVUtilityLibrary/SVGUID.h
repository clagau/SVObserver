//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace std
{
template<> struct hash<GUID>
{
	std::size_t operator()(GUID const& s) const noexcept
	{
		return std::hash<unsigned long> {}(s.Data1);
	}
};
}

inline bool operator < (const GUID& guid1, const GUID& guid2)
{
	RPC_STATUS l_Status;
	return ::UuidCompare(const_cast<GUID*>(&guid1), const_cast<GUID*>(&guid2), &l_Status) < 0;
}

inline bool operator > (const GUID& guid1, const GUID& guid2)
{
	RPC_STATUS l_Status;
	return ::UuidCompare(const_cast<GUID*>(&guid1), const_cast<GUID*>(&guid2), &l_Status) > 0;
}

class SVGUID
{
public:
	SVGUID();
	SVGUID( const SVGUID& rObject );
	explicit SVGUID(const GUID& rGuid);
	explicit SVGUID(const _bstr_t& rString);
	explicit SVGUID(const std::string& rString);
	explicit SVGUID( const _variant_t& rVariant );

	virtual ~SVGUID();

	bool empty() const;

	void clear();

	operator const GUID& () const;
	operator GUID& ();

	GUID* operator&();
	
	std::string ToString() const;

	SVGUID& operator=(const SVGUID& rGuid);
	SVGUID& operator=(const GUID& rGuid);
	SVGUID& operator=(const _bstr_t& rString);
	SVGUID& operator=(const std::string& rString);

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
