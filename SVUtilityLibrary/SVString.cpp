//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVString
//* .File Name       : $Workfile:   SVString.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Dec 2014 18:20:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVString.h"
#include <algorithm>
#include <functional>
#include <locale>
#include <sstream>
#include <regex>

/////////////////////////////////////////////////////////////////////////////
// String formatting

#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#define DOUBLE_ARG  double

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

// provide npos same as std::basic_string
const size_t SVString::npos = SVString::SVDataType::npos;

/**
@SVOperationName Default Construction

@SVOperationDescription This constructor initializes the internal attribute to an empty default value.

@SVTest50101 Verify the size of a newly constructed string is zero.
*/
SVString::SVString()
: m_String()
{
}

/**
@SVOperationName Copy Construction

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Copy Assignment Operator to copy the data from the supplied object.

@SVTest50104 Construct a string with an other  SVString. Compare sizes to be the same.
*/
SVString::SVString( const SVString& p_rString )
: m_String( p_rString.m_String )
{
}

/**
@SVOperationName Construction from Data Type

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Data Type Operator to copy the data from the supplied object.

*/
SVString::SVString( const SVDataType& p_rString )
: m_String( p_rString )
{
}

/**
@SVOperationName Construction from Element Type and Count

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Data Type Operator to copy the data from the supplied object.

*/
SVString::SVString( SVElementType p_Char, size_type p_Count )
: m_String( p_Count, p_Char )
{
}

/**
@SVOperationName Construction from Element Type Pointer

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Element Type Pointer Operator to copy the data from the supplied object.

*/
SVString::SVString( const SVElementType* p_szString )
: m_String()
{
	if (p_szString != NULL)
		m_String = p_szString;
}

/**
@SVOperationName Construction from Element Type Pointer and String Size

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Element Type Pointer Operator to copy the data from the supplied object.

*/
SVString::SVString( const SVElementType* p_szString, size_type p_Size )
: m_String()
{
	if (p_szString != NULL && 0 < p_Size )
	{
		m_String.assign( p_szString, p_Size );
	}
}

SVString::SVString( const wchar_t* p_szString )
: m_String()
{
	_bstr_t l_String( p_szString );

	if( l_String.length() )
		m_String = l_String;
}

/**
@SVOperationName Construction from Basic String

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Element Type Pointer Operator to copy the data from the supplied object.

*/
SVString::SVString( const BSTR& p_rString )
: m_String()
{
	_bstr_t l_String( p_rString );

	if( l_String.length() )
		m_String = l_String;
}

/**
@SVOperationName Construction from Varaint

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Element Type Pointer Operator to copy the data from the supplied object.

*/
SVString::SVString( const VARIANT& p_rVariant )
: m_String()
{
	_bstr_t l_String( p_rVariant );

	if (l_String.length())
		m_String = l_String;
}

/**
@SVOperationName Construction from Variant

@SVOperationDescription This constructor initializes the internal attribute to an empty default value and executes the Assignment from Element Type Pointer Operator to copy the data from the supplied object.

*/
SVString::SVString( const _variant_t& p_rVariant )
: m_String()
{
	_bstr_t l_String( p_rVariant );

	if (l_String.length())
		m_String = l_String;
}

/**
@SVOperationName Destructor

@SVOperationDescription This destructor clears the internal storage attribute.

*/
SVString::~SVString()
{
	m_String.clear();
}

SVString::SVElementType& SVString::operator[](size_type p_Off)
{
	return m_String[ p_Off ];
}

const SVString::SVElementType& SVString::operator[](size_type p_Off) const
{
	return m_String[ p_Off ];
}

/**
@SVOperationName Copy Assignment Operator

@SVOperationDescription This operator checks the supplied object and if they are different, it copies the internal data from the supplied object to the internal data attribute.

@SVTest50106 Assign a non-empty string object to this empty object.  Verify the operation completed sucessfully by comparing the size of both object to make sure they are equal.

*/
const SVString& SVString::operator=( const SVString& p_rString )
{
	if( this != &p_rString )
	{
		m_String = p_rString.m_String;
	}

	return *this;
}

