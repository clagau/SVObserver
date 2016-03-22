//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobal Function Module
//* .File Name       : $Workfile:   SVGlobal.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Aug 2014 15:49:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <direct.h>
#include "SVGlobal.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVObserver.h"
#include "SVImageProcessingClass.h"
#pragma endregion Includes

TCHAR SVRCCurrentSVCPathName[ _MAX_PATH ];

SVDataManager TheSVDataManager;

//******************************************************************************
//* FUNCTION DEFINITION(S):
//******************************************************************************

void FormatLongerString2( CString& RString, UINT IDS, LPCTSTR LPSZFirst, LPCTSTR LPSZSecond )
{
	// Only for MFC-Threads!

	CString message;
	message.LoadString( IDS );

	if( ! LPSZFirst )
		LPSZFirst = _T( "" );

	if( ! LPSZSecond )
		LPSZSecond = _T( "" );

	int first, second;
	BOOL searching = TRUE;
	while( searching )
	{
		first = message.Find( _T( "%1" ) );
		second = message.Find( _T( "%2" ) );
		if( first >= 0 || second >= 0  )
		{
			if( first < second && first >= 0 )
			{
				RString = RString + message.Left( first ) + LPSZFirst;
				message = message.Mid( first + 2 );
			}
			else
			{
				if( second >= 0 )
				{
					RString = RString + message.Left( second ) + LPSZSecond;
					message = message.Mid( second + 2 );
				}
			}
		}
		else
		{
			RString = RString + message;
			searching = FALSE;
		}
	}
}

CMDIChildWnd* SVSearchForMDIChildWnd( CWnd* PStartWnd )
{
	// Only for MFC-Threads!

	while( PStartWnd != NULL )
	{
		if( ! PStartWnd->IsKindOf( RUNTIME_CLASS( CWnd ) ) )
			return NULL;

		if( PStartWnd->IsKindOf( RUNTIME_CLASS( CMDIChildWnd ) ) )
			return( ( CMDIChildWnd* ) PStartWnd );

		PStartWnd = PStartWnd->GetParent();
	}
	return NULL;
}


// Calculate real pixel number of a line, which is built by Bresenham.
// Works also with negative values.
long SVCalcLinePixelNumber( long Width, long Height )
{
	long Length = max( labs( Width ), labs( Height ) );
	return( ( Width && Height ) ? ++Length : Length );
	
	// Special for Width: 0, Height: 0 => PixelNumber: 1
	//return( ( ( Width == 0 ) ^ ( Height == 0 ) ) ? Length : ++Length );
}


long SVCalcLineXValue( long Pixel, long Width, long Height )
{
	return (long)SVCalcLineDXValue(  Pixel,  Width,  Height );
}

double SVCalcLineDXValue( double Pixel, double Width, double Height )
{
	double l_dResult = 0;

	if( Width != 0 || Height != 0 )
	{
		double l_dMin = min( fabs( Width ), fabs( Height ) ); 
		double l_dMax = max( fabs( Width ), fabs( Height ) );

		long l_lDirection = ( Width * Height >= 0 ) ? 1 : - 1;

		BOOL l_bIsXGradient = l_dMax == fabs( Width );
		BOOL l_bIsQ1Q4 = Width >= 0;

		double l_dGradient = l_dMin / l_dMax * l_lDirection;

		l_dResult = SVCalcLineDXValue( Pixel, l_dGradient, l_bIsXGradient, l_bIsQ1Q4 );
	}

	return l_dResult;
}

// This function calculates the x coordinate of a given pixel point of a line.
// The pixels are counted from line beginning to line end.
// Works also with negative values.
// NOTE:
// ATTENTION! There is no input parameter checking!
// The pixel index starts at zero! 
// A horizontal line has a Width of w, a Height of 0, a Gradient of 0 
// and the IsXGradient parameter must be set to TRUE!
// The Gradient must be calculated as Gradient := Height / Width;
// A vertical line has a Width of 0, a Height of h, a Gradient of 0 
// and the IsXGradient parameter must be set to FALSE!
// The Gradient must be calculated as Gradient := Width / Height;
// Which formula must be used to calculate the Gradient of any other line
// depends on the following conditions
//
//			  min( abs( Width ), abs( Height ) )
// Gradient = ---------------------------------- * ( ( Width * Height >= 0 ) ? 1 : - 1 )
//			  max( abs( Width ), abs( Height ) )
//
// IsXGradient := ( max( abs( Width ), abs( Height ) ) == abs( Width ) )
//
// IsQ1Q4      := ( Width >= 0 )
//
// The return value is also zero based!
long SVCalcLineXValue( long Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q4 )
{
	if( IsXGradient )
		return( IsQ1Q4 ? Pixel : - Pixel );
	
	long RetVal = ( long ) ( fabs( Gradient * Pixel ) + 0.5 );
	if( Pixel < 0 )
		IsQ1Q4 = ! IsQ1Q4;
	return( IsQ1Q4 ? RetVal : - RetVal );
}

double SVCalcLineDXValue( double Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q4 )
{
	if( IsXGradient )
		return( IsQ1Q4 ? Pixel : - Pixel );
	
	double RetVal =  fabs( Gradient * Pixel );
	if( Pixel < 0 )
		IsQ1Q4 = ! IsQ1Q4;
	return( IsQ1Q4 ? RetVal : - RetVal );
}


long SVCalcLineYValue( long Pixel, long Width, long Height )
{
	return (long)SVCalcLineDYValue( Pixel, Width, Height );
}

