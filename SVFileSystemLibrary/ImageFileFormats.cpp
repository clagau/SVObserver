///\copyright 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file ImageFileFormats.h 
/// enum and functions for Image File Formats

#include "stdafx.h"
#include "ImageFileFormats.h"
#include "SVUtilityLibrary/StringHelper.h"

ImageFileFormat inferMilImageFileFormat(const TCHAR* pFilepath)
{
	return (nullptr == pFilepath) ? ImageFileFormat::invalid : inferMilImageFileFormat(std::string(pFilepath));
}

auto fileformats = {ImageFileFormat::mim, ImageFileFormat::tiff, ImageFileFormat::bmp, ImageFileFormat::png}; //These are the file formats corresponding to a valid extension

bool filenameMatchesImageFormat(const std::string& rFilenameOrPath, ImageFileFormat fileFormat)
{
	const auto extension = SvUl::getExtension(rFilenameOrPath);

	auto const& allExtensions = allImageFileNameExtensions(fileFormat);
	auto result = std::ranges::find_if(allExtensions, [extension](const auto& rExtension)
	{return 0 == SvUl::CompareNoCase(extension, rExtension); });

	return result != allExtensions.end();
}


ImageFileFormat inferMilImageFileFormat(const std::string& rFilenameOrPath)
{
	auto result = std::ranges::find_if(fileformats,
		[rFilenameOrPath](auto fileFormat){return filenameMatchesImageFormat(rFilenameOrPath, fileFormat); });

	if (result != fileformats.end())
	{
		return *result;
	}

	return ImageFileFormat::invalid;
}

std::string firstImageFileNameExtension(ImageFileFormat fileFormat)
{
	return allImageFileNameExtensions(fileFormat)[0];
}

std::vector<std::string> allImageFileNameExtensions(ImageFileFormat fileFormat)
{
	switch (fileFormat)
	{
		case ImageFileFormat::invalid:
		default:
			return {_T(".unknown")};
		case ImageFileFormat::bmp:
			return {_T(".bmp")};
		case ImageFileFormat::png:
			return {_T(".png")};
		case ImageFileFormat::tiff:
			return {_T(".tif"), _T(".tiff")};
		case ImageFileFormat::mim:
			return {_T(".mim")};
		case ImageFileFormat::any:
			return {_T(".bmp"), _T(".png"), _T(".tif"), _T(".tiff"), _T(".mim")};
	}
}

