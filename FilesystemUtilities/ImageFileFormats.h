///\copyright 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file ImageFileFormats.h 
/// enum and functions for Image File Formats

#pragma once


enum class ImageFileFormat : long ///< used to define which image file format(s) may be used
{
	invalid,
	bmp,
	png,
	tiff,
	mim,
	any
};


[[nodiscard]] ImageFileFormat inferredMilImageFileFormat(const TCHAR* pFilepath);
[[nodiscard]] ImageFileFormat inferredMilImageFileFormat(const std::string& filenameOrPath);
[[nodiscard]] bool filenameMatchesImageFormat(const std::string& rFilenameOrPath, ImageFileFormat fileFormat);

std::string firstImageFileNameExtension(ImageFileFormat fileFormat);
std::vector<std::string> allImageFileNameExtensions(ImageFileFormat fileFormat);
