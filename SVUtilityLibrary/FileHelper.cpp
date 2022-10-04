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
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

namespace SvUl
{
std::vector<std::string> getFileList(LPCTSTR pPath, LPCTSTR pExtension, bool recursive)
{
	std::vector<std::string> result;
	try
	{

		if (nullptr != pPath && strlen(pPath) > 0)
		{
			std::vector<std::filesystem::directory_entry>  filteredList {};

			if (recursive)
			{
				std::filesystem::recursive_directory_iterator  dirList {pPath};
				if (nullptr == pExtension)
				{
					std::copy(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList));
				}
				else
				{
					std::copy_if(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList), [&pExtension](const auto& rEntry)
					{
						return (rEntry.is_regular_file() && MakeLower(rEntry.path().extension().string().c_str()) == pExtension);
					});
				}
			}
			else
			{
				std::filesystem::directory_iterator  dirList {pPath};

				if (nullptr == pExtension)
				{
					std::copy(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList));
				}
				else
				{
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
	}
	catch (std::exception& e)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		SvDef::StringVector msgList;
		msgList.push_back(e.what());
		Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}

	
	return result;
}
} //namespace SvUl
