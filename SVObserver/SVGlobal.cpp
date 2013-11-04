//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobal Function Module
//* .File Name       : $Workfile:   SVGlobal.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:19:58  $
//******************************************************************************

#include "stdafx.h"
#include <direct.h>
#include "SVGlobal.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageClass.h"
#include "SVObserver.h"
#include "SVImageProcessingClass.h"

__declspec( thread ) int tls_ObjectIndex = 1;

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
	int keyLength = _tcslen( LPCHKey );
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
			// Error, invalid drive
			//::MessageBox( NULL, "Invalid Drive Name!\n\nINVALID DRIVE", NULL, MB_ICONEXCLAMATION | MB_OK );

			// Restore settings...
			_tchdir( curPath );
			return FALSE;
		}

		while( pLast = _tcspbrk( pLast, _T( "\\/" ) ) )
		{
			_tcsncpy( path, TStrPathName, pLast++ - TStrPathName + 1 );
			path[ pLast - TStrPathName ] = _TCHAR( '\0' );
//::MessageBox( NULL, path, NULL, MB_SYSTEMMODAL | MB_ICONEXCLAMATION | MB_OK );
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
					// Error unable to create the directoy
					//::MessageBox( NULL, "Unable To Create Directory!", NULL, MB_ICONEXCLAMATION | MB_OK );

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
											index = pIter2 - &tStrPathProcess[ count ] + 1;
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
									count = pIter - tStrPathProcess + 1;

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
												index = pIter2 - &tStrPathProcess[ count ] + 1;
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

/////////////////////////////////////////////////////////////////////////////
//
// 06 Dec 1999 - frb.
// Try to solve a MIL problem with converting MIL image ID to Window HDC
// handle to a device context.
//
HBITMAP SVMilBufferToBitmap2( SVImageClass *pImage )
{
	HDC hMemDC		  = NULL;
	HBITMAP hResultBM = NULL;
	
	SVImageInfoClass ImageInfo;
	SVSmartHandlePointer ImageHandle;
	
	if ( ! pImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
	{
		return NULL;
	}
	
	ImageInfo = pImage->GetImageInfo();
	
	SVImageBufferHandleImage l_MilHandle;
	ImageHandle->GetData( l_MilHandle );

	SVMatroxBuffer MilBufferID = l_MilHandle.GetBuffer();
	
	if( MilBufferID.empty() )
	{
		return NULL;
	}
	
	//
	// Try to get device context of mil buffer...
	// The MIL image buffer attributes may be modified to add M_DIB if
	// necessary so that getting a HDC will succeed.
	//
	HDC hDC;
	// &&&
	hDC = SVImageProcessingClass::Instance().CreateBufferDC( ImageInfo, ImageHandle );
	
	//
	// Now create the bitmap.
	//
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
	
	// &&&
	HRESULT hr = SVImageProcessingClass::Instance().DestroyBufferDC( ImageHandle, hDC );
	
	return hResultBM;
}


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
	
	int count = __max( _tcslen( TStrString1 ), _tcslen( TStrString2 ) );
	return _tcsnicmp( TStrString1, TStrString2, count );
}


BOOL SVGetPLCBaseAddress( CString& RPLCBase, const CString& RItem )
{
	int index = RItem.FindOneOf( _T( ":" ) );
	if( index > 1 && index + 1 < RItem.GetLength() )
	{
		// Address must be:
		//	ax:y[,Lz]
		//	a - any letter
		//	x - base address
		//  y - offset
		//  z - Length of Block
		RPLCBase = RItem.Left( index );
		return TRUE;
	}
	return FALSE;
}


int SVGetPLCOffset( LPCTSTR TStrPLCAddress )
{
	if( TStrPLCAddress )
	{
		CString strAddress = TStrPLCAddress;
		int index = strAddress.FindOneOf( _T( ":" ) );
		if( index > 1 && index + 1 < strAddress.GetLength() )
		{
			CString strOffset;
			// Address must be:
			//	ax:y[,Lz]
			//	a - any letter
			//	x - base address
			//  y - offset
			//  z - Length of Block
			strOffset = strAddress.Mid( index + 1 );
			if( ( index = strOffset.FindOneOf( _T( "," ) ) ) >= 0 )
			{
				strOffset = strOffset.Left( index );
			}

			// Check for not valid offset: empty offset or hex offset...
			if( ! strOffset.IsEmpty() && strOffset.FindOneOf( _T( "xX" ) ) < 0 )
			{
				LPTSTR pStop   = NULL;
				int offset = _tcstol( strOffset, &pStop, 10 );
				// Check for not valid offset: not a number offset or negative offset... 
				if( pStop && *pStop == _TCHAR( '\0' ) && offset >= 0 )
					return offset;

			}
		}
	}
	return -1;
}

int SVComparePLCAddresses( LPCTSTR TStrAddress1, LPCTSTR TStrAddress2 )
{
	if( ! TStrAddress1 )
	{
		if( ! TStrAddress2 )
			return 0;
		else
			return -1;
	}

	if( ! TStrAddress2 )
		return 1;

	int offset1 = SVGetPLCOffset( TStrAddress1 );
	int offset2 = SVGetPLCOffset( TStrAddress2 );

	CString strAddress1 = TStrAddress1;
	CString strAddress2 = TStrAddress2;
	CString strBase1, strBase2;

	if( ! SVGetPLCBaseAddress( strBase1, strAddress1 ) || 
		! SVGetPLCBaseAddress( strBase2, strAddress2 ) ||
		offset1 < 0 || offset2 < 0
	  )
	{
		return SVCompareNoCase( TStrAddress1, TStrAddress2 );
	}

	int RetVal = strBase1.CompareNoCase( strBase2 );
	if( RetVal == 0 )
	{
		if( offset1 > offset2 )
			return 1;
		
		if( offset1 > offset2 )
			return 0;

		return -1;
	}

	return RetVal;
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
		AfxMessageBox( pszBuff );
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

//** EOF **

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGlobal.cpp_v  $
 * 
 *    Rev 1.1   12 Jun 2013 15:19:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.34   22 May 2013 09:18:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code (SVReadTimeStamp)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 10:43:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   18 Sep 2012 18:35:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  781
   SCR Title:  Update SVObserver Version Number for Esok Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the creation and management for the version information.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   13 Jul 2012 10:45:30   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  764
   SCR Title:  Add tool comments to all tools in SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed TCHAR used in AddEscapeSpecialCharacters to a TBYTE to handle characters above 128.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   01 Sep 2011 10:56:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  737
   SCR Title:  Fix serialization bug associated with control characters in strings
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified SVAddEscapeCharacters and SVRemoveEscapeCharacters to convert control characters.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   14 Jul 2011 09:12:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  726
   SCR Title:  Upgrade SVObserver to 5.41
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update version number to 5.41 Beta 003.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   05 Jul 2011 12:43:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  725
   SCR Title:  Upgrade SVObserver to version 5.4 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to only display one digit after the decimal point.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   25 Feb 2011 12:08:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   10 Feb 2011 14:39:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   27 Jan 2011 11:06:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   04 Nov 2010 13:35:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   16 Dec 2009 10:29:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   03 Sep 2009 10:07:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   30 Jul 2009 11:39:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branced changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   24 Jul 2007 13:06:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Upgrade Matrox Imaging Library to 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   22 Jun 2007 10:40:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   21 Jun 2007 12:52:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18.1.0   14 Jul 2009 13:27:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix data manager issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   19 Jul 2005 14:32:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     SVAddEscapeSpecialCharacters and SVRemoveEscapeSpecialCharacters now return bool indicating if the string had been modified
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   17 Feb 2005 14:23:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   08 Jul 2003 12:02:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     BOOL to HRESULT
   changed SVConvertFromHexString for new architecture
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   25 Jun 2003 09:12:38   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  371
   SCR Title:  Remove DOS Focus COM support
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the CommPort support for DOS Focus.  Moved methods from the SVCommRC project to the Globals
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   22 Apr 2003 09:59:38   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   20 Mar 2003 15:50:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated function to fix a short-cut problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   29 Jan 2003 16:08:26   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated global function SVMilBufferToBitmap2 to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   19 Nov 2002 13:11:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed System calls to SVImageProcessingClass calls
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Nov 2001 13:16:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added two new global SVCalcLineDXValue and SVCalcLineDYValue with two new overloads for those functions to calculate sub-pixel resolution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   30 Oct 2001 10:27:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  248
   SCR Title:  Notify user when loading a configuration from a newer version of SVObserver
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new function SVGetVersionString that returns a text representation of a SVObserver version.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   04 Oct 2001 14:56:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   12 Jun 2001 14:41:36   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Revised file header.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   19 Apr 2001 14:15:30   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   06 Feb 2001 13:27:00   cSchmittinger
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  cSchmittinger;  Carl Schmittinger
   Change Description:  
     Modified source code to use new file management classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4.1.0   16 May 2001 14:08:00   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  202
   SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   Mar 01 2000 12:03:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added new global functions:
   SVPolarToCartesian, SVCartesianToPolar
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Feb 24 2000 11:38:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  111
   SCR Title:  Integrate Conditional Tool Set/Tool Drawing
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added SVFindMenuByCommand, SVFindMenuByName, and SVFindSubMenuByName functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Jan 03 2000 10:20:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  73
   SCR Title:  Mask Editor Mil 5.0 to Mil 6.0 repairs.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for MIL 5.0 to MIL 6.0 transition to accomodate DIB (Device Independent Bitmap) capabilties for image masking functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:48:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  67
   SCR Title:  Version 3.01 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes for v3.00 to v3.01 versioning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 11:39:14   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Nov 18 1999 09:05:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  59
   SCR Title:  Version 3.00 Release Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Release versioning changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   Nov 02 1999 08:59:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  55
   SCR Title:  Version 3.00 Beta 16 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   Nov 01 1999 17:04:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  12
   SCR Title:  Port OCR functionallity from custom 2.xx version.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove sv_strncpy() function. Not needed by OCR anymore.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   Oct 20 1999 16:06:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  50
   SCR Title:  Version 3.00 Beta 12 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.00 Beta 11 to v3.00 Beta 12.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Oct 15 1999 13:41:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Repair buts in image saving part of archives.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   Oct 08 1999 10:54:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Versioning changes to Beta 8.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Oct 04 1999 10:11:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed CalcX/Y Functions to support 360 Degree mesurement.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Sep 24 1999 09:09:16   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  31
   SCR Title:  Integrate file manager/tracker.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Completed File Tracker.
   Added Passed-Failed-Warned-Enabled-Processed-Counter
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   Sep 20 1999 13:51:44   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  12
   SCR Title:  Port OCR functionallity from custom 2.xx version.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Take out time stamp on build - Not needed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   16 Sep 1999 11:01:42   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added two methods:
   SVConvertToHexString and
   SVConvertFromHexString
   for support of converting a serialize buffer to an Object Script
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   Sep 15 1999 16:19:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added date and time to SVObserver version string so the 'version' of the 'build' can be determined to the date and time.  Used only for the development process.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Sep 13 1999 18:34:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  12
   SCR Title:  Port OCR functionallity from custom 2.xx version.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixed routine that removes 'slashes' before double quotes for string value processing.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   10 Sep 1999 17:30:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added 2 new functions:
   SVEscapeDoubleQuotes and
   SVRemoveEscapedDoubleQuotes
   to support strings with embedded quoptes in the Object
   Script Parser.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   Aug 23 1999 10:08:40   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  22
   SCR Title:  Port Multiple Position Queue code from program version 2.4.2
   Checked in by:  Steve Jones
   Change Description:  
     Revised to support Beta Versioning.
   Added 3 new methods, SVCompareNoCase, SVGetPLCBaseAddress, and SVGetPLCOffset.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
