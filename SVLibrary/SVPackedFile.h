// ******************************************************************************
// * COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : modulename
// * .File Name       : $Workfile:   SVPackedFile.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Feb 2014 16:37:30  $
// ******************************************************************************
#pragma once

#define SV_PACKEDFILE_VERSION 1

#pragma region Includes
#pragma endregion Includes

class SVPackedFile 
{
public:
	SVPackedFile();

	virtual ~SVPackedFile();

	const std::string& getConfigFilePath() const { return m_configFilePath; };

	//Extracts the files contained in szPackedFile.
	bool UnPackFiles( LPCTSTR PackedFileName, LPCTSTR UnPackDir = nullptr );

private:
	std::string m_configFilePath;
};

