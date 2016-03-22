//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFileLoader
//* .File Name       : $Workfile:   SVImageFileLoader.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <iterator>
#include "SVImageFileLoader.h"
#include "SVFileSystemLibrary/SVFileInfo.h"
#include "SVFileSystemLibrary/SVFileInfoComparator.h"
#include "SVFileSystemLibrary/SVFileSystemScanner.h"
#pragma endregion Includes

HRESULT SVImageFileLoader::Load(LPCTSTR FileName, SVImageFile& rImageFile)
{
	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE | LR_SHARED);
	rImageFile = hBitmap;
	return ((hBitmap) ? S_OK : S_FALSE);
}

HRESULT SVImageFileLoader::LoadFirstFile( LPCTSTR Name, LPCTSTR Ext, SVImageFile& rImageFile )
{
	SVString FileName;

	if( FILE_ATTRIBUTE_DIRECTORY == ( ::GetFileAttributes(Name) & FILE_ATTRIBUTE_DIRECTORY)  )
	{
		typedef std::set<SVFileInfo, SVFileInfoComparator> SVFileList;
		typedef SVFileList::const_iterator FileListIterator;
		typedef std::insert_iterator<SVFileList> Insertor;

		SVFileList fileList;
		SVFileSystemScanner<Insertor>::ScanForFiles( Name, Ext, Insertor(fileList, fileList.end()));
		FileListIterator it = fileList.begin();
		if (it != fileList.end())
		{
			FileName = it->filename;
		}
	}
	else
	{
		FileName = Name;
	}

	return Load( FileName.c_str(), rImageFile );
}