double SVCalcLineDYValue( double Pixel, double Width, double Height )
{
	double l_dResult = 0;

	if( Width != 0 || Height != 0 )
	{
		double l_dMin = min( fabs( Width ), fabs( Height ) ); 
		double l_dMax = max( fabs( Width ), fabs( Height ) );

		long l_lDirection = ( Width * Height >= 0 ) ? 1 : - 1;

		BOOL l_bIsXGradient = l_dMax == fabs( Width );
		BOOL l_bIsQ1Q2 = Height >= 0;

		double l_dGradient = l_dMin / l_dMax * l_lDirection;

		l_dResult =  SVCalcLineDYValue( Pixel, l_dGradient, l_bIsXGradient, l_bIsQ1Q2 );
	}

	return l_dResult;
}

// This function calculates the y coordinate of a given pixel point of a line.
// The pixels are counted from line beginning to line end.
// Works also with negative values.
// NOTE:
// ATTENTION! There is no input parameter checking!
// The pixel index starts at zero! 
// A horizontal line has a Width of w, a Height of 0, a Gradient of 0 
// and the IsXGradient parameter must be set to TRUE!
// The Gradient must be calculated as Gradient := Height / Width;
// A vertical line has a Width of 0, a Height of h, a Gradient of 0 
// and the IsXGradient parameter must be set to FALSE!
// The Gradient must be calculated as Gradient := Width / Height;
// Which formula must be used to calculate the Gradient of any other line
// depends on the following conditions
//
//			  min( abs( Width ), abs( Height ) )
// Gradient = ---------------------------------- * ( ( Width * Height >= 0 ) ? 1 : - 1 )
//			  max( abs( Width ), abs( Height ) )
//
// IsXGradient := ( max( abs( Width ), abs( Height ) ) == abs( Width ) )
//
// IsQ1Q2      := ( Height >= 0 )
//
// The return value is also zero based!
long SVCalcLineYValue( long Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q2 )
{
	if( ! IsXGradient )
		return( IsQ1Q2 ? Pixel : - Pixel );
	
	long RetVal = ( long ) ( fabs( Gradient * Pixel ) + 0.5 );
	if( Pixel < 0 )
		IsQ1Q2 = ! IsQ1Q2;
	return( IsQ1Q2 ? RetVal : - RetVal );
}

double SVCalcLineDYValue( double Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q2 )
{
	if( ! IsXGradient )
		return( IsQ1Q2 ? Pixel : - Pixel );
	
	double RetVal = fabs( Gradient * Pixel );
	if( Pixel < 0 )
		IsQ1Q2 = ! IsQ1Q2;
	return( IsQ1Q2 ? RetVal : - RetVal );
}






double SVGetDataTypeMin( DWORD DataType )
{
	switch( DataType )
	{
		case SV_1BIT:
			return 0.0;

		case SV_8BIT_INT:
			return 0.0;

		case SV_SIGNED_8BIT_INT:
			return -128.0;
			
		case SV_16BIT_INT:
			return 0.0;

		case SV_SIGNED_16BIT_INT:
			return -32768.0;

		case SV_32BIT_INT:
			return 0.0;

		case SV_SIGNED_32BIT_INT:
			// Be careful: lose precision!
			return -2147483648.0;

		case SV_SIGNED_64BIT_INT:
			// Be careful: lose precision!
			return -9223372036854775808.0;

		case SV_FLOAT:
			return 3.4E-38;

		case SV_DOUBLE:
			return 1.7E-308;
	}
	return 0.0;
}



double SVGetDataTypeMax( DWORD DataType )
{
	switch( DataType )
	{
		case SV_1BIT:
			return 1.0;

		case SV_8BIT_INT:
			return 255.0;

		case SV_SIGNED_8BIT_INT:
			return 127.0;
			
		case SV_16BIT_INT:
			return 65535.0;

		case SV_SIGNED_16BIT_INT:
			return 32767.0;

		case SV_32BIT_INT:
			// Be careful: lose precision!
			return 4294967295.0;

		case SV_SIGNED_32BIT_INT:
			// Be careful: lose precision!
			return 2147483647.0;

		case SV_SIGNED_64BIT_INT:
			// Be careful: lose precision!
			return 9223372036854775807.0;

		case SV_FLOAT:
			return 3.4E38;

		case SV_DOUBLE:
			return 1.7E308;
	}
	return 0.0;
}



double SVGetDataTypeRange( DWORD DataType )
{
	switch( DataType )
	{
		case SV_1BIT:
			return 2.0;

		case SV_8BIT_INT:
		case SV_SIGNED_8BIT_INT:
			return 256.0;

		case SV_16BIT_INT:
		case SV_SIGNED_16BIT_INT:
			return 65536.0;

		case SV_32BIT_INT:
		case SV_SIGNED_32BIT_INT:
			// Be careful: lose precision!
			return 4294967296.0;

		case SV_SIGNED_64BIT_INT:
			// Be careful: lose precision!
			return 18446744073709551616.0;

		case SV_FLOAT:
			return 6.8E38;

		case SV_DOUBLE:
			return 1.7E308;
			// return 3.4E308;
	}
	return 0.0;
	// ( long ) pow( 2.0, ( BYTE ) ( DataType ) )
}




void SVSimpleEncrypt( LPTSTR LPCHCode, int CodeLength, LPCTSTR LPCHKey )
{
	int keyLength = static_cast<int>(_tcslen( LPCHKey ));
	int j = 0;
	TCHAR tch;
	for( int i = 0; i < CodeLength; ++i )
	{
		tch = LPCHCode[ i ];
		// a xor b = a or b - a and b
		LPCHCode[ i ] = ( tch | LPCHKey[ j ] ) - ( tch & LPCHKey[ j ] );
		if( ++j == keyLength ) 
			j = 0;
	}
}


