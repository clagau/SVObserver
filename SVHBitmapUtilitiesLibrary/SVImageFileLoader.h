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

class SVImageFile;

class SVImageFileLoader
{
public:
	//************************************
	//! The method loads a bitmap file 
	//! \param FileName <in> The name of the bitmap file
	//! \param rImageFile <out> The image file information
	//! \returns S_OK when successful
	//************************************
	static HRESULT Load( LPCTSTR FileName, SVImageFile& rImageFile );

	//************************************
	//! The method loads the first bitmap file
	//! \param Name <in> The name of the bitmap file or a directory containing the files
	//! \param Ext <in> The file extension to search for the first file
	//! \param rImageFile <out> The image file information
	//! \returns S_OK when successful
	//************************************
	static HRESULT LoadFirstFile( LPCTSTR Name, LPCTSTR Ext, SVImageFile& rImageFile );
};

