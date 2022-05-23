//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file StringHelper.h
//*****************************************************************************
// Helper functions for std::string
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <tchar.h>
#pragma endregion Includes

namespace SvUl
{

	//************************************
	/// Function to create a std::string from other objects
	/// \param pString, rString or rVariant [in] The source object
	/// \returns std::string The created object.
	//************************************
	std::string createStdString( const wchar_t* pString );
	std::string createStdString( const _bstr_t& rString );
	std::string createStdString( const _variant_t& rVariant );

	//************************************
	/// Compare to strings like the std::string.compare function, but ignore upper and lower cases.
	/// \param rLhs [in]
	/// \param rRhs [in]
	/// \returns int 0 = strings are similar
	//************************************
	int CompareNoCase(const std::string& rLhs, const std::string & rRhs);

	//************************************
	/// Make all character to lower case.
	/// \param pString [in] The source string to be changed.
	/// \returns std::string
	//************************************
	std::string MakeLower(LPCTSTR pString);
	//************************************
	/// Make all character to upper case.
	/// \param pString [in] The source string to be changed.
	/// \returns std::string
	//************************************
	std::string MakeUpper(LPCTSTR pString);

	//************************************
	/// Trim defined character until another character is found from the left (TrimLeft), right (TrimRight) or both (Trim)
	/// \param pString[in] The source string to be changed
	/// \param pTrimChar [in,out] Character to trim.
	/// \returns std::string
	//************************************
	std::string Trim(LPCTSTR pString, LPCTSTR pTrimChar = _T(" \t\n"));
	std::string TrimLeft(LPCTSTR pString, LPCTSTR pTrimChar = _T(" \t\n"));
	std::string TrimRight(LPCTSTR pString, LPCTSTR pTrimChar = _T(" \t\n"));

	//************************************
	/// Copy a number of character from the Left of the string
	/// \param rStringValue [in] The source string, will not be changed.
	/// \param count [in] Count of character to copy. If count > size, it returns an empty String.
	/// \returns std::string The copied string.
	//************************************
	std::string Left( const std::string& rStringValue, size_t count );

	//************************************
	/// Copy a number of character from the mid of the string
	/// \param rStringValue [in] The source string, will not be changed.
	/// \param count [in] Count of character to copy. If count > size, it return a copy of rStringValue.
	/// \returns std::string The copied string.
	//************************************
	std::string Mid( const std::string& rStringValue, size_t count );

	//************************************
	/// Copy a number of character from the right part of the string.
	/// \param rStringValue [in] The source string, will not be changed.
	/// \param count [in] Count of character to copy.
	/// \returns std::string The copied string.
	//************************************
	std::string Right( const std::string& rStringValue, size_t count );

	//************************************
	//! Method to remove a list of characters
	/// \param rStringValue [in][out] The source string can be changed
	//! \param ExcludeCharacters [in] the characters to remove
	//************************************
	void RemoveCharacters(std::string& rStringValue, LPCTSTR ExcludeCharacters);

	//************************************
	//! Method to validate the string with a list of characters
	/// \param rStringValue [in] The source string, will not be changed.
	//! \param ValidCharacters [in] the characters which are valid
	//************************************
	std::string ValidateString(const std::string& rStringValue, LPCTSTR ValidCharacters);

	//************************************
	// Description:  Check if searchString is found in this string. For the searchString wildcard "*" can be used.
	// Parameter:  searchString <in>:  the string to look for
	// Parameter:  offsetSource <in>:  offset where the search in this string will started.  Default is 0 (the beginning).
	// Parameter:  offsetSearch <in>:  offset where from the search string is used.  Default is 0 (the beginning)
	// Returns:  bool:  true if searchString is found
	//************************************
	bool isSubmatch(const std::string& rStringValue, const std::string& rSearchString, size_t offsetSource = 0, size_t offsetSearch = 0 );

	//************************************
	/// Search in the string an substring and replace all of them with the new substring.
	/// \param rStringValue [in,out] The source string to be changed.
	/// \param pFromStr [in] search substring
	/// \param pToStr [in] replace substring
	/// \returns std::string& Reference to rStringValue
	//************************************
	std::string& searchAndReplace( std::string& rStringValue, const std::string::traits_type::char_type* pFromStr, const std::string::traits_type::char_type* pToStr, size_t beginPos = 0);

	//************************************
	/// Converts the value of objects to strings based on the formats specified and inserts them into another string. 
	/// \param pFormat [in] Format 
	/// \param  [in] A list of objects
	/// \returns std::string New created object
	//************************************
	std::string Format(LPCSTR pFormat, ...); //NOTE! Output maximum is 2000 characters
	std::string Format(LPCWSTR pFormat, ...); //NOTE! Output maximum is 2000 characters

	//************************************
	/// Check if string can convert to a number and return this if possible.
	/// \param Value [out] The converted number.
	/// \param failIfLeftoverChars [in] If value == true, it retrun also false if after convert unused character left (expect of spaces), If value == false, it return only false if convert failed.
	/// \returns bool Return if convert was succeeded.
	//************************************
	template<typename T>
	bool Convert2Number(const std::string& rStringValue, T& Value, bool failIfLeftoverChars = false);

	template bool Convert2Number(const std::string& rStringValue, double&, bool);
	template bool Convert2Number(const std::string& rStringValue, long&, bool);

	//************************************
	/// Load the string from the resource dll
	/// \param ResourceID [in] The resource ID to load from the resource dll
	/// \returns std::string Created string with the loaded string
	//************************************
	std::string LoadStdString( UINT ResourceID );

	std::string to_utf8(const _bstr_t& rUtf16);
	std::string to_utf8(const std::wstring& rUtf16);
	std::string to_utf8(const std::string& rAnsi);

	std::wstring to_utf16(const std::string& rUtf8, int cp = CP_UTF8);

	std::string to_ansi(const std::string& rUtf8);

	//************************************
	/// Places escape character before double quotes in a String
	/// \param rString [in out] String to convert
	/// \param rString [in] convert control characters
	/// \returns true on success
	//************************************
	bool AddEscapeSpecialCharacters(std::string& rString, bool bConvertCtrl);

	//************************************
	/// Removes any escape characters before double quotes in a String
	/// \param rString [in out] String to convert
	/// \param rString [in] convert control characters
	/// \returns true on success
	//************************************
	bool RemoveEscapedSpecialCharacters(std::string& rString, bool bConvertCtrl);

	inline std::string AsString(short Value) { return Format(_T("%d"), Value); };
	inline std::string AsString(int Value) { return Format(_T("%d"), Value); };
	inline std::string AsString(long Value) { return Format(_T("%d"), Value); };
	inline std::string AsString(unsigned short Value) { return Format(_T("%u"), Value); };
	inline std::string AsString(unsigned int Value) { return Format(_T("%u"), Value); };
	inline std::string AsString(unsigned long Value) { return Format(_T("%u"), Value); };
	inline std::string AsString(double Value) { return Format(_T("%f"), Value); };

	std::string makeNameUnique(const std::string& rOriginalName, const std::vector<std::string>& rStringsToBeSearched, bool useExplorerStyle);

	///case-insensitive comparison
	bool isStringInList(const std::string& rToBeFound, const std::vector<std::string>& rStringsToBeSearched, const std::string& rExclude = _T("")); 
	namespace LoadedStrings
	{
		//SVOResource/resource.h can not be included, because some apps have its own resource.h and this would lead to macro redefinition
		static const std::string g_ToolSetName{ SvUl::LoadStdString(10151/*IDS_CLASSNAME_SVTOOLSET*/) };
	}
} // namespace SvUl

