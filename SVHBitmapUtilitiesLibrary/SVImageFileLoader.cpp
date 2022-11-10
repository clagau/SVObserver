//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVImageFileLoader.h"
#include "SVImageFile.h"
#include "SVFileSystemLibrary/FilepathUtilities.h"
#pragma endregion Includes

HRESULT SVImageFileLoader::LoadImageFile(LPCTSTR FileName, SVImageFile& rImageFile)
{
	HBITMAP hBitmap = (HBITMAP)::LoadImage(nullptr, FileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE | LR_SHARED);
	rImageFile = hBitmap;
	return ((hBitmap) ? S_OK : S_FALSE);
}

HRESULT SVImageFileLoader::LoadFirstFile(LPCTSTR Name, ImageFileFormat fileFormat, SVImageFile& rImageFile)
{
	std::string FileName;

	if (FILE_ATTRIBUTE_DIRECTORY == (::GetFileAttributes(Name) & FILE_ATTRIBUTE_DIRECTORY))
	{
		std::vector<std::string> fileList {getFileList(Name, fileFormat, false)};
		auto iter = fileList.begin();
		if (fileList.end() != iter)
		{
			FileName = *iter;
		}
	}
	else
	{
		FileName = Name;
	}

	return LoadImageFile(FileName.c_str(), rImageFile);
}