/**
@SVOperationName Append from Element Type Pointer Operator

@SVOperationDescription This operator appends the supplied data to the internal data attribute.

@SVTest5011? Append a single character object to this empty object.  Verify the operation completed sucessfully by verifing the new size equals the size of the original string object plus the size of the new appended string.

*/

const SVString& SVString::operator+=( SVElementType p_Char )
{
	m_String += p_Char;

	return *this;
}

/**
@SVOperationName Append from Element Type Pointer Operator

@SVOperationDescription This operator appends the supplied data to the internal data attribute.

@SVTest50118 Append a non-empty c-style string object to this empty object.  Verify the operation completed sucessfully by verifing the new size equals the size of the original string object plus the size of the new appended string.

*/
const SVString& SVString::operator+=( const SVElementType* p_szString )
{
	if (p_szString != NULL)
		m_String += p_szString;

	return *this;
}

const SVString& SVString::operator+=( const SVString& p_rString )
{
	m_String += p_rString.m_String;

	return *this;
}

SVString SVString::operator+( const SVElementType* p_szString ) const
{
	SVString l_Temp( *this );

	l_Temp += p_szString;

	return l_Temp;
}

SVString SVString::operator+( const SVString& p_String ) const
{
	SVString l_Temp( *this );

	l_Temp += p_String.ToString();

	return l_Temp;
}

bool SVString::operator<( const SVString& p_rString ) const
{
	return (m_String.compare(p_rString.ToString()) < 0) ? true : false;
}

bool SVString::operator<( const SVDataType& p_rString ) const
{
	return (m_String.compare(p_rString) < 0) ? true : false;
}

bool SVString::operator<( const SVElementType* p_szString ) const
{
	return (m_String.compare(p_szString) < 0) ? true : false;
}

bool SVString::operator==( const SVString& p_rString ) const
{
	return (m_String.compare(p_rString.ToString()) == 0) ? true : false;
}

bool SVString::operator==( const SVDataType& p_rString ) const
{
	return (m_String.compare(p_rString) == 0) ? true : false;
}

bool SVString::operator==( const SVElementType* p_szString ) const
{
	return (m_String.compare(p_szString) == 0) ? true : false;
}

bool SVString::operator!=( const SVString& p_rString ) const
{
	return (m_String.compare(p_rString.ToString()) != 0) ? true : false;
}

bool SVString::operator!=( const SVDataType& p_rString ) const
{
	return (m_String.compare(p_rString) != 0) ? true : false;
}

bool SVString::operator!=( const SVElementType* p_szString ) const
{
	return (m_String.compare(p_szString) != 0) ? true : false;
}

/**
@SVOperationName Conversion to Constant Element Type Pointer

@SVOperationDescription This operator returns a read-only data pointer to a standard c-style string.

@SVTest50121 Assign this object to a pointer variable to a c-style string object.  Verify the operation completed sucessfully by comparing the size of both object to make sure they are equal.

*/
const SVString::SVElementType* SVString::c_str() const
{
	return m_String.c_str();
}

/**
@SVOperationName Conversion to Constant Element Type Pointer

@SVOperationDescription This operator returns a read-only data pointer to a standard c-style string.

@SVTest50121 Assign this object to a pointer variable to a c-style string object.  Verify the operation completed sucessfully by comparing the size of both object to make sure they are equal.

*/
const SVString::SVElementType* SVString::ToString() const
{
	return m_String.c_str();
}

const SVString::SVDataType& SVString::ToDataType() const
{
	return m_String;
}

_bstr_t SVString::ToBSTR() const
{
	return _bstr_t( m_String.c_str() );
}

_variant_t SVString::ToVARIANT() const
{
	return _variant_t( m_String.c_str() );
}

bool SVString::IsNumber() const
{
	size_type l_Size( size() );
	bool l_Status( 0 < l_Size );

	for( size_type i = 0; l_Status && i < l_Size; ++i )
	{
		l_Status = ( isdigit( m_String[ i ] ) != FALSE );

		if( i == 0 && ! l_Status )
		{
			l_Status = m_String[ i ] == '-';
		}
	}

	return l_Status;
}

