//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVString
//* .File Name       : $Workfile:   SVString.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:25:14  $
//******************************************************************************

#ifndef SVSTRING_H
#define SVSTRING_H

#include <string>
#include <tchar.h>
#include <comdef.h>

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

	/* Need to use SVResourceManager singleton
	bool LoadString( UINT uID );
	bool LoadString( HINSTANCE hInst, UINT uID );
	*/

	void Format( const SVElementType* lpszFormat, ... );
	void Format( const wchar_t* lpszFormat, ... );

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

SVString operator+( LPCTSTR psz1, const SVString& str2 );
bool operator==( LPCTSTR psz1, const SVString& str2 );
bool operator!=( LPCTSTR psz1, const SVString& str2 );

#endif // SVSTRING_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVString.h_v  $
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
