//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// NaturalStringCompare is a function object that is used to do a natural comparison
/// as opposed to a lexicographical comparision of 2 strings.
/// The comparision used is also case insensitive.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <algorithm>
#pragma endregion Includes

namespace SvUl
{
	// compare character case-insensitive
	template<typename char_type>
	struct icompare_char
	{
		bool operator()(char_type c1, char_type c2) 
		{
			return std::toupper(c1) < std::toupper(c2);
		}
	};

	/// @Note: This function works on strings with trailing numbers
	/// if a natural sort order is desired for strings with numbers 
	// that are not in the trailing position, then it could be modified
	// to use the API function StrCmpLogical
	// return true if s1 comes before s2
	template<typename string_type>
	struct NaturalStringCompare 
	{
		bool operator()(const string_type& s1, const string_type& s2) 
		{
			if (s1.size() < s2.size())
				return true;
			if (s1.size() > s2.size())
				return false;
			return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), icompare_char<string_type::value_type>());
		}
	};
} // namespace SvUl
