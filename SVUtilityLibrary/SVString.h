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
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <tchar.h>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

typedef std::basic_string<TCHAR> SVString;
typedef std::pair<SVString, SVString> SVStringPair;
typedef std::set<SVString> SVStringSet;
typedef std::vector<SVString> SVStringVector;
typedef std::vector<SVStringPair> StringPairVector;
typedef std::map<SVString, SVString> TranslateMap;

namespace Seidenader { namespace SVUtilityLibrary { namespace StringFunctions {

	//************************************
	/// Function to create a SVString from other objects
	/// \param pString, rString or rVariant [in] The source object
	/// \returns SVString The created object.
	//************************************
	SVString createSVString( const wchar_t* pString );
	SVString createSVString( const _bstr_t& rString );
	SVString createSVString( const _variant_t& rVariant );

	//************************************
	/// Compare to strings like the std::string.compare function, but ignore upper and lower cases.
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
	/// Copy a number of character from the Left of the string
	/// \param rStringValue [in] The source string, will not be changed.
	/// \param count [in] Count of character to copy. If count > size, it returns an empty String.
	/// \returns SVString The copied string.
	//************************************
	SVString Left( const SVString& rStringValue, size_t count );

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
	/// \param rStringValue [in][out] The source string can be changed
	//! \param ExcludeCharacters [in] the characters to remove
	//************************************
	void RemoveCharacters(SVString& rStringValue, LPCTSTR ExcludeCharacters);

	//************************************
	//! Method to validate the string with a list of characters
	/// \param rStringValue [in] The source string, will not be changed.
	//! \param ValidCharacters [in] the characters which are valid
	//************************************
	SVString ValidateString(const SVString& rStringValue, LPCTSTR ValidCharacters);

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

	//************************************
	/// Load the string from the resource dll
	/// \param ResourceID [in] The resource ID to load from the resource dll
	/// \returns SVString Created string with the loaded string
	//************************************
	SVString LoadSVString( UINT ResourceID );
} /* namespace StringFunctions */ } /* namespace SVUtilityLibrary */ } /* namespace Seidenader */

namespace SvUl = Seidenader::SVUtilityLibrary;
namespace SvUl_SF = Seidenader::SVUtilityLibrary::StringFunctions;
