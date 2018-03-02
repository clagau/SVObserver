//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFile
//* .File Name       : $Workfile:   SVFile.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:38:22  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <io.h>
#include <share.h>
#include "SVFile.h"
#pragma endregion Includes

SVFile::SVFile()
: m_pFile( nullptr )
{
}

SVFile::~SVFile()
{
	Close();
}

bool SVFile::IsOpen() const
{
	return nullptr != m_pFile;
}

HRESULT SVFile::Open( const char* p_szFileName, UINT p_OpenFlags )
{
	HRESULT l_Status( S_OK );

	Close();

	_bstr_t l_Mode;
	int l_Share( 0 );

	l_Status = GetAccessMode( l_Mode, l_Share, p_OpenFlags );

	if( S_OK == l_Status )
	{
		m_pFile = ::_fsopen( p_szFileName, static_cast< const char* >( l_Mode ), l_Share );

		if( nullptr == m_pFile )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

void SVFile::Close()
{
	if( nullptr != m_pFile )
	{
		::fflush( m_pFile );
		::fclose( m_pFile );

		m_pFile = nullptr;
	}
}

UINT SVFile::Read( void* lpBuf, UINT nCount )
{
	UINT l_BytesRead( 0 );

	if( nullptr != m_pFile )
	{
		l_BytesRead = static_cast< UINT >( ::fread( lpBuf, 1, nCount, m_pFile ) );
	}

	return l_BytesRead;
}

HRESULT SVFile::ReadContents( SVByteVector& p_rContents )
{
	HRESULT l_Status = S_OK;

	p_rContents.clear();

	if( nullptr != m_pFile )
	{
		UINT l_BytesRead( 0 );

		unsigned long strSize = static_cast< unsigned long >( GetLength() );

		p_rContents.resize( strSize );

		l_BytesRead = Read( &( p_rContents[ 0 ] ), strSize );

		if( l_BytesRead != strSize )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVFile::Write( const void* lpBuf, UINT nCount )
{
	HRESULT l_Status( S_OK );

	if( nullptr != m_pFile )
	{
		if( ::fwrite( lpBuf, 1, nCount, m_pFile ) != nCount )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVFile::WriteContents( const SVByteVector& p_rContents )
{
	HRESULT l_Status( S_OK );

	if( nullptr != m_pFile )
	{
		unsigned long strSize = static_cast< unsigned long >( p_rContents.size() );

		l_Status = Write( &( p_rContents[ 0 ] ), strSize );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

ULONGLONG SVFile::GetLength() const
{
	ULONGLONG l_Size( 0 );

	if( nullptr != m_pFile )
	{
		int l_FileDescripter( ::_fileno( m_pFile ) );

		if( -1 < l_FileDescripter )
		{
			l_Size = ::_filelength( l_FileDescripter );
		}
	}

	return l_Size;
}

ULONGLONG SVFile::SeekToEnd()
{
	ULONGLONG l_Location( 0 );

	if( nullptr != m_pFile )
	{
		if( 0 == ::fseek( m_pFile, 0L, SEEK_END ) )
		{
			fpos_t l_Position( 0 );

			if( 0 == fgetpos( m_pFile, &l_Position ) )
			{
				l_Location = l_Position;
			}
		}
	}

	return l_Location;
}

ULONGLONG SVFile::Seek(LONGLONG lOff, UINT nFrom)
{
	ULONGLONG l_Location( 0 );

	if( nullptr != m_pFile )
	{
		l_Location = ::fseek( m_pFile, static_cast< long >( lOff ), nFrom );
	}

	return l_Location;
}

HRESULT SVFile::GetAccessMode( _bstr_t& p_rAccessMode, int& p_rShareMode, UINT p_OpenFlags )
{
	HRESULT l_Status( S_OK );

	p_rAccessMode = _T( "" );
	p_rShareMode = 0;

	UINT l_AccessFlags( p_OpenFlags & ( modeRead | modeWrite | modeReadWrite | modeCreate | modeNoTruncate ) );
	UINT l_TypeFlags( p_OpenFlags & ( typeText | typeBinary ) );
	UINT l_ShareFlags( p_OpenFlags & ( shareDenyNone ) );

	switch( l_AccessFlags )
	{
		case modeRead:
		{
			p_rAccessMode = _T( "r" );

			break;
		}
		case modeWrite:
		case modeWrite | modeCreate:
		{
			p_rAccessMode = _T( "w" );

			break;
		}
		case modeReadWrite:
		case modeReadWrite | modeCreate | modeNoTruncate:
		{
			p_rAccessMode = _T( "r+" );

			break;
		}
		case modeReadWrite | modeCreate:
		{
			p_rAccessMode = _T( "w+" );

			break;
		}
		default:
		{
			l_Status = E_FAIL;

			break;
		}
	}

	if( S_OK == l_Status )
	{
		switch( l_TypeFlags )
		{
			default:
			case typeBinary:
			{
				p_rAccessMode += _T( "b" );

				break;
			}
			case typeText:
			{
				p_rAccessMode += _T( "t" );

				break;
			}
		}
	}

	if( S_OK == l_Status )
	{
		switch( l_ShareFlags )
		{
			default:
			case shareDenyNone:
			{
				p_rShareMode = _SH_DENYNO;

				break;
			}
		}
	}
	return l_Status;
}