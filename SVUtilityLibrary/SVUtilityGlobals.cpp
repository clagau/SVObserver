//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityGlobals
//* .File Name       : $Workfile:   SVUtilityGlobals.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVUtilityGlobals.h"
#include "SVString.h"

HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind copy of the requested element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
    void* pElement;
    HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
    if (S_OK == hr)
	{
        memcpy(pv, pElement, psa->cbElements);
	}
    return hr;
}

HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind put of the specified element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
    void* pElement;
    HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
    if (S_OK == hr)
	{
		memcpy(pElement, &pv, psa->cbElements);
	}
    return hr;
}

HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv)
// sets the pointer to point to the actual memory of the SAFEARRAY that contains the element.
{
    DWORD dwOffset=0;
    DWORD dwIndexOffset=1;
    for (int i=psa->cDims-1; i >= 0 ; i--)
    {
		long lIndex = *(rgIndices+i);
		int iDimensionIndex = psa->cDims-1 - i;
		if ( lIndex < psa->rgsabound[iDimensionIndex].lLbound || lIndex >= (long)( psa->rgsabound[iDimensionIndex].lLbound + psa->rgsabound[iDimensionIndex].cElements ) )
			return DISP_E_BADINDEX;
		
		long dwIndex = (lIndex - psa->rgsabound[iDimensionIndex].lLbound);   // offset index in dimension used for figuring out memory offset
		
		dwOffset += dwIndex * dwIndexOffset;
		dwIndexOffset *= psa->rgsabound[iDimensionIndex].cElements;   // offset of dimension used in calculating next dim
    }
    dwOffset *= psa->cbElements;    // scale by size of element
	
    BYTE* pElement = ((BYTE*) psa->pvData) + dwOffset;
    *ppv = pElement;
	
    return S_OK;
}

void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}
}

HRESULT KeepPrevErrorReturnPrev( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}

	return p_rhrPrev;
}

HRESULT KeepPrevErrorReturnNew( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}

	return p_hrNew;
}

// TStrPathName must be the full pathname of a file
// or a full pathname of a directory with a slash on the end!
// e.g.: c:/dir1/dir2/dir3/filename.ext
//       c:/dir1/dir2/dir3/
// strlen of TStrPathName must not exceed _MAX_PATH length!
// If the drive and the directory exists, the function returns
// true. 
// If the directory not exits and BCreateIfNotExists is true,
// the return value is true if the directory is creatible.
//
// In all other cases it returns false.
bool SVCheckPathDir( LPCTSTR PathName, bool CreateIfDoesNotExist )
{
	TCHAR  curPath[ _MAX_PATH ];
	// Save current settings...
	if( nullptr == _tgetcwd( curPath, _MAX_PATH )  )
	{
		curPath[ 0 ] = _TCHAR( '/0' );
	}
	// check path
	TCHAR  path[ _MAX_PATH ];
	TCHAR* pLast = ( TCHAR* ) PathName;
	TCHAR  drive[ _MAX_DRIVE ];
	TCHAR  dir[ _MAX_DIR ];
	TCHAR  fname[ _MAX_FNAME ];  
	TCHAR  ext[ _MAX_EXT ];
	if( PathName && _tcslen( PathName ) < _MAX_PATH )
	{
		_tsplitpath( pLast, drive, dir, fname, ext );
		if( _tcslen( drive ) == 0 && nullptr != _tcschr( pLast, _TCHAR( ':' ) ) )
		{
			// Restore settings...
			_tchdir( curPath );
			return false;
		}

		while( pLast = _tcspbrk( pLast, _T( "\\/" ) ) )
		{
			_tcsncpy( path, PathName, pLast++ - PathName + 1 );
			path[ pLast - PathName ] = _TCHAR( '\0' );

			if( _tchdir( path ) != 0 )
			{
				// Check for creation...
				if( ! CreateIfDoesNotExist )
				{
					// Restore settings...
					_tchdir( curPath );
					return false;
				}

				// We have to make a new directory
				if( _tmkdir( path ) != 0 || _tchdir( path ) != 0 )
				{
					// Restore settings...
					_tchdir( curPath );
					return false;
				}
			}
		}

		// Restore settings...
		_tchdir( curPath );
		return true;
	}

	// Restore settings...
	_tchdir( curPath );
	return false;
}

bool SVDeleteFiles( LPCTSTR PathName, bool IncludeSubDirectories )
{
	// TStrPathName must be the full pathname of a file,
	// that also includes * for the filename or the file extension!
	// e.g.: c:/dir1/dir2/dir3/filename.ext
	//       c:/dir1/dir2/*.*
	// strlen of TStrPathName must not exceed _MAX_PATH length!

	// If all deleting is done well, the function returns
	// true. 
	// In all other cases it returns false.

	TCHAR  path[ _MAX_PATH ];
	TCHAR  drive[ _MAX_DRIVE ];
	TCHAR  dir[ _MAX_DIR ];
	TCHAR  fname[ _MAX_FNAME ];  
	TCHAR  ext[ _MAX_EXT ];
	if( PathName && _tcslen( PathName ) < _MAX_PATH )
	{
		_tsplitpath( PathName, drive, dir, fname, ext );

		// Search for file(s)...
		WIN32_FIND_DATA findData;
		bool Result( true );
		HANDLE hFindFile = FindFirstFile( PathName, &findData );
		do
		{
			if( hFindFile )
			{
				_tmakepath( path, drive, dir, findData.cFileName, _T( "" ) );
				if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if( _tcscmp( findData.cFileName, _T( "." ) ) && _tcscmp( findData.cFileName, _T( ".." ) ) &&
						IncludeSubDirectories 
					  )
					{
						// Delete sub directory contents...
						_tcscat( path, _T( "\\*.*" ) );
						Result = SVDeleteFiles( path, IncludeSubDirectories ) && Result;

						// Prepare path for deleting sub directory...
						_tmakepath( path, drive, dir, findData.cFileName, _T( "" ) );
						// Check for current directory...
						TCHAR curDir[ _MAX_PATH ];
						_tgetcwd( curDir, _MAX_PATH );
						if( _tcscmp( curDir, path ) == 0 )
						{
							_tmakepath( curDir, drive, dir, _T( "" ), _T( "" ) );
							_tchdir( curDir );
						}
						// Delete sub directory...
						Result = _trmdir( path ) && Result;
					}
				}
				else
				{
					// Delete file...
					Result = DeleteFile( path ) && Result;
				}
			}
		} while( FindNextFile( hFindFile, &findData ) );

		FindClose( hFindFile );
		return Result;
	}

	return false;
}

bool SVFileExists( LPCTSTR FilePath )
{
	if( FilePath )
	{
		WIN32_FIND_DATA findData;
		HANDLE hFindFile = FindFirstFile( FilePath, &findData );
		if( hFindFile != INVALID_HANDLE_VALUE )
		{
			// Success...
			FindClose( hFindFile );
			return true;
		}
	}
	return FALSE;
}

