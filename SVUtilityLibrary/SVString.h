//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVString
//* .File Name       : $Workfile:   SVString.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Dec 2014 18:20:04  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <string>
#include <tchar.h>
#include <comdef.h>
#include <vector>
#include <map>
#include <set>
#pragma endregion Includes


/**
@SVObjectName String

@SVObjectOverview This object encapsulates the storage and operations for dealing with strings.  Strings are a complex type like images.  They can either be an array of character or a string.

@SVObjectOperations This object supports the copy, assignment, and conversions to and from block array of characters and Standard Template Library (STL) wide character strings.

*/
class SVString
{
public:
	typedef std::string SVDataType;
	typedef SVDataType::traits_type::_Elem SVElementType;
	typedef SVDataType::size_type size_type;
	typedef SVDataType::iterator iterator;
	typedef SVDataType::const_iterator const_iterator;
	typedef SVDataType::reference reference;
	typedef SVDataType::const_reference const_reference;
	typedef SVDataType::value_type value_type;

public:
	SVString();
	SVString( const SVString& p_rString );
	SVString( const SVDataType& p_rString );
	SVString( SVElementType p_Char, size_type p_Count );
	SVString( const SVElementType* p_szString );
	SVString( const SVElementType* p_szString, size_type p_Size );
	SVString( const wchar_t* p_szString );
	SVString( const BSTR& p_rString );
	SVString( const VARIANT& p_rVariant );
	SVString( const _variant_t& p_rVariant );

	template< typename SVStringType >
	SVString( const SVStringType& p_rString )
	: m_String() { m_String = static_cast< const SVElementType* >( p_rString ); }
	
	virtual ~SVString();

	SVElementType& operator[](size_type p_Off);
	const SVElementType& operator[](size_type p_Off) const;

	const SVString& operator=( const SVString& p_rString );

	const SVString& operator+=( SVElementType p_Char );
	const SVString& operator+=( const SVElementType* p_szString );
	const SVString& operator+=( const SVString& p_rString );

	SVString operator+( const SVElementType* p_szString ) const;
	SVString operator+( const SVString& p_szString ) const;

	bool operator<( const SVString& p_rString ) const;
	bool operator<( const SVDataType& p_rString ) const;
	bool operator<( const SVElementType* p_szString ) const;

	bool operator==( const SVString& p_rString ) const;
	bool operator==( const SVDataType& p_rString ) const;
	bool operator==( const SVElementType* p_szString ) const;

	bool operator!=( const SVString& p_rString ) const;
	bool operator!=( const SVDataType& p_rString ) const;
	bool operator!=( const SVElementType* p_szString ) const;

	const SVElementType* c_str() const;
	const SVElementType* ToString() const;
	const SVDataType& ToDataType() const;
	_bstr_t ToBSTR() const;
	_variant_t ToVARIANT() const;

	bool IsNumber() const;

	int Compare(const SVString & str) const;
	int CompareNoCase(const SVString & str) const;

	SVString& MakeLower();
	SVString& MakeUpper();

	SVString& Trim();
	SVString& Trim(SVElementType ch);
	SVString& Trim(const SVString & str);
	SVString& TrimLeft();
	SVString& TrimLeft(SVElementType ch);
	SVString& TrimLeft(const SVString & str);
	SVString& TrimRight();
	SVString& TrimRight(SVElementType ch);
	SVString& TrimRight(const SVString & str);

	SVElementType GetAt(size_t index) const;
	void SetAt(size_t index, SVElementType ch);

	SVString Left( size_t count ) const;
	SVString Mid( size_t index ) const;
	SVString Mid( size_t index , size_t count ) const;
	SVString Right( size_t count ) const;
	void Remove(SVElementType ch);

	//************************************
	//! Method to remove a list of characters
	//! \param ExcludeCharacters [in] the characters to remove
	//************************************
	void RemoveCharacters(LPCTSTR ExcludeCharacters);

	//************************************
	// Description:  Check if searchString is found in this string. For the searchString wildcard "*" can be used.
	// Parameter:  searchString <in>:  the string to look for
	// Parameter:  offsetSource <in>:  offset where the search in this string will started.  Default is 0 (the beginning).
	// Parameter:  offsetSearch <in>:  offset where from the search string is used.  Default is 0 (the beginning)
	// Returns:  bool:  true if searchString is found
	//************************************
	bool isSubmatch( SVString searchString, size_t offsetSource = 0, size_t offsetSearch = 0 );

