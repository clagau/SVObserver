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

class ZipHelper
{
public:
#pragma region Declarations

#pragma endregion Declarations

public:
#pragma region Constructor
	//************************************
	// Description: The class constructor
	//************************************
	ZipHelper();

	//************************************
	// Description: The class destructor
	//************************************
	virtual ~ZipHelper();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	// Description: This method makes the zip file
	// Parameter: rZipFileName <in> Reference to the zip file name
	// Parameter: rZipFiles <in> Reference to the set of zip files
	// Parameter: DeleteSourceFiles <in> If true deletes the source files after zipping file
	//************************************
	static void makeZipFile( const std::string& rZipFileName, const SvDef::StringSet& rZipFiles, bool DeleteSourceFiles );

	//************************************
	// Description: This method unzips all the files
	// Parameter: rZipFileName <in> Reference to the zip file name
	// Parameter: rDestinationFolder <in> Reference to the zip file name
	// Parameter: rUnzippedFiles <out> Reference to the files that have been unzipped
	//************************************
	static void unzipAll( const std::string& rZipFileName, const std::string& rDestinationFolder, SvDef::StringSet& rUnzippedFiles );
#pragma endregion Public Methods
};