/**
@SVOperationName Clear

@SVOperationDescription This operator clears the internal data attribute.

@SVTest50103 This test verifies that a non-empty object that is cleared is now empty.  This test is verified by checking if the Empty operator returns true after the clear.

*/	
void SVString::clear()
{
	m_String.clear();
}

/**
@SVOperationName Empty

@SVOperationDescription This operator return the empty state of the internal data attribute.

@SVTest50122 This test verifies that a default constructed object is empty.  This test verifys that the Empty operator is correctly returning a true value for a default costructed object.
@SVTest50108 This test verifies that a parameterized constructed object is not empty.  This test verifys that the Empty operator is correctly returning a false value for a parameterized costructed object.

*/
bool SVString::empty() const
{
	return m_String.empty();
}

/**
@SVOperationName Size

@SVOperationDescription This operator returns the size of the internal data attribute.

@SVTest50123 This test verifies that the size of a default constructed object is equal to zero.  This test verifys that the Size is correctly returning a zero value for a default costructed string object.
@SVTest50124 This test verifies that the size of a parameterized constructor is not equal to zero.  This test verifys that the Size is correctly returning a non-zero value for a parameterized costructed string object.
@SVTest50125 This test verifies that the size of a copy construction of a non-empty string is equal to the size of the non-empty string.

*/
SVString::size_type SVString::size() const
{
	return m_String.size();
}

void SVString::resize( size_type p_Count, SVElementType p_Ch )
{
	m_String.resize( p_Count, p_Ch );
}

/**
@SVOperationName Begin

@SVOperationDescription This operator return an iterator to the first element in the internal data attribute.

@SVTest50109 This test checks the returned iterator to verify that it points to the first element in the string.
@SVTest50110 Checks the iterators operation.
@SVTest50111 Checks the iterators operation.

*/
SVString::const_iterator SVString::begin() const
{
	return m_String.begin();
}

/**
@SVOperationName End

@SVOperationDescription This operator return an iterator to the next element after the last elements in the internal data attribute.

@SVTest50112 This test checks the returned iterator against the iterator that has incremented the size of the string times.  The two iterators must be the same.

*/
SVString::const_iterator SVString::end() const
{
	return m_String.end();
}

/**
@SVOperationName Data Type Append

@SVOperationDescription This operator appends the supplied object to the end of the internal data attribute.

@SVTest50105 Append a non-empty c-style string object to this empty object.  Verify the operation completed sucessfully by verifing the new size equals the size of the original string object plus the size of the new appended string.

*/
const SVString& SVString::append( const SVElementType* p_szString )
{
	if (p_szString != NULL)
		m_String.append( p_szString );

	return *this;
}

/**
@SVOperationName Assign

@SVOperationDescription This operator assigns the supplied object to the end of the internal data attribute.

@SVTest50102 Assign a non-empty c-style string object to this empty object.  Verify the operation completed sucessfully by comparing the size of both object to make sure they are equal.

*/
const SVString& SVString::assign( const SVElementType* p_szString )
{
	if (p_szString != NULL)
		m_String.assign( p_szString );
	else
		m_String.clear();

	return *this;
}

/**
@SVOperationName Assign to Size

@SVOperationDescription This operator assigns the supplied object to the end of the internal data attribute.

@SVTest50102 Assign a non-empty c-style string object to this empty object.  Verify the operation completed sucessfully by comparing the size of both object to make sure they are equal.

*/
const SVString& SVString::assign( const SVElementType* p_szString, size_type p_Size )
{
	if (p_szString != NULL && 0 < p_Size )
		m_String.assign( p_szString, p_Size );
	else
		m_String.clear();

	return *this;
}

/**
@SVOperationName Erase

@SVOperationDescription

*/
SVString& SVString::erase( size_type p_Pos, size_type p_Count )
{
	m_String.erase( p_Pos, p_Count );

	return *this;
}

/**
@SVOperationName Sub-String

@SVOperationDescription This operator returns an object that contains the desired sub-string of the string based on the supplied parameters.

@SVTest50107 - Test of substr. Create a new SVString with the default constructor.  Using another SVString that has a size of at least 7, extract 3 char's from the sting and then check the size of the new string to make sure that the size is 3.

*/
SVString SVString::substr( size_type p_Offset, size_type p_Count ) const
{
	return m_String.substr( p_Offset, p_Count );
}