	//************************************
	//! Method to check if the string matches a regular expression
	//! \param RegularExpression [in] the regular expression to check with
	//! \returns true if the internal string matches the regular expression
	//************************************
	bool matchesRegularExpression( LPCTSTR RegularExpression ) const;

	void clear();

	bool empty() const;

	size_type size() const;
	void resize( size_type p_Count, SVElementType p_Ch = SVElementType() );
	void reserve( size_type count );

	const_iterator begin() const;
	const_iterator end() const;

	const SVString& append( const SVElementType* p_szString );

	const SVString& assign( const SVElementType* p_szString );
	const SVString& assign( const SVElementType* p_szString, size_type p_Size );

	SVString& erase( size_type p_Pos = 0, size_type p_Count = SVDataType::npos );

	SVString substr( size_type p_Offset = 0, size_type p_Count = SVDataType::npos ) const;

	size_type find_first_of( SVElementType p_Char, size_type p_Offset = 0 ) const;
	size_type find_first_of( const SVElementType* p_szString, size_type p_Offset = 0 ) const;

	size_type find_first_not_of( SVElementType p_Char, size_type p_Offset = 0 ) const;
	size_type find_first_not_of( const SVElementType* p_szString, size_type p_Offset = 0 ) const;

	size_type find_last_of( SVElementType p_Char, size_type p_Offset = SVDataType::npos ) const;
	size_type find_last_of( const SVElementType* p_szString, size_type p_Offset = SVDataType::npos ) const;

	size_type find_last_not_of( SVElementType p_Char, size_type p_Offset = SVDataType::npos ) const;
	size_type find_last_not_of( const SVElementType* p_szString, size_type p_Offset = SVDataType::npos ) const;

	size_type find( SVElementType p_Char, size_type p_Offset = 0 ) const;
	size_type find( const SVElementType* p_szString, size_type p_Offset = 0 ) const;

	size_type rfind( SVElementType p_Char, size_type p_Offset = SVDataType::npos ) const;
	size_type rfind( const SVElementType* p_szString, size_type p_Offset = SVDataType::npos ) const;

	SVString& insert( size_type p_Offset, SVElementType p_Char );
	SVString& insert( size_type p_Offset, const SVElementType* p_szString );

	SVString& replace( SVElementType p_FromChar, SVElementType p_ToChar );
	SVString& replace( const SVElementType* p_FromStr, const SVElementType* p_ToStr );
	
	reference at( size_type p_Offset );
	const_reference at( size_type p_Offset ) const;

	void Format( const SVElementType* lpszFormat, ... );
	void Format( const wchar_t* lpszFormat, ... );

	//************************************
	/// Check if string can convert to a number and return this if possible.
	/// \param Value [out] The converted number.
	/// \param failIfLeftoverChars [in] If value == true, it retrun also false if after convert unused character left (expect of spaces), If value == false, it return only false if convert failed.
	/// \returns bool Return if convert was succeeded.
	//************************************
	template<typename T>
	bool Convert2Number(T& Value, bool failIfLeftoverChars = false);

	static std::locale & current_locale()
	{
		static std::locale loc("");
		return loc;
	}
	static const SVString & EmptyString()
	{
		static SVString emptyString(_T(""));
		return emptyString;
	}
	// provide npos same as std::string
	static const size_t npos;

protected:
	SVDataType m_String;
};

typedef std::vector< SVString > SVStringArray;
typedef std::set< SVString > SVStringSet;
typedef std::map< SVString, SVString > TranslateMap;

template bool SVString::Convert2Number(double&, bool);
template bool SVString::Convert2Number(long&, bool);

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVString.h_v  $
 * 
 *    Rev 1.3   16 Dec 2014 18:20:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method isSubmatch to support the Object Selector's Filter Page search.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2014 17:17:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Typedef definitions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Oct 2013 15:46:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added SVStringArray.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:25:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   11 Feb 2013 12:43:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVString with a nw method to get a const reference to the internal data type.
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
 *    Rev 1.5   10 Jul 2012 12:10:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added missing value typedef.
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
 *    Rev 1.2   30 Jul 2009 09:41:00   jspila
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
 *    Rev 1.0   10 Sep 2008 10:29:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
