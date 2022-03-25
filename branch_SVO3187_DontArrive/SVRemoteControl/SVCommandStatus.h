//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandStatus
//* .File Name       : $Workfile:   SVCommandStatus.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   06 Nov 2014 08:43:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <string>
#include <sstream>
#include <comdef.h>
#pragma endregion Includes

struct SVCommandStatus
{
	SVCommandStatus() : hResult( S_OK ), errorText() {}
	SVCommandStatus( const SVCommandStatus& p_rObject ) : hResult( p_rObject.hResult ), errorText( p_rObject.errorText ) {}
	SVCommandStatus( HRESULT p_Status, const std::wstring& p_rErrorText ) : hResult( p_Status ), errorText( p_rErrorText ) {}
	~SVCommandStatus() { clear(); }
	SVCommandStatus& operator=(const SVCommandStatus& rObject) 
	{
		if (this != &rObject)
		{
			hResult = rObject.hResult;
			errorText = rObject.errorText;
		}
		return *this;
	}

	void clear() { hResult = S_OK; errorText.clear(); }

	void FormatMessage();

	HRESULT hResult;
	std::wstring errorText;
};

inline void SVCommandStatus::FormatMessage()
{
	wchar_t * buff = nullptr;
	if (::FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hResult,
		0,
		(LPWSTR)&buff,
		64,
		nullptr
		) && buff != nullptr)
	{
		errorText = buff;
		::LocalFree(buff);
	}
	else
	{
		std::wstringstream ss;
		ss << L"Unknown error (" << hResult << L").";
		errorText = ss.str();
	}
}