SVString& SVString::insert( size_type p_Offset, SVElementType p_Char )
{
	SVDataType tmp(1, p_Char);
	m_String.insert( p_Offset, tmp );
	return *this;
}

SVString& SVString::insert( size_type p_Offset, const SVElementType* p_szString )
{
	m_String.insert( p_Offset, p_szString );
	return *this;
}

/**
@SVOperationName Find First Of Element Type and Offset

@SVOperationDescription This operator returns the desired location in the string based on the provided parameters.

@SVTest50113 Start with a known string.  Use find_first_of with a string that is present in the beginning of the string. verify that it returns the position of the first charactor found in this string.
@SVTest50114 Start with a known string.  Use find_first_of with a string that is present at the end of the string. verify that it returns the position of the first charactor found in this string.
*/
SVString::size_type SVString::find_first_of( SVElementType p_Char, size_type p_Offset ) const
{
	return m_String.find_first_of( p_Char, p_Offset );
}

/**
@SVOperationName Find First Of Element Type Pointer and Offset

@SVOperationDescription This operator returns the desired location in the string based on the provided parameters.

@SVTest50115 Start with a known string.  Use find_first_of with a string that is present in string. verify that it returns the position of the first charactor found in this string.
*/
SVString::size_type SVString::find_first_of( const SVElementType* p_szString, size_type p_Offset ) const
{
	size_type pos = SVDataType::npos;
	if (p_szString != NULL)
		pos = m_String.find_first_of( p_szString, p_Offset );
	return pos; 
}

SVString::size_type SVString::find_first_not_of( SVElementType p_Char, size_type p_Offset ) const
{
	return m_String.find_first_not_of( p_Char, p_Offset );
}

SVString::size_type SVString::find_first_not_of( const SVElementType* p_szString, size_type p_Offset ) const
{
	size_type pos = SVDataType::npos;
	if (p_szString != NULL)
		pos = m_String.find_first_not_of( p_szString, p_Offset );
	return pos; 
}

SVString::size_type SVString::find_last_of( SVElementType p_Char, size_type p_Offset ) const
{
	return m_String.find_last_of( p_Char, p_Offset );
}

SVString::size_type SVString::find_last_of( const SVElementType* p_szString, size_type p_Offset ) const
{
	size_type pos = SVDataType::npos;
	if (p_szString != NULL)
		pos = m_String.find_last_of( p_szString, p_Offset );
	return pos; 
}

SVString::size_type SVString::find_last_not_of( SVElementType p_Char, size_type p_Offset ) const
{
	return m_String.find_last_not_of( p_Char, p_Offset );
}

SVString::size_type SVString::find_last_not_of( const SVElementType* p_szString, size_type p_Offset ) const
{
	size_type pos = SVDataType::npos;
	if (p_szString != NULL)
		pos = m_String.find_last_not_of( p_szString, p_Offset );
	return pos; 
}

SVString::size_type SVString::find( SVElementType p_Char, size_type p_Offset) const
{
	return m_String.find(p_Char, p_Offset);
}

SVString::size_type SVString::find( const SVElementType* p_szString, size_type p_Offset) const
{
	size_type pos = SVDataType::npos;
	if (p_szString != NULL)
		pos = m_String.find(p_szString, p_Offset);
	return pos;
}

SVString::size_type SVString::rfind( SVElementType p_Char, size_type p_Offset) const
{
	return m_String.rfind(p_Char, p_Offset);
}

SVString::size_type SVString::rfind( const SVElementType* p_szString, size_type p_Offset) const
{
	size_type pos = SVDataType::npos;
	if (p_szString != NULL)
		pos = m_String.rfind(p_szString, p_Offset);
	return pos;
}

SVString& SVString::replace( SVElementType p_FromChar, SVElementType p_ToChar )
{
	std::replace_if( m_String.begin(), m_String.end(), std::bind1st(std::equal_to< SVElementType >(), p_FromChar),  p_ToChar );
	return *this;
}

