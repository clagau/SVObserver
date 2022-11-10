//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageFileLoader
//* .File Name       : $Workfile:   SVImageFileLoader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:22  $
//******************************************************************************
#pragma  once

#pragma region Includes
#pragma endregion Includes
#include "SVFileSystemLibrary/ImageFileFormats.h"

class SVImageFile;

class SVImageFileLoader
{
public:
	//************************************
	//! The method loads an image file 
	//! \param FileName <in> The name of the image file
	//! \param rImageFile <out> The image file information
	//! \returns S_OK when successful
	//************************************
	static HRESULT LoadImageFile( LPCTSTR FileName, SVImageFile& rImageFile );

	//************************************
	//! The method loads the first image file
	//! \param Name <in> The name of the image file or a directory containing the files
	//! \param fileFormat <in> The file format to consider for the first file
	//! \param rImageFile <out> The image file information
	//! \returns S_OK when successful
	//************************************
	static HRESULT LoadFirstFile(LPCTSTR Name, ImageFileFormat fileFormat, SVImageFile& rImageFile);
};

