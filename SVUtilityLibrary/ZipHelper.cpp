//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Zip helper
//* .File Name       : $Workfile:   ZipHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 04:37:14  $
//* ----------------------------------------------------------------------------
//* This class is used to create zip files and unzip files
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ZipHelper.h"
#include "io.h"
#include "Zip.h"
#include "Unzip.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
ZipHelper::ZipHelper()
{
}

ZipHelper::~ZipHelper()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void ZipHelper::makeZipFile( const SVString& rZipFileName, const SVStringSet& rZipFiles, bool DeleteSourceFiles )
{
	if( 0 != rZipFiles.size() )
	{
		HZIP ZipHandle = ::CreateZip( rZipFileName.c_str(), NULL);
		if( NULL != ZipHandle )
		{
			SVStringSet::const_iterator Iter( rZipFiles.begin() );
			while( rZipFiles.end() != Iter )
			{
				if( ::_access( Iter->c_str(), 0 ) == 0 )
				{
					std::string FileName;
					_TCHAR Name[_MAX_FNAME];
					_TCHAR Extension[_MAX_EXT];

					_splitpath( Iter->c_str(), NULL, NULL, Name, Extension );
					FileName = Name;
					FileName += Extension;
					::ZipAdd(ZipHandle, FileName.c_str(), Iter->c_str() );
					if( DeleteSourceFiles )
					{
						::DeleteFile( Iter->c_str() );
					}
				}

				++Iter;
			}

			::CloseZipZ( ZipHandle );
		}
	}
}

void ZipHelper::unzipAll( const SVString& rZipFileName, const SVString& rDestinationFolder, SVStringSet& rUnzippedFiles )
{
	if( !rDestinationFolder.empty() )
	{
		HZIP ZipHandle = ::OpenZip( rZipFileName.c_str(), NULL );
		if ( NULL != ZipHandle )
		{
			ZIPENTRY Entry;
			::SetUnzipBaseDir( ZipHandle,  rDestinationFolder.c_str() );
			::GetZipItem( ZipHandle, -1, &Entry );
			int Count = Entry.index;
			for(int i = 0; i < Count; i++)
			{
				::GetZipItem( ZipHandle, i, &Entry );
				::UnzipItem( ZipHandle, i, Entry.name );
				SVString FileName( rDestinationFolder );
				FileName += _T("\\");
				FileName += Entry.name;
				rUnzippedFiles.insert( FileName );
			}
			::CloseZipU( ZipHandle );
		}
	}
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\ZipHelper.cpp_v  $
 * 
 *    Rev 1.0   19 Dec 2014 04:37:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