SVString& SVString::replace( const SVElementType* p_FromStr, const SVElementType* p_ToStr )
{
	if (p_FromStr != NULL && p_ToStr != NULL)
	{
		size_t pos = m_String.find(p_FromStr);

		while( pos != SVDataType::npos )
		{
			m_String.replace(pos, strlen(p_FromStr), p_ToStr);

			pos = m_String.find( p_FromStr, pos + _tcslen( p_ToStr ) );
		}
	}

	return *this;
}

SVString::reference SVString::at( size_type p_Offset )
{
	return m_String.at( p_Offset );
}

SVString::const_reference SVString::at( size_type p_Offset ) const
{
	return m_String.at( p_Offset );
}

void SVString::reserve( size_type count )
{
	m_String.reserve(count);
}

/* Need to use SVResourceManager singleton
bool SVString::LoadString( UINT stringID )
{
	return LoadString( NULL, stringID );
}

bool SVString::LoadString( HINSTANCE hInst, UINT stringID )
{
	// try fixed buffer first (to avoid wasting space in the heap)
	TCHAR szTemp[256];
	int nLen = ::LoadString(hInst, stringID, szTemp, _countof(szTemp));
	if (_countof(szTemp) - nLen > sizeof(TCHAR))
	{
		m_String = szTemp;
		return nLen > 0;
	}

	// try buffer size of 512, then larger size until entire string is retrieved
	int nSize = 256;
	do
	{
		nSize += 256;
		m_String.resize(nSize);
		nLen = ::LoadString(hInst, stringID, (LPTSTR)(m_String.data()), nSize);
	} while (nSize - nLen <= sizeof(TCHAR));
	
	return nLen > 0;
}
*/

// formatting (using wsprintf style formatting)
void SVString::Format( const SVElementType* lpszFormat, ...)
{
	assert(lpszFormat);

	if (lpszFormat != NULL)
	{
		va_list argList;
		va_start(argList, lpszFormat);

		size_t nMaxLen = _vsctprintf( lpszFormat, argList ) + 1;

		SVElementType* pData = new SVElementType[nMaxLen];
		vsprintf(pData, lpszFormat, argList);
		m_String = pData;
		delete [] pData;
			
		va_end(argList);
	}
}

// formatting (using wsprintf style formatting)
void SVString::Format( const wchar_t* lpszFormat, ...)
{
	assert(lpszFormat);

	if (lpszFormat != NULL)
	{
		va_list argList;
		va_start(argList, lpszFormat);

		size_t nMaxLen = _vscwprintf( lpszFormat, argList ) + 1;

		wchar_t* pData = new wchar_t[nMaxLen];
		vswprintf(pData, lpszFormat, argList);
		m_String = _bstr_t( pData );
		delete [] pData;
			
		va_end(argList);
	}
}

template<typename T>
bool SVString::Convert2Number(T& Value, bool failIfLeftoverChars)
{
	SVString text = *this;
	text.Trim();
	std::istringstream stream(text.c_str());
	char c;
	if (!(stream >> Value) || (failIfLeftoverChars && stream.get(c)))
	{
		return false;
	}
	return true;
}

int SVString::Compare(const SVString & p_str) const
{
	return m_String.compare(p_str.m_String);
}

namespace detail
{
	struct to_l
	{
		void operator()(SVString::SVElementType& p_ch) const
		{
			p_ch = std::tolower(p_ch, SVString::current_locale());
		}
	};
	struct to_u
	{
		void operator()(SVString::SVElementType & p_ch) const
		{
			p_ch = std::toupper(p_ch, SVString::current_locale());
		}
	};
} // namespace detail

int SVString::CompareNoCase(const SVString & p_str) const
{
	SVString i_left(*this), i_right(p_str);
	return i_left.MakeLower().Compare(i_right.MakeLower());
}

SVString& SVString::MakeLower()
{
	std::for_each(m_String.begin(), m_String.end(), detail::to_l());
	return *this;
}

SVString& SVString::MakeUpper()
{
	std::for_each(m_String.begin(), m_String.end(), detail::to_u());
	return *this;
}

SVString& SVString::Trim()
{
	static const SVString match(_T(" \t\n"));
	return Trim(match);
}

