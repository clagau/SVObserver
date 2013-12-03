//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFile
//* .File Name       : $Workfile:   SVFile.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:12:42  $
//******************************************************************************

#include "stdafx.h"
#include <io.h>
#include <share.h>
#include "SVFile.h"
#include "SVUtilityLibrary/SVString.h"

SVFile::SVFile()
: m_pFile( NULL )
{
}

SVFile::~SVFile()
{
	Close();
}

bool SVFile::IsOpen() const
{
	return m_pFile != NULL;
}

HRESULT SVFile::Open( const char* p_szFileName, UINT p_OpenFlags )
{
	HRESULT l_Status( S_OK );

	Close();

	SVString l_Mode;
	int l_Share( 0 );

	l_Status = GetAccessMode( l_Mode, l_Share, p_OpenFlags );

	if( l_Status == S_OK )
	{
		m_pFile = ::_fsopen( p_szFileName, l_Mode.ToString(), l_Share );

		if( m_pFile == NULL )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

void SVFile::Close()
{
	if( m_pFile != NULL )
	{
		::fflush( m_pFile );
		::fclose( m_pFile );

		m_pFile = NULL;
	}
}

UINT SVFile::Read( void* lpBuf, UINT nCount )
{
	UINT l_BytesRead( 0 );

	if( m_pFile != NULL )
	{
		l_BytesRead = static_cast<UINT>(::fread( lpBuf, 1, nCount, m_pFile ));
	}

	return l_BytesRead;
}

HRESULT SVFile::ReadContents( SVByteVector& p_rContents )
{
	HRESULT l_Status = S_OK;

	p_rContents.clear();

	if( m_pFile != NULL )
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

	if( m_pFile != NULL )
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

	if( m_pFile != NULL )
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

	if( m_pFile != NULL )
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

	if( m_pFile != NULL )
	{
		if( ::fseek( m_pFile, 0L, SEEK_END ) == 0 )
		{
			fpos_t l_Position( 0 );

			if( fgetpos( m_pFile, &l_Position ) == 0 )
			{
				l_Location = l_Position;
			}
		}
	}

	return l_Location;
}

HRESULT SVFile::GetAccessMode( SVString& p_rAccessMode, int& p_rShareMode, UINT p_OpenFlags )
{
	HRESULT l_Status( S_OK );

	p_rAccessMode.clear();
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

	if( l_Status == S_OK )
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

	if( l_Status == S_OK )
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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSystemLibrary\SVFile.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 10:12:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:51:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:12:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include new utility classes for bitmap management and conversion, and file minipulation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Apr 2010 12:58:32   jspila
 * Project: SVSVIMInterface
 * Change Request (SCR) nbr: 62
 * SCR Title: Update Font Object with New Interface
 * Checked in by: Joe; Joe Spila
 * Change Description: Initial Check-in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
