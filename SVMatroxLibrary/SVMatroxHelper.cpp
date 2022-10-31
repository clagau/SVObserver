//*****************************************************************************
/// This file contains helper function for this library
/// \file SVMatroxHelper.cpp
//*****************************************************************************
#include "StdAfx.h"
#include "SVMatroxHelper.h"
#include "SVMatroxResourceMonitor.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVUtilityLibrary/StringHelper.h"

HRESULT DestroyMatroxId(__int64& rId, void(*pFreeFunc)(MIL_ID), SVMatroxIdentifierEnum identifierType)
{
	HRESULT Result(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (nullptr != pFreeFunc)
		{
			if (M_NULL != rId)
			{

				pFreeFunc(rId);
				Result = SVMatroxApplicationInterface::GetLastStatus();
				if (S_OK == Result)
				{
					SVMatroxResourceMonitor::EraseIdentifier(identifierType, rId);

					rId = M_NULL;
				}

			}
		}
		else
		{
			Result = SVMEE_BAD_POINTER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return Result;

}


MIL_INT64 getMilImageFormatCode(ImageFileFormat fileFormat)
{
	switch (fileFormat)
	{
		case ImageFileFormat::invalid:
		default:
			return -1;
		case ImageFileFormat::mim:
			return M_MIL;
		case ImageFileFormat::tiff:
			return M_TIFF;
		case ImageFileFormat::bmp:
			return M_BMP;
		case ImageFileFormat::png:
			return M_PNG;
	}
}

std::string imageFileNameExtension(ImageFileFormat fileFormat)
{
	switch (fileFormat)
	{
		case ImageFileFormat::invalid:
		default:
			return ".unknown";
		case ImageFileFormat::bmp:
			return ".bmp";
		case ImageFileFormat::png:
			return ".png";
		case ImageFileFormat::tiff:
			return ".tif";
		case ImageFileFormat::mim:
			return ".mim";
	}
}

ImageFileFormat inferMilImageFileFormat(const TCHAR* pFilepath)
{
	return (nullptr == pFilepath) ? ImageFileFormat::invalid : inferMilImageFileFormat(std::string(pFilepath)); 
}

ImageFileFormat inferMilImageFileFormat(const std::string& rFilenameOrPath)
{
	auto lowercaseExtension = SvUl::getLowerCaseExtension(rFilenameOrPath);

	auto fileformats = {ImageFileFormat::mim, ImageFileFormat::tiff, ImageFileFormat::bmp, ImageFileFormat::png};

	auto result = std::ranges::find_if(fileformats, [lowercaseExtension](auto fileFormat){return 0 == SvUl::CompareNoCase(lowercaseExtension, imageFileNameExtension(fileFormat)); });

	if (result != fileformats.end())
	{
		return *result;
	}

	return ImageFileFormat::invalid;
}