SVString& SVString::Trim(SVElementType p_ch)
{
	SVDataType match(1, p_ch);
	return Trim(match);
}

SVString& SVString::Trim(const SVString & p_str)
{
	return TrimLeft(p_str).TrimRight(p_str);
}

SVString& SVString::TrimLeft()
{
	static const SVDataType match(_T(" \t\n"));
	return TrimLeft(match);
}

SVString& SVString::TrimLeft(SVElementType p_ch)
{
	SVDataType match(1, p_ch);
	return TrimLeft(match);
}

SVString& SVString::TrimLeft(const SVString & p_str)
{
	m_String.erase(0, m_String.find_first_not_of(p_str.ToString()));
	return *this;
}

SVString& SVString::TrimRight()
{
	static const SVDataType match(_T(" \t\n"));
	return TrimRight(match);
}

SVString& SVString::TrimRight(SVElementType p_ch)
{
	SVDataType match(1, p_ch);
	return TrimRight(match);
}

SVString& SVString::TrimRight(const SVString& p_str)
{
	m_String.erase(m_String.find_last_not_of(p_str.ToString()) + 1);
	return *this;
}

SVString::SVElementType SVString::GetAt(size_t index) const
{
	return m_String.at(index);
}

void SVString::SetAt(size_t index, SVElementType ch)
{
	m_String.replace(index, 1, &ch);
}

SVString SVString::Left( size_t count ) const
{
	return m_String.substr(0, count);
}

SVString SVString::Mid( size_t index ) const
{
	SVString l_String;

	if( index < m_String.size() )
	{
		l_String = Mid(index, m_String.size() - index);
	}

	return l_String;
}

SVString SVString::Mid( size_t index , size_t count ) const
{
	return m_String.substr(index, count);
}

SVString SVString::Right( size_t count ) const
{
	if (m_String.size() && m_String.size() >= count)
		return m_String.substr(m_String.size() - count, count);

	return SVString();
}

void SVString::Remove(SVElementType ch)
{
	m_String.erase(std::remove ( m_String.begin(), m_String.end(), ch ), m_String.end());
}

bool SVString::isSubmatch(SVString searchString, size_t offsetSource, size_t offsetSearch )
{
	bool retVal = false;
	size_t wildcardPos = searchString.find_first_of('*', offsetSearch);
	if (SVString::npos == wildcardPos)
	{
		retVal = (SVString::npos != m_String.find(searchString.c_str()+offsetSearch, offsetSource));
	}
	else
	{
		SVString firstSearchString = searchString.substr(offsetSearch, wildcardPos-offsetSearch);
		size_t firstPos = m_String.find(firstSearchString.c_str(), offsetSource);
		if (SVString::npos != firstPos)
		{
			retVal = isSubmatch(searchString, firstPos + firstSearchString.size(), wildcardPos+1);
		}
	}
	return retVal;
}

bool SVString::matchesRegularExpression( LPCTSTR RegularExpression ) const
{
	bool Result( false );

	Result = std::regex_match( m_String.cbegin(), m_String.cend(), std::regex( RegularExpression ) );

	return Result;
}

SVString operator+( LPCTSTR psz1, const SVString& str2 )
{
	SVString l_Temp( psz1 );

	l_Temp += str2.ToString();

	return l_Temp;
}

bool operator==( LPCTSTR psz1, const SVString& str2 )
{
	SVString l_Temp( psz1 );

	return l_Temp == str2;
}

bool operator!=( LPCTSTR psz1, const SVString& str2 )
{
	SVString l_Temp( psz1 );

	return l_Temp == str2;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVString.cpp_v  $
 * 
 *    Rev 1.1   16 Dec 2014 18:20:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method isSubmatch to support the Object Selector's Filter Page search.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:24:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Feb 2013 12:43:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVString with a nw method to get a const reference to the internal data type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Sep 2012 13:49:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  787
 * SCR Title:  Fix problem when Discrete I/O does not change when forced attribute is set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Aug 2012 17:26:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update comment on method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Aug 2012 16:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated string class to fix find methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Jun 2012 17:42:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Feb 2011 09:52:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2010 08:22:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2009 09:40:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jun 2009 07:46:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ToString method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
