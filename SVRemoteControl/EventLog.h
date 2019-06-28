//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Logging
//* .File Name       : $Workfile:   Logging.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Jan 2015 10:07:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <winnt.h>
#include <sstream>
#include "SVLogLibrary/Logging.h"
#pragma endregion Includes


inline void SVLog(HRESULT hr, const wchar_t * file, long line)
{
	if (!SUCCEEDED(hr))
	{
		std::wstringstream ws;
		wchar_t * msg = nullptr;
		const std::wstring separator = L" ";
		if (SUCCEEDED(FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			hr,
			0,
			reinterpret_cast<wchar_t *>(&msg),
			64,
			nullptr)) && msg != nullptr)
		{
			ws << msg << separator << file << separator << line;
			::LocalFree(msg);
		}
		else
		{
			std::wstringstream hr_ws;
			hr_ws << std::hex << hr;
			ws << hr_ws.str().c_str() << separator << file << separator << line;
		}
		
		if (hr == S_OK)
		{
			SV_LOG_GLOBAL(info) << ws.str();
		}
		else
		{
			SV_LOG_GLOBAL(error) << ws.str();
		}
		ws << std::endl;
		::OutputDebugStringW(ws.str().c_str());
	}
}

inline void SVLog(const wchar_t * msg, const wchar_t * file, long line)
{
	std::wstringstream ws;
	const std::wstring separator = L" ";
	ws << msg << separator << file << separator << line;
	SV_LOG_GLOBAL(error) << ws.str();
	ws << std::endl;
	::OutputDebugStringW(ws.str().c_str());
}

inline void SVLogDebug(const wchar_t * msg)
{
	std::wstringstream ws;
	const std::wstring separator = L" ";
	ws << msg;
	SV_LOG_GLOBAL(info) << ws.str();
	ws << std::endl;
	::OutputDebugStringW(ws.str().c_str());
}


#ifdef SVLOG
#undef SVLOG
#endif

#define SVLOG(x) SVLog(x, __FILEW__, __LINE__)
