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
#include <set>
#include <iterator>
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
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageFileLoader.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:41:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/