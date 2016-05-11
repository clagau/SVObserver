//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEncodeDecodeUtilities
//* .File Name       : $Workfile:   SVEncodeDecodeUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   25 Apr 2014 11:12:32  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <atlenc.h>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/scoped_array.hpp>
#include "SVEncodeDecodeUtilities.h"

HRESULT SVEncodeDecodeUtilities::Base64EncodeToByteBufferFromByteBuffer( int& p_rOutputBufferSize, char* p_pOutput, size_t p_InputBufferSize, const unsigned char* p_pInput )
{
	HRESULT l_Status = S_OK;

	if( ( 0 < p_rOutputBufferSize ) && ( nullptr != p_pOutput ) )
	{
		if( ( 0 < p_InputBufferSize ) && ( nullptr != p_pInput ) )
		{
			int enc_len = ::Base64EncodeGetRequiredLength(static_cast<int>(p_InputBufferSize), ATL_BASE64_FLAG_NOCRLF);

			if( enc_len <= p_rOutputBufferSize )
			{
				if( !( ::Base64Encode( p_pInput, static_cast<int>(p_InputBufferSize), p_pOutput, &enc_len, ATL_BASE64_FLAG_NOCRLF) ) )
				{
					l_Status = E_UNEXPECTED;
				}

				p_rOutputBufferSize = enc_len;
			}
			else
			{
				p_rOutputBufferSize = enc_len;

				l_Status = E_INVALIDARG;
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVEncodeDecodeUtilities::Base64EncodeToStringFromByteBuffer( std::string& p_rOutput, size_t p_InputBufferSize, const unsigned char* p_pInput )
{
	HRESULT l_Status = S_OK;

	if( ( 0 < p_InputBufferSize ) && ( nullptr != p_pInput ) )
	{
		int enc_len = ::Base64EncodeGetRequiredLength(static_cast<int>(p_InputBufferSize), ATL_BASE64_FLAG_NOCRLF);
		boost::scoped_array<char> enc_buff( new char[enc_len + 1]);

		::memset( enc_buff.get(), '\0', ( enc_len + 1 ) );

		l_Status = Base64EncodeToByteBufferFromByteBuffer( enc_len, enc_buff.get(), p_InputBufferSize, p_pInput );

		if( S_OK == l_Status )
		{
			p_rOutput = enc_buff.get();
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVEncodeDecodeUtilities::Base64EncodeToStringFromString( std::string& p_rOutput, const std::string& p_rInput )
{
	HRESULT l_Status = S_OK;

	if( !( p_rInput.empty() ) )
	{
		l_Status = Base64EncodeToStringFromByteBuffer( p_rOutput, p_rInput.size(), reinterpret_cast< const unsigned char* >( p_rInput.data() ) );
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVEncodeDecodeUtilities::Base64EncodeToStringFromFile( std::string& p_rOutput, const SVString& p_rFileName )
{
	HRESULT l_Status = S_OK;

	HANDLE hFile = ::CreateFile(p_rFileName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD file_sz = ::GetFileSize(hFile, nullptr);
		if( 0 < file_sz )
		{
			HANDLE hMapping = ::CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, file_sz, nullptr);
			if( nullptr != hMapping )
			{
				BYTE * buff = (BYTE *)::MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, file_sz);
				if( nullptr != buff )
				{
					l_Status = Base64EncodeToStringFromByteBuffer( p_rOutput, file_sz, buff );

					::UnmapViewOfFile(buff);
				}
				else
				{
					l_Status = ::GetLastError();;
				}

				::CloseHandle(hMapping);
			}
			else
			{
				l_Status = ::GetLastError();;
			}
		}
		else
		{
			l_Status = ::GetLastError();;
		}

		::CloseHandle(hFile);
	}
	else
	{
		l_Status = ::GetLastError();;
	}

	return l_Status;
}

HRESULT SVEncodeDecodeUtilities::Base64DecodeToByteBufferFromString( int& p_rOutputBufferSize, unsigned char* p_pOutput, const std::string& p_rInput )
{
	HRESULT l_Status = S_OK;

	if( !( p_rInput.empty() ) && ( 0 < p_rOutputBufferSize ) && ( nullptr != p_pOutput ) )
	{
		int l_BufferSize = p_rOutputBufferSize;
		if( !( ::Base64Decode( p_rInput.c_str(), static_cast<int>(p_rInput.size()), p_pOutput, &l_BufferSize ) ) )
		{
			l_Status = E_UNEXPECTED;
		}
		p_rOutputBufferSize = static_cast<int>(l_BufferSize);
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVEncodeDecodeUtilities::Base64DecodeToFileFromString( const SVString& p_rFileName, const std::string& p_rInput )
{
	HRESULT l_Status = S_OK;

	HANDLE hFile = ::CreateFile(p_rFileName.c_str(), GENERIC_WRITE|GENERIC_READ, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if( hFile != INVALID_HANDLE_VALUE )
	{
		int buff_len = ::Base64DecodeGetRequiredLength(static_cast<int>(p_rInput.size()));
		HANDLE hMapping = ::CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(buff_len), nullptr);
		if( nullptr != hMapping )
		{
			BYTE * buff = (BYTE *)::MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, buff_len);
			if (nullptr != buff)
			{
				l_Status = Base64DecodeToByteBufferFromString( buff_len, buff, p_rInput );

				::UnmapViewOfFile(buff);
			}
			else
			{
				l_Status = ::GetLastError();
			}

			::CloseHandle(hMapping);
		}
		else
		{
			l_Status = ::GetLastError();
		}

		::SetFilePointer(hFile, buff_len, nullptr, FILE_BEGIN);
		::SetEndOfFile(hFile);
		::CloseHandle(hFile);
	}
	else
	{
		l_Status = ::GetLastError();
	}

	return l_Status;
}

