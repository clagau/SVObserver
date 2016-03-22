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

//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/static_assert.hpp>
//Moved to precompiled header: #include <boost/scoped_array.hpp>

namespace SVStringConversions
{
	template<typename T>
	inline std::string to_utf8(const T& utf16)
	{
		// do static assert
		// Only allowed types are _bstr_t and std::wstring
		//BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Use of non allowed type, allowed types are _bstr_t and std::wstring");
		BOOST_STATIC_ASSERT(sizeof(T) == 0);
	}

	std::wstring to_utf16(const std::string & utf8, int cp = CP_UTF8);
}

#include "SVStringConversions.inl"

