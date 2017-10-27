//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:   SVStringConversions.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:40:56  $
//******************************************************************************
#pragma once
#pragma region Includes
#include <string>
#include <boost/static_assert.hpp>
#pragma endregion Includes

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

	std::wstring to_utf16(const std::string & utf8);
}

#include "SVStringConversions.inl"
