//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:   SVStringConversions.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 13:27:10  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <tchar.h>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/static_assert.hpp>
#include "SVString.h"
#pragma endregion Includes
namespace SvUl
{
	template<typename T>
	std::string to_utf8(const T& rUtf16)
	{
		// do static assert
		// Only allowed types are _bstr_t and std::wstring
		//BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Use of non allowed type, allowed types are _bstr_t and std::wstring");
		BOOST_STATIC_ASSERT(sizeof(T) == 0);
	}

	template<>
	std::string to_utf8(const wchar_t& rUtf16);

	std::wstring to_utf16(const std::string& rUtf8, int cp = CP_UTF8);

	inline SVString AsString(short n)			{return SvUl_SF::Format( _T("%d"), n );};
	inline SVString AsString(int n)				{return SvUl_SF::Format( _T("%d"), n );};
	inline SVString AsString(long n)			{return SvUl_SF::Format( _T("%d"), n );};
	inline SVString AsString(unsigned short n)	{return SvUl_SF::Format( _T("%u"), n );};
	inline SVString AsString(unsigned int n)	{return SvUl_SF::Format( _T("%u"), n );};
	inline SVString AsString(unsigned long n)	{return SvUl_SF::Format( _T("%u"), n );};
	inline SVString AsString(double d)			{return SvUl_SF::Format( _T("%f"), d );};

	////////////////////////////////////////////////////////////////////////////////
	// Places escape character before double quotes in a String
	////////////////////////////////////////////////////////////////////////////////
	bool AddEscapeSpecialCharacters( SVString& rString, bool bConvertCtrl );

	////////////////////////////////////////////////////////////////////////////////
	// Removes any escape characters before double quotes in a String
	////////////////////////////////////////////////////////////////////////////////
	bool RemoveEscapedSpecialCharacters( SVString& rString, bool bConvertCtrl );

} // namespace SvUl

#pragma region Inline
#include "SVStringConversions.inl"
#pragma endregion Inline
