//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Zip helper
//* .File Name       : $Workfile:   ZipHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 04:37:14  $
//* ----------------------------------------------------------------------------
//* This class is used to create zip files and unzip files
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvUl
{

enum CompressionFormat
{
	UnknownCompressionFormat,
	SevenZip,
	Zip,
	GZip,
	BZip2,
	Rar,
	Tar,
	Iso,
	Cab,
	Lzma,
	Lzma86
};

typedef UINT32(WINAPI* CreateObjectFunc)(const GUID* clsID, const GUID* interfaceID, void** outObject);

/// This method creates a 7 zip file
/// \param rZipFileName [in] Reference to the zip file name
/// \param rZipFiles [in] Reference to the set of zip files
/// \param rFolderPrefix [in] Reference to the folder prefix which is not inserted into the zip file
/// \param DeleteSourceFiles [in] If true deletes all the source files after successful zip file 
/// \returns true on success
bool makeZipFile( const std::string& rZipFileName, const SvDef::StringVector& rZipFiles, const std::string& rFolderPrefix, bool DeleteSourceFiles );

/// This method unzips all files to a folder
/// \param rZipFileName [in] Reference to the zip file name
/// \param rDestinationFolder [in] Reference to the destination folder
/// \param rUnzippedFiles [out] Reference to the files that have been unzipped
/// \returns true on success
bool unzipAll( const std::string& rZipFileName, const std::string& rDestinationFolder, SvDef::StringVector& rUnzippedFiles );
} // namespace SvUl