// TStrPathName must be the full pathname of a file
// or a full pathname of a directory with a slash on the end!
// e.g.: c:/dir1/dir2/dir3/filename.ext
//       c:/dir1/dir2/dir3/
// strlen of TStrPathName must not exceed _MAX_PATH length!
// If the drive and the directory exists, the function returns
// TRUE. 
// If the directory not exits and BCreateIfNotExists is TRUE,
// the return value is TRUE if the directory is creatible.
//
// In all other cases it returns FALSE.
BOOL SVCheckPathDir( LPCTSTR TStrPathName, BOOL BCreateIfNotExists )
{
	TCHAR  curPath[ _MAX_PATH ];
	// Save current settings...
	if( _tgetcwd( curPath, _MAX_PATH ) == NULL )
		curPath[ 0 ] = _TCHAR( '/0' );

	// check path
	TCHAR  path[ _MAX_PATH ];
	TCHAR* pLast = ( TCHAR* ) TStrPathName;
	TCHAR  drive[ _MAX_DRIVE ];
	TCHAR  dir[ _MAX_DIR ];
	TCHAR  fname[ _MAX_FNAME ];  
	TCHAR  ext[ _MAX_EXT ];
	if( TStrPathName && _tcslen( TStrPathName ) < _MAX_PATH )
	{
		_tsplitpath( pLast, drive, dir, fname, ext );
		if( _tcslen( drive ) == 0 && _tcschr( pLast, _TCHAR( ':' ) ) != NULL )
		{
			// Restore settings...
			_tchdir( curPath );
			return FALSE;
		}

		while( pLast = _tcspbrk( pLast, _T( "\\/" ) ) )
		{
			_tcsncpy( path, TStrPathName, pLast++ - TStrPathName + 1 );
			path[ pLast - TStrPathName ] = _TCHAR( '\0' );

			if( _tchdir( path ) != 0 )
			{
				// Check for creation...
				if( ! BCreateIfNotExists )
				{
					// Restore settings...
					_tchdir( curPath );
					return FALSE;
				}

				// We have to make a new directory
				if( _tmkdir( path ) != 0 || _tchdir( path ) != 0 )
				{
					// Restore settings...
					_tchdir( curPath );
					return FALSE;
				}
			}
		}

		// Restore settings...
		_tchdir( curPath );
		return TRUE;
	}

	// Restore settings...
	_tchdir( curPath );
	return FALSE;
}



BOOL SVDeleteFiles( LPCTSTR TStrPathName, BOOL BIncludeSubDirectories )
{
	// TStrPathName must be the full pathname of a file,
	// that also includes * for the filename or the file extension!
	// e.g.: c:/dir1/dir2/dir3/filename.ext
	//       c:/dir1/dir2/*.*
	// strlen of TStrPathName must not exceed _MAX_PATH length!

	// If all deleting is done well, the function returns
	// TRUE. 
	// In all other cases it returns FALSE.

	TCHAR  path[ _MAX_PATH ];
	TCHAR  drive[ _MAX_DRIVE ];
	TCHAR  dir[ _MAX_DIR ];
	TCHAR  fname[ _MAX_FNAME ];  
	TCHAR  ext[ _MAX_EXT ];
	if( TStrPathName && _tcslen( TStrPathName ) < _MAX_PATH )
	{
		_tsplitpath( TStrPathName, drive, dir, fname, ext );

		// Search for file(s)...
		WIN32_FIND_DATA findData;
		BOOL BRetValue = TRUE;
		HANDLE hFindFile = FindFirstFile( TStrPathName, &findData );
		do
		{
			if( hFindFile )
			{
				_tmakepath( path, drive, dir, findData.cFileName, _T( "" ) );
				if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if( _tcscmp( findData.cFileName, _T( "." ) ) && _tcscmp( findData.cFileName, _T( ".." ) ) &&
						BIncludeSubDirectories 
					  )
					{
						// Delete sub directory contents...
						_tcscat( path, _T( "\\*.*" ) );
						BRetValue = SVDeleteFiles( path, BIncludeSubDirectories ) && BRetValue;

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
						BRetValue = _trmdir( path ) && BRetValue;
					}
				}
				else
				{
					// Delete file...
					BRetValue = DeleteFile( path ) && BRetValue;
				}
			}
		} while( FindNextFile( hFindFile, &findData ) );

		FindClose( hFindFile );
		return BRetValue;
	}

	return FALSE;
}


BOOL SVGetAbsoluteFilePathName( CString& RStrOutputPath, LPCTSTR TStrAbsoluteInputPath, LPCTSTR TStrRelativeFileInputPath )
{
	// Only for MFC-Threads!

	// TStrAbsoluteInputPath must be the full and absolute pathname 
	// of a file or directory!
	// e.g.: c:\dir1\dir2\dir3\filename.ext
	//       c:\dir1\dir2\
	// strlen of TStrAbsoluteInputPath must not exceed _MAX_PATH length!

	// TStrRelativeFileInputPath could be any relative path of an existing
	// file on the same drive as TStrAbsoluteInputPath! 
	// e.g.: ..\..\dir2\dir3\filename.ext
	// strlen of TStrAbsoluteInputPath plus strlen of TStrRelativeFileInputPath
	// must not exceed 2 * _MAX_PATH length!

	// RStrOutputPath receives the MERGED absolute path name of the file, 
	// if the function is successfully. Otherwise NO changes are made!

	// If successful the function returns TRUE.
	// That includes path length and path name checking,
	// and file exist checking!
	//
	// In all other cases it returns FALSE.
	

	if( TStrAbsoluteInputPath && 
		_tcslen( TStrAbsoluteInputPath ) < _MAX_PATH && 
		TStrRelativeFileInputPath 
	  )
	{
		TCHAR  tStrPathProcess[ 2 * _MAX_PATH ];
		TCHAR  tStrPathOut[ _MAX_PATH ];
		TCHAR* tStrFilePart;
		TCHAR  drive[ _MAX_DRIVE ];
		TCHAR  dir[ _MAX_DIR ];
		TCHAR  fname[ _MAX_FNAME ];  
		TCHAR  ext[ _MAX_EXT ];
		_tsplitpath( TStrAbsoluteInputPath, drive, dir, fname, ext );

		// Cut any filename of absolute path away...
		_tcscpy( tStrPathProcess, drive );
		_tcscat( tStrPathProcess, dir );

		if( _tcslen( tStrPathProcess ) + _tcslen( TStrRelativeFileInputPath ) < 2 * _MAX_PATH )
		{
			// Add relative file path...
			_tcscat( tStrPathProcess, TStrRelativeFileInputPath );
			tStrPathOut[ 0 ] = _TCHAR( '\0' );
			if( GetFullPathName( tStrPathProcess, _MAX_PATH, tStrPathOut, &tStrFilePart ) == _tcslen( tStrPathOut ) )
			{
				if( SVFileExists( tStrPathOut ) )
				{
					// Success...
					RStrOutputPath = tStrPathOut;
					return TRUE;
				}
			} 
		}
	}
	return FALSE;
}




