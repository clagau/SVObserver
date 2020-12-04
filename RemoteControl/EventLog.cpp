//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file Eventlog.cpp
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//******************************************************************************

#pragma once

#pragma region Includes
#include "Stdafx.h"
#include "SVLogLibrary/Logging.h"
#pragma endregion Includes

namespace SvRc
{
constexpr LPCTSTR separator = _T(" ");

void SVLog(HRESULT hr, LPCTSTR file, long line)
{
	if (FAILED(hr))
	{
		std::stringstream ss;
		char* msg = nullptr;
		DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
		if (S_OK == (FormatMessage(flags, nullptr, hr,	0, reinterpret_cast<char*> (&msg), 64, nullptr)) && msg != nullptr)
		{
			ss << msg << separator << file << separator << line;
			::LocalFree(msg);
		}
		else
		{
			std::stringstream hr_ss;
			hr_ss << std::hex << hr;
			ss << hr_ss.str().c_str() << separator << file << separator << line;
		}
		
		if (hr == S_OK)
		{
			SV_LOG_GLOBAL(info) << ss.str();
		}
		else
		{
			SV_LOG_GLOBAL(error) << ss.str();
		}
		ss << std::endl;
		::OutputDebugString(ss.str().c_str());
	}
}

void SVLog(LPCTSTR msg, LPCTSTR file, long line)
{
	std::stringstream ss;
	ss << msg << separator << file << separator << line;
	SV_LOG_GLOBAL(error) << ss.str();
	ss << std::endl;
	::OutputDebugString(ss.str().c_str());
}

} //namespace SvRc