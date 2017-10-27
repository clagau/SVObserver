//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:$
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:$
//* .Check In Date   : $Date:$
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include <comdef.h>
#include <boost/config.hpp>
#include <boost/scoped_array.hpp>
#include "SVStringConversions.h"
#pragma endregion Includes

namespace SVStringConversions
{
	template<>
	inline std::string to_utf8(const _bstr_t & utf16)
	{
		if (utf16.length())
		{
			std::wstring wstr = static_cast<wchar_t*>(utf16);
			int sz = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr)+1;
			boost::scoped_array<char> buff(new char[sz]);
			::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buff.get(), sz, nullptr, nullptr);
			return std::string(buff.get());
		}
		return std::string();
	}

	template<>
	inline std::string to_utf8(const std::wstring & utf16)
	{
		int sz = ::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr)+1;
		boost::scoped_array<char> buff(new char[sz]);
		::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, buff.get(), sz, nullptr, nullptr);
		return std::string(buff.get());
	}

	inline std::wstring to_utf16(const std::string & utf8)
	{
		int sz = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0)+1;
		boost::scoped_array<wchar_t> buff(new wchar_t[sz]);
		::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buff.get(), sz);
		return std::wstring(buff.get());
	}
}