BOOL SVGetRelativeFilePathName( CString& RStrOutputPath, LPCTSTR TStrAbsoluteInputPath, LPCTSTR TStrAbsoluteFileInputPath )
{
	// Only for MFC-Threads!

	// TStrAbsoluteInputPath must be the full and absolute pathname 
	// of a file or directory!
	// e.g.: c:\dir1\dir2\dir3\filename.ext
	//       c:\dir1\dir2\
	// strlen of TStrAbsoluteInputPath must not exceed _MAX_PATH length!

	// TStrAbsoluteFileInputPath must be the full and absolute pathname 
	// of an existing file on the same drive as TStrAbsoluteInputPath!
	// e.g.: c:\dir1\dir2\dir4\filename.ext
	// strlen of TStrAbsoluteFileInputPath must not exceed _MAX_PATH length!

	// RStrOutputPath receives the relative path name of the file, 
	// that means TStrAbsoluteFileInputPath relative to TStrAbsoluteInputPath,
	// if the function is successfully. Otherwise NO changes are made!

	// If successful the function returns TRUE.
	// That includes path length and path name checking,
	// and file exist checking!
	//
	// In all other cases it returns FALSE.
	

	if( TStrAbsoluteInputPath && 
		_tcslen( TStrAbsoluteInputPath ) < _MAX_PATH && 
		TStrAbsoluteFileInputPath &&
		_tcslen( TStrAbsoluteFileInputPath ) < _MAX_PATH
	  )
	{
		TCHAR  tStrPathOut[ _MAX_PATH ];
		TCHAR* tStrFilePart;

		TCHAR  tStrPathProcess[ _MAX_PATH ];
		TCHAR  drive[ _MAX_DRIVE ];
		TCHAR  dir[ _MAX_DIR ];
		TCHAR  fname[ _MAX_FNAME ];  
		TCHAR  ext[ _MAX_EXT ];
		_tsplitpath( TStrAbsoluteInputPath, drive, dir, fname, ext );

		TCHAR  tStrPathProcess2[ _MAX_PATH ];
		TCHAR  drive2[ _MAX_DRIVE ];
		TCHAR  dir2[ _MAX_DIR ];
		TCHAR  fname2[ _MAX_FNAME ];  
		TCHAR  ext2[ _MAX_EXT ];
		_tsplitpath( TStrAbsoluteFileInputPath, drive2, dir2, fname2, ext2 );

		// Check for same drive...
		if( ! _tcsicmp( drive, drive2 ) && _tcslen( drive ) != 0 )
		{
			// Qualify absolute path...
			_tcscpy( tStrPathProcess, drive );
			_tcscat( tStrPathProcess, dir );
			_tcscat( tStrPathProcess, _T( "X" ) );
			tStrPathOut[ 0 ] = _TCHAR( '\0' );
			if( GetFullPathName( tStrPathProcess, _MAX_PATH, tStrPathOut, &tStrFilePart ) == _tcslen( tStrPathOut ) )
			{
				// Cut dummy filename...
				*tStrFilePart = _TCHAR( '\0' );
				// Now we have a regular path ( including drive )...
				_tcscpy( tStrPathProcess, tStrPathOut );

				// Qualify absolute file path...
				if( GetFullPathName( TStrAbsoluteFileInputPath, _MAX_PATH, tStrPathOut, &tStrFilePart ) == _tcslen( tStrPathOut ) )
				{
					// Now we have a regular file path ( including drive )...
					_tcscpy( tStrPathProcess2, tStrPathOut );
					if( SVFileExists( tStrPathProcess2 ) )
					{
						// And the file exits...
						// Create now the relative file path...
						int count    = 0;
						int oldCount = 0;
						TCHAR* pIter;
						tStrPathOut[ 0 ] = _TCHAR( '\0' );
						BOOL bFinished = FALSE;
						while( ! bFinished )
						{
							if( ! ( pIter = _tcschr( &tStrPathProcess[ count ], _TCHAR( '\\' ) ) ) )
							{
								// No more sub directories in absolute path...
								bFinished = TRUE;
							}
							else
							{
								// Check for end of absolute file path...
								if( ! _tcschr( &tStrPathProcess2[ count ], _TCHAR( '\\' ) ) )
								{
									// No more directories in absolute file path...

									// Search for directory depth of absolute path...
									TCHAR* pIter2;
									int index = 0;
									do
									{
										pIter2 = _tcschr( &tStrPathProcess[ count + index ], _TCHAR( '\\' ) );
										if( pIter2 )
										{
											index = static_cast<int>(pIter2 - &tStrPathProcess[ count ]) + 1;
											// Append go backward...
											_tcscat( tStrPathOut, _T( "..\\" ) );
										}
									} while( pIter2 );

									bFinished = TRUE;
								}
								else
								{
									oldCount = count;
									// Move to the next valid position...
									count = static_cast<int>(pIter - tStrPathProcess) + 1;

									if( _tcsnicmp( tStrPathProcess, tStrPathProcess2, count ) )
									{
										// Different path at this position...

										// Append go backward...
										_tcscat( tStrPathOut, _T( "..\\" ) );
										// Search for directory depth...
										TCHAR* pIter2;
										int index = 0;
										do
										{
											pIter2 = _tcschr( &tStrPathProcess[ count + index ], _TCHAR( '\\' ) );
											if( pIter2 )
											{
												index = static_cast<int>(pIter2 - &tStrPathProcess[ count ]) + 1;
												// Append go backward...
												_tcscat( tStrPathOut, _T( "..\\" ) );
											}
										} while( pIter2 );

										count = oldCount;
									
										bFinished = TRUE;
									}

								}
					
							}
							
						}

						// Append necessary part of file path...
						_tcscat( tStrPathOut, &tStrPathProcess2[ count ] );

						RStrOutputPath = tStrPathOut;
						return TRUE;
					}
				}
			} 
		}
	}
	return FALSE;
}


			
BOOL SVFileExists( LPCTSTR TStrFilePath )
{
	if( TStrFilePath )
	{
		WIN32_FIND_DATA findData;
		HANDLE hFindFile = FindFirstFile( TStrFilePath, &findData );
		if( hFindFile != INVALID_HANDLE_VALUE )
		{
			// Success...
			FindClose( hFindFile );
			return TRUE;
		}
	}
	return FALSE;
}



