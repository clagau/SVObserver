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
#include <vector>
#include <map>
#include <set>
#include <comutil.h>
#include <tchar.h>
#pragma endregion Includes


/**
@SVObjectName String

@SVObjectOverview This object encapsulates the storage and operations for dealing with strings.  Strings are a complex type like images.  They can either be an array of character or a string.

@SVObjectOperations This object supports the copy, assignment, and conversions to and from block array of characters and Standard Template Library (STL) wide character strings.

*/

typedef std::string SVString;
typedef std::set< SVString > SVStringSet;
typedef std::vector< SVString > SVStringArray;
typedef std::map< SVString, SVString > TranslateMap;

namespace Seidenader { namespace SVUtilityLibrary { namespace StringFunctions {

	//************************************
	/// Function to create a SVString from other objects
	/// \param pString, rString or rVariant [in] The source object
	/// \returns SVString The created object.
	//************************************
	SVString createSVString( const wchar_t* pString );
	SVString createSVString( const _bstr_t& rString );
	SVString createSVString( const VARIANT& rVariant );

	//************************************
	/// Compare to strings like the std::string.comapre function, but ignore upper and lower cases.
	/// \param rStringValue [in]
	/// \param rStringValue2 [in]
	/// \returns int 0 = strings are similar
	//************************************
	int CompareNoCase(const SVString& rStringValue, const SVString & rStringValue2);

	//************************************
	/// Make all character to lower case.
	/// \param rStringValue [in,out] The source string to be changed.
	/// \returns SVString& Reference to rStringValue
	//************************************
	SVString& MakeLower(SVString& rStringValue);
	//************************************
	/// Make all character to upper case.
	/// \param rStringValue [in,out] The source string to be changed.
	/// \returns SVString& Reference to rStringValue
	//************************************
	SVString& MakeUpper(SVString& rStringValue);

	//************************************
	/// Trim defined character until another character is found from the left (TrimLeft), right (TrimRight) or both (Trim)
	/// \param rStringValue [in,out] The source string to be changed.
	/// \param pTrimChar [in,out] Character to trim.
	/// \returns SVString& Reference to rStringValue
	//************************************
	SVString& Trim(SVString& rStringValue, LPCTSTR pTrimChar = _T(" \t\n"));
	SVString& TrimLeft(SVString& rStringValue, LPCTSTR pTrimChar = _T(" \t\n"));
	SVString& TrimRight(SVString& rStringValue, LPCTSTR pTrimChar = _T(" \t\n"));

	//************************************
	/// Copy a number of character from the mid of the string
	/// \param rStringValue [in] The source string, will not be changed.
	/// \param count [in] Count of character to copy. If count > size, it return a copy of rStringValue.
	/// \returns SVString The copied string.
	//************************************
	SVString Mid( const SVString& rStringValue, size_t count );
	//************************************
	/// Copy a number of character from the right part of the string.
	/// \param rStringValue [in] The source string, will not be changed.
	/// \param count [in] Count of character to copy.
	/// \returns SVString The copied string.
	//************************************
	SVString Right( const SVString& rStringValue, size_t count );

	//************************************
	//! Method to remove a list of characters
	//! \param ExcludeCharacters [in] the characters to remove
	//************************************
	void RemoveCharacters(SVString& rStringValue, LPCTSTR ExcludeCharacters);

	//************************************
	// Description:  Check if searchString is found in this string. For the searchString wildcard "*" can be used.
	// Parameter:  searchString <in>:  the string to look for
	// Parameter:  offsetSource <in>:  offset where the search in this string will started.  Default is 0 (the beginning).
	// Parameter:  offsetSearch <in>:  offset where from the search string is used.  Default is 0 (the beginning)
	// Returns:  bool:  true if searchString is found
	//************************************
	bool isSubmatch(const SVString& rStringValue, const SVString& rSearchString, size_t offsetSource = 0, size_t offsetSearch = 0 );

	//************************************
	/// Search in the string an substring and replace all of them with the new substring.
	/// \param rStringValue [in,out] The source string to be changed.
	/// \param pFromStr [in] search substring
	/// \param pToStr [in] replace substring
	/// \returns SVString& Reference to rStringValue
	//************************************
	SVString& searchAndReplace( SVString& rStringValue, const SVString::traits_type::_Elem* pFromStr, const SVString::traits_type::_Elem* pToStr );

	//************************************
	/// Converts the value of objects to strings based on the formats specified and inserts them into another string. 
	/// \param pFormat [in] Format 
	/// \param  [in] A list of objects
	/// \returns SVString New created object
	//************************************
	SVString Format( LPCTSTR pFormat, ... );
	SVString Format( const wchar_t* pFormat, ... );

	//************************************
	/// Check if string can convert to a number and return this if possible.
	/// \param Value [out] The converted number.
	/// \param failIfLeftoverChars [in] If value == true, it retrun also false if after convert unused character left (expect of spaces), If value == false, it return only false if convert failed.
	/// \returns bool Return if convert was succeeded.
	//************************************
	template<typename T>
	bool Convert2Number(const SVString& rStringValue, T& Value, bool failIfLeftoverChars = false);

	template bool Convert2Number(const SVString& rStringValue, double&, bool);
	template bool Convert2Number(const SVString& rStringValue, long&, bool);
}}}

namespace SvUl = Seidenader::SVUtilityLibrary;
namespace SvUl_SF = Seidenader::SVUtilityLibrary::StringFunctions;

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
