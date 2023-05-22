//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVMatroxImageFile.h
/// All Rights Reserved 
//*****************************************************************************
/// Class holds an imagebuffer for an image read from file 
//******************************************************************************

#pragma once

#include <FilesystemUtilities/ImageFileFormats.h>
class SVMatroxImageFile
{
public:
	SVMatroxImageFile();
	~SVMatroxImageFile();
	SIZE GetSize();
	MIL_INT GetType();
	MIL_INT GetSizeBit();
	MIL_INT GetSizeBand();
	bool Load(LPCSTR filename);
	bool LoadFirstFile(LPCTSTR Name, ImageFileFormat fileFormat);
	std::string GetDebugString();
	bool IsColor();

private:
	MIL_ID m_ImageBuffer {M_NULL};
	
	std::string m_Filename;


};