BOOL SVGetPathInformation( CString& RStrOutput, LPCTSTR TStrFileInputPath, DWORD DwMask )
{
	// Only for MFC-Threads!

	// TStrFileInputPath should include a file name!
	// e.g.: c:\dir1\dir2\dir3\filename.ext
	//		 ..\filename.ext
	//	     filename
	// strlen of TStrFileInputPath must not exceed _MAX_PATH length!

	// RStrOutput only receives requested information!

	// If successful the function returns TRUE.
	// In all other cases it returns FALSE.

	if( TStrFileInputPath && _tcslen( TStrFileInputPath ) < _MAX_PATH )
	{
		TCHAR  tStrPathOut[ _MAX_PATH ];
		TCHAR  drive[ _MAX_DRIVE ];
		TCHAR  dir[ _MAX_DIR ];
		TCHAR  fname[ _MAX_FNAME ];  
		TCHAR  ext[ _MAX_EXT ];
		_tsplitpath( TStrFileInputPath, drive, dir, fname, ext );
		tStrPathOut[ 0 ] = _TCHAR( '\0' );

		if( ( DwMask & ( ( DWORD ) SVDRIVE ) ) == ( ( DWORD ) SVDRIVE ) )
			_tcscat( tStrPathOut, drive );

		if( ( DwMask & ( ( DWORD ) SVDIR ) ) == ( ( DWORD ) SVDIR ) )
			_tcscat( tStrPathOut, dir );

		if( ( DwMask & ( ( DWORD ) SVFILE ) ) == ( ( DWORD ) SVFILE ) )
			_tcscat( tStrPathOut, fname );

		if( ( DwMask & ( ( DWORD ) SVEXT ) ) == ( ( DWORD ) SVEXT ) )
			_tcscat( tStrPathOut, ext );

		RStrOutput = tStrPathOut;
		return TRUE;
	}
	return FALSE;
}


BOOL SVWaitXTimesForSingleObject( HANDLE HWaitObject, DWORD DwWaitTime, int XTimes )
{
	BOOL BRetVal = ( XTimes > 0 );

	while( XTimes-- > 0 )
		if( WaitForSingleObject( HWaitObject, DwWaitTime ) == WAIT_OBJECT_0 )
			ResetEvent( HWaitObject );
		else
			BRetVal = FALSE;

	return BRetVal;
}

BOOL SVSelectListCtrlItem( CListCtrl& RListControl, int Item )
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	
	if( Item >= 0 && Item < RListControl.GetItemCount() ) 
		RListControl.GetItemRect( Item, &rect, LVIR_LABEL );
	
	DWORD lParam = (DWORD) rect.top;
	lParam *= 256 * 256;
	lParam += ( DWORD ) rect.left;

	// Catch focus...
//	HWND hOldFocusOwner = ::SetFocus( RListControl.m_hWnd );
	CWnd* pOldFocusOwner = RListControl.SetFocus();

	// Select list ctrl item...( via mouse click! )
	BOOL BRetVal = ( RListControl.SendMessage( WM_LBUTTONDOWN, MK_LBUTTON, lParam ) == 0 );
	
	// Recover old focus state...
	if( /*hOldFocusOwner*/ pOldFocusOwner )
//		::SetFocus( hOldFocusOwner );
		pOldFocusOwner->SetFocus();

	return BRetVal;
}

