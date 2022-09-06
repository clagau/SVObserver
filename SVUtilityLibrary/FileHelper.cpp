//*****************************************************************************
/// \copyright COPYRIGHT (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file FileHelper.cpp
/// \brief General file helper functions
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
#include "FileHelper.h"
#include "StringHelper.h"
#pragma endregion Includes

namespace SvUl
{
std::vector<std::string> getFileList(LPCTSTR pPath, LPCTSTR pExtension, bool recursive)
{
	std::vector<std::string> result;

	if (nullptr != pPath)
	{
		std::vector<std::filesystem::directory_entry>  filteredList {};

		if (recursive)
		{
			std::filesystem::recursive_directory_iterator  dirList{pPath};
			if (nullptr == pExtension)
			{
				// cppcheck-suppress danglingTemporaryLifetime
				std::copy(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList));
			}
			else
			{
				// cppcheck-suppress danglingTemporaryLifetime
				std::copy_if(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList), [&pExtension](const auto& rEntry)
				{
					return (rEntry.is_regular_file() && MakeLower(rEntry.path().extension().string().c_str()) == pExtension);
				});
			}
		}
		else
		{
			std::filesystem::recursive_directory_iterator  dirList {pPath};
			if (nullptr == pExtension)
			{
				// cppcheck-suppress danglingTemporaryLifetime
				std::copy(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList));
			}
			else
			{
				// cppcheck-suppress danglingTemporaryLifetime
				std::copy_if(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList), [&pExtension](const auto& rEntry)
				{
					return (rEntry.is_regular_file() && MakeLower(rEntry.path().extension().string().c_str()) == pExtension);
				});
			}
		}
		result.reserve(filteredList.size());
		for (const auto& rEntry : filteredList)
		{
			// cppcheck-suppress useStlAlgorithm
			result.push_back(rEntry.path().string());
		}
		//StrCmpLogicalW is the sorting function used by Windows Explorer
		auto FolderCompare = [](const std::string& rLhs, const std::string& rRhs) { return ::StrCmpLogicalW(_bstr_t {rLhs.c_str()}, _bstr_t {rRhs.c_str()}) < 0; };
		std::sort(result.begin(), result.end(), FolderCompare);
	}
	return result;
}
} //namespace SvUl
