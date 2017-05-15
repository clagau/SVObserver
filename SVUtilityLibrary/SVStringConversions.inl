//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringConversions
//* .File Name       : $Workfile:   SVStringConversions.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 Jul 2013 13:27:12  $
//******************************************************************************

namespace SvUl
{
	template<>
	inline std::string to_utf8(const _bstr_t& rUtf16)
	{
		if (rUtf16.length())
		{
			const wchar_t* const pUtf16 = static_cast<const wchar_t* const> (rUtf16);
			int size = ::WideCharToMultiByte(CP_UTF8, 0, pUtf16, -1, nullptr, 0, nullptr, nullptr) + 1;
			std::vector<char> buff;
			buff.resize( size, '\0' );
			::WideCharToMultiByte(CP_UTF8, 0, pUtf16, -1, &buff.at(0), size, nullptr, nullptr);
			return std::string(&buff.at(0));
		}
		return std::string();
	}

	template<>
	inline std::string to_utf8(const std::wstring& rUtf16)
	{
		int size = ::WideCharToMultiByte(CP_UTF8, 0, rUtf16.c_str(), -1, nullptr, 0, nullptr, nullptr) + 1;
		std::vector<char> buff;
		buff.resize( size, '\0' );
		::WideCharToMultiByte(CP_UTF8, 0, rUtf16.c_str(), -1, &buff.at(0), size, nullptr, nullptr);
		return std::string(&buff.at(0));
	}

	inline std::wstring to_utf16(const std::string & utf8, int cp)
	{
		int size = ::MultiByteToWideChar(cp, 0, utf8.c_str(), -1, nullptr, 0) + 1;
		std::vector<wchar_t> buff;
		buff.resize( size, '\0' );
		::MultiByteToWideChar(cp, 0, utf8.c_str(), -1, &buff.at(0), size);
		return std::wstring(&buff.at(0));
	}
} // namespace SvUl