BOOL SVGetVersionString( CString& RSTRCurrentVersion, DWORD dwVersion )
{
	int subVersion   = ( int ) LOBYTE( LOWORD( dwVersion ) );
	CString strBeta;

	//
	// 0x000300ff is v3.00 release build.
	//
	if( subVersion != 0  && subVersion != 0xff )
	{
		strBeta.Format( _T( " Beta %d" ), subVersion );
	}

#ifdef _DEBUG999
	//	__DATE__ or __TIMESTAMP__
#pragma message(__TIMESTAMP__)
	CString s;
	s.Format(_T(" [%s]"),__TIMESTAMP__);
	strBeta += s;
#endif //_DEBUG

	RSTRCurrentVersion.Format( _T( "Version %u.%2.2u%s" ), 
	                            LOBYTE( HIWORD( dwVersion ) ), 
	                            HIBYTE( LOWORD( dwVersion ) ), 
	                            strBeta );

#ifdef _DEBUG
	RSTRCurrentVersion += _T("d");        // For debug builds.
#endif
	return TRUE;
}

/*---- 01 Nov 1999 - frb.
int	sv_strncpy (char		*destString,
				const char	*sourceString,
				int			nbrOfBytes)
{
	strncpy (destString, sourceString, nbrOfBytes);

	destString [nbrOfBytes - 1] = 0;

	return 0;
}
-------*/


/*------------ 09 Dec 1999 - obsolete - frb.
HBITMAP SVMilBufferToBitmap( MIL_ID MilBufferID ) 
{
    if( MilBufferID == M_NULL )
        return NULL;

    // Try to get device context of mil buffer...
	MbufControl( MilBufferID, M_WINDOW_DC_ALLOC, M_DEFAULT );
	HDC hDC			  = ( HDC ) MbufInquire( MilBufferID, M_WINDOW_DC, M_NULL );
    HDC hMemDC		  = NULL;
	HBITMAP hResultBM = NULL;
    if( hDC != M_NULL )
	{

        if( hMemDC = ::CreateCompatibleDC( hDC ) )
        {
			HBITMAP hOrgBitmap = ( HBITMAP ) ::GetCurrentObject( hDC, OBJ_BITMAP );

			// Get bitmap info...
			BITMAP bmInfo;
			::GetObject( hOrgBitmap, sizeof( BITMAP ), &bmInfo );

			HBITMAP hReplaceBM = ::CreateCompatibleBitmap( hDC, bmInfo.bmWidth, bmInfo.bmHeight );
			HBITMAP hOldBM = ( HBITMAP ) ::SelectObject( hMemDC, hReplaceBM );

            // Copy source bitmap into memDC...
            ::SetStretchBltMode( hDC, COLORONCOLOR );
            ::StretchBlt( hMemDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY );

			// Replace bitmap and kill memDC...
			hResultBM = ( HBITMAP ) ::SelectObject( hMemDC, hOldBM );
            ::DeleteDC( hMemDC );
		}
         
		// Release mil buffer dc...
        MbufControl( MilBufferID, M_WINDOW_DC_FREE, M_DEFAULT );

	}

	return hResultBM;
}
--------------*/


int SVCompareNoCase( LPCTSTR TStrString1, LPCTSTR TStrString2 )
{
	if( ! TStrString1 )
	{
		if( ! TStrString2 )
			return 0;
		else
			return -1;
	}
	
	if( ! TStrString2 )
		return 1;
	
	size_t count = __max( _tcslen( TStrString1 ), _tcslen( TStrString2 ) );
	return _tcsnicmp( TStrString1, TStrString2, count );
}



// Escapes Special Characters in CStrings 
bool SVAddEscapeSpecialCharacters( CString& RString, bool bConvertCtrl )
{
	bool bAdded = false;

	IntStrPairMap FoundStrings;

	//// Find Special Characters
	int len = RString.GetLength();
	for( int index = 0 ; index < len ; index++ )
	{
		// find the single character....
		TBYTE l_ch = RString[index]; 
		if( l_ch == _T('\\'))
		{
			FoundStrings[index] = StrStrPair(_T("\\\\"),_T("\\"));
			bAdded = true;
		}
		else
		if( l_ch == _T('"'))
		{
			FoundStrings[index] = StrStrPair(_T("\\\""),_T("\""));
			bAdded = true;
		}
		else
		if( bConvertCtrl && iscntrl(l_ch) )
		{
			CString first;
			CString second;
			first.Format( _T("\\%03o"),l_ch);
			second.Format( _T("%c"), l_ch);
			FoundStrings[index] = StrStrPair(first, second);
			bAdded = true;
		}
	}

	// Process List
	if( bAdded )
	{
		CString NewString;
		int start = 0;
		int count;

		for( IntStrPairMap::iterator it = FoundStrings.begin() ; it != FoundStrings.end(); ++it)
		{
			count = it->first - start;
			if( count > 0 )
			{
				NewString += RString.Mid( start, count); // copy up to the found character.
			}
			// Expanded character added...
			NewString += it->second.first;

			start = it->first + it->second.second.GetLength();
		}
		NewString += RString.Mid( start );
		RString = NewString;
	}
	return bAdded;
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes Escaped Double Quotes in CStrings.
//
//
bool SVRemoveEscapedSpecialCharacters( CString& RString, bool bConvertCtrl )
{
	bool bRemoved = false;

	IntStrPairMap FoundStrings;


	//// Find Special Characters
	int len = RString.GetLength()-1;
	int index = 0;

	while( ( index = RString.Find(_T('\\'), index ))!= -1)
	{
		if( RString[index+1] == _T('\\') )
		{
			FoundStrings[index] = StrStrPair(_T("\\\\"), _T("\\"));
			index++;
		}
		else
		if( RString[index+1] == _T('"'))
		{
			FoundStrings[index] = StrStrPair(_T("\\\""), _T("\""));
			index++;
		}
		else
		if( bConvertCtrl && index < len - 2 && 
			isdigit(RString[index + 1]) &&
			isdigit(RString[index + 2]) &&
			isdigit(RString[index + 3]) )
		{
			CString first;
			CString second;
			int ctrlValue = (RString[index+1]-_T('0'))*64  + (RString[index+2]-_T('0'))*8 + (RString[index+3]-_T('0'));
			first.Format( _T("\\%03o"),ctrlValue);
			second.Format(_T("%c"), ctrlValue);
			FoundStrings[index] = StrStrPair(first, second);
			index += 3;
		}
		index++;
	}

	// Process Found list.
	if( FoundStrings.size() > 0 )
	{
		CString NewString;
		
		int start = 0;
		for( IntStrPairMap::iterator it = FoundStrings.begin() ; it != FoundStrings.end() ; ++it )
		{
			int count = it->first - start;
			NewString += RString.Mid( start, count );
			// Insert "Single Character"
			NewString += it->second.second;	
			start = it->first + it->second.first.GetLength();
		}
		NewString += RString.Mid( start );
		RString = NewString;
		bRemoved = true;

	}
	return bRemoved;
}


// Convert Hex Data to a Hex Dumop String
// Len of the data is first in the String
void SVConvertToHexString( DWORD len, LPBYTE buff, CString& hexString )
{
	CString tmpStr;

	// put len in string first
	hexString.Format( _T( "0x%08x" ), len );

	for( DWORD i = 0; i < len; i++ )
	{
		tmpStr.Format( _T( ",0x%02x" ), buff[i] );
		hexString += tmpStr;
	}
}

// Convert Hex Dump String to hex binary data
// Sets len of the hex binary data
// Note: Allocates buffer so caller must delete it
BOOL SVConvertFromHexString( DWORD &len, LPBYTE *buff, CString& hexString )
{
	CString tmpStr;
	*buff = NULL;
	len = 0;

	// get length first
	int index = hexString.Find( _T( "," ) );

	// index should be at position 10
	if( index == 10 )
	{
		tmpStr = hexString.Left( index );

		// Convert to hex
		_stscanf_s( tmpStr, _T( "%x" ), &len );

		// Allocate buffer
		*buff = new BYTE[ len ];
		memset( *buff, '\0', len );

		int startIndex = index + 1;
		int l_iSize = hexString.GetLength();

		for( DWORD i = 0;i < len;i++ )
		{
			index = -1;
			
			if ( startIndex < l_iSize )
			{
			index = hexString.Find( _T( "," ), startIndex );
			}

			if (index != -1 )
			{
				// get Hex BYTE String Data
				tmpStr = hexString.Mid( startIndex, ( index - startIndex ) );

				// convert to hex BYTE
				BYTE hexByte;
				DWORD value;
				_stscanf_s( tmpStr, _T( "%x" ), &value );
				hexByte = static_cast<unsigned char>(value);

				(*buff)[i] = hexByte;

				// set next index
				startIndex = index + 1;
			}
			else
				break;
		}
		return TRUE;
	}
	return FALSE;
}




////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFindMenuByCommand
// -----------------------------------------------------------------------------
// .Description : Returns 'parent' menu and position of searched menu, if any. 
//				: Otherwise NULL.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :22.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
CMenu* SVFindMenuByCommand( CMenu *pMenu, int nID, BOOL bIncludeSubMenues, int& rMenuPos )
{
	CMenu* pRetMenu = NULL;
	if( pMenu )
	{
		int count = pMenu->GetMenuItemCount();
		for( int i = 0; i < count && ! pRetMenu; ++ i )
		{
			if( (unsigned int)(nID) != pMenu->GetMenuItemID( i ) )
			{
				if( bIncludeSubMenues )
				{
					// Check Sub Menu...
					CMenu* pSubMenu = pMenu->GetSubMenu( i );
					pRetMenu = ::SVFindMenuByCommand( pSubMenu, nID, bIncludeSubMenues, rMenuPos );
				}
			}
			else
			{
				// Bingo...
				pRetMenu = pMenu;
				rMenuPos = i;
			}
		}
	}
	return pRetMenu;
}



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFindMenuByName
// -----------------------------------------------------------------------------
// .Description : Returns 'parent' menu and position of searched menu, if any.
//				: Otherwise NULL.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :22.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
CMenu* SVFindMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues, int& rMenuPos )
{
	CMenu* pRetMenu = NULL;
	if( pMenu && szMenuString )
	{
		int count = pMenu->GetMenuItemCount();
		for( int i = 0; i < count && ! pRetMenu; ++ i )
		{
			CString strMenu;
			pMenu->GetMenuString( i, strMenu, MF_BYPOSITION );
			if( strMenu.CompareNoCase( szMenuString ) )
			{
				// Not found...

				if( bIncludeSubMenues )
				{
					// Check Sub Menu...
					CMenu* pSubMenu = pMenu->GetSubMenu( i );
					pRetMenu = ::SVFindMenuByName( pSubMenu, szMenuString, bIncludeSubMenues, rMenuPos );
				}
			}
			else
			{
				// Bingo...
				pRetMenu = pMenu;
				rMenuPos = i;
			}
		}
	}
	return pRetMenu;
}

 
////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFindSubMenuByName
// -----------------------------------------------------------------------------
// .Description : Returns pointer to sub menu which is searched for, if any.
//				: Otherwise NULL.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :22.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
CMenu* SVFindSubMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues )
{
	CMenu* pRetMenu = NULL;
	if( pMenu && szMenuString )
	{
		int count = pMenu->GetMenuItemCount();
		for( int i = 0; i < count && ! pRetMenu; ++ i )
		{
			CString strMenu;
			pMenu->GetMenuString( i, strMenu, MF_BYPOSITION );
			if( strMenu.CompareNoCase( szMenuString ) )
			{
				// Not found...

				if( bIncludeSubMenues )
				{
					// Check Sub Menu...
					CMenu* pSubMenu = pMenu->GetSubMenu( i );
					pRetMenu = ::SVFindSubMenuByName( pSubMenu, szMenuString, bIncludeSubMenues );
				}
			}
			else
			{
				// Bingo...
				pRetMenu = pMenu->GetSubMenu( i );
			}
		}
	}
	return pRetMenu;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCartesianToPolar
// -----------------------------------------------------------------------------
// .Description : Returns radius and angle of converted point.
//				: Angle is given back in Degree ( 0.0...360.0 )
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVCartesianToPolar( const POINT& RPoint, double& RRadius, double& RAngle )
{
	double radGrad = 3.14159265 / 180.0;
	double radius = RPoint.x * RPoint.x + RPoint.y * RPoint.y;
	RRadius = ( radius > 0.0 ) ? sqrt( radius ) : 0;
	// gives back -180.0...180.0
	RAngle = atan2( (double)-RPoint.y, (double)RPoint.x ) / radGrad;
	// Convert to 0...360
	if( RAngle < 0.0 )
	{
		RAngle = 360.0 + RAngle;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPolarToCartesian
// -----------------------------------------------------------------------------
// .Description : Returns POINT defined by radius and angle.
//				: Angle must be given in Degree ( 0.0...360.0 )
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVPolarToCartesian( double Radius, double Angle, POINT& RPoint )
{
	double radGrad = 3.14159265 / 180.0;
	double ang = Angle * radGrad;
	RPoint.x = (long)(Radius * cos( ang ));
	RPoint.y = (long)(- Radius * sin( ang ));
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPolarToCartesian
// -----------------------------------------------------------------------------
// .Description : Returns POINT defined by radius and angle.
//				: Angle must be given in Degree ( 0.0...360.0 )
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
POINT SVPolarToCartesian( double Radius, double Angle )
{
	double radGrad = 3.14159265 / 180.0;
	double ang = Angle * radGrad;
	POINT retPoint;
	retPoint.x = (long)(Radius * cos( ang ));
	retPoint.y = (long)(- Radius * sin( ang ));
	return retPoint;
}


LPCTSTR SVRCGetSVCPathName()
{
	DWORD dwType, dwSize, dwResult;
	HKEY hKey;

	dwType = 0L;
	dwSize = sizeof (SVRCCurrentSVCPathName);
	memset (SVRCCurrentSVCPathName, 0, sizeof (SVRCCurrentSVCPathName));

	dwResult = RegOpenKeyEx (HKEY_CURRENT_USER, 
                            _T( "Software\\SVR Gesellschaft für Bildverarbeitung mbH\\SVObserver\\Settings" ),
                             0L, KEY_QUERY_VALUE, &hKey);
	if (dwResult == ERROR_SUCCESS)
	{
		RegQueryValueEx (hKey, _T("CurrentSVCPathName"), (DWORD *) NULL, &dwType, (BYTE *) SVRCCurrentSVCPathName, &dwSize);
		RegCloseKey (hKey);
	}
	else
	{
#ifdef _DEBUG
		char *pszBuff;
		pszBuff = NULL;
		FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					   NULL, dwResult, 0, (char *) &pszBuff, 0, NULL);
		ASSERT( false );// pszBuff
		LocalFree (pszBuff);
#endif
	}
	return SVRCCurrentSVCPathName;
}


void SVRCSetSVCPathName( LPCTSTR TStrPathName )
{
//	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if( TStrPathName )
	{
		if( _tcslen( TStrPathName ) < _MAX_PATH )
			_tcscpy( SVRCCurrentSVCPathName, TStrPathName );
		else
		{
			memcpy( SVRCCurrentSVCPathName, TStrPathName, _MAX_PATH );
			SVRCCurrentSVCPathName[ _MAX_PATH - 1 ] = '\0';
		}
	}
	AfxGetApp()->WriteProfileString( _T( "Settings" ), _T( "CurrentSVCPathName" ), SVRCCurrentSVCPathName );
}



#define INDEX_FOUND  -2

//************************************
// Method:    _FindMenuIndex
// Description:  Function is only used in EnableParentMenu
// Parameter: CMenu *  & pParent pointer to parent menu (output)
// Parameter: int &ParentMenuIndex index of the parenmenuentry
// Parameter: CMenu * pMenu 
// Parameter: UINT ID CommandId 
// Parameter: int  start index of submenu where search for the command entry is started.
// Returns:   int index of Entry, 
//************************************
int   _FindMenuIndex( CMenu*  &pParent,  int&  ParentMenuIndex, CMenu*  pMenu, UINT ID , int start =0)
{
	for(int i =start ; i < pMenu->GetMenuItemCount(); i++)
	{
		if( pMenu->GetMenuItemID(i) == ID)
		{
			return i;
		}
		else
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);

			if(pSubMenu)
			{
				int index = _FindMenuIndex(pParent,ParentMenuIndex, pSubMenu, ID);

				if(index == INDEX_FOUND)
				{
					return INDEX_FOUND;
				}

				if(index > -1)
				{
					ParentMenuIndex = i;
					pParent =  pMenu;
					return INDEX_FOUND;
				}
			}
		}

	}
	return -1;
}

bool EnableParentMenu(CMenu* pMenu, UINT ID, bool Enable, int start )
{
	UINT flag = Enable?  MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION |  MF_GRAYED;
	CMenu* pParentMenu =nullptr;
	int ParentMenuIndex =-1;
	if( _FindMenuIndex( pParentMenu, ParentMenuIndex, pMenu, ID , start ) == INDEX_FOUND)
	{
		if(pParentMenu )
		{
			return (-1 != pParentMenu->EnableMenuItem(ParentMenuIndex, flag ));
		}
	}
	return false;
}


//** EOF **

