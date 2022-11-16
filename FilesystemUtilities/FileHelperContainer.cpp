///\copyright 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FileNameContainer.cpp

#pragma once

#include "stdafx.h"
#include "FileHelperContainer.h"
#include "SVUtilityLibrary/StringHelper.h"

namespace SvFs
{
FileNameContainer::FileNameContainer()
{}

FileNameContainer::FileNameContainer(const FileNameContainer& orig)
{
	*this = orig;
}

FileNameContainer::~FileNameContainer()
{}

const FileNameContainer& FileNameContainer::operator=(const FileNameContainer& rRhs)
{
	m_FileNameVector = rRhs.getFileNameVector();
	return *this;
}

LPCTSTR FileNameContainer::GetFileNameList()
{
	mcsFileList.clear();

	for (int i = 0; i < static_cast<int> (m_FileNameVector.size()); i++)
	{
		if (0 != i)
		{
			mcsFileList += ";";
		}

		mcsFileList += m_FileNameVector[i].GetFullFileName();
	}

	return mcsFileList.c_str();
}

bool FileNameContainer::SetFileNameList(LPCTSTR szFileNameList)
{
	bool bOk = true;
	bool bDone = false;

	std::string sTemp;
	std::string sFileNameList = szFileNameList;

	m_FileNameVector.clear();

	do
	{
		bDone = sFileNameList.empty();

		if (!bDone)
		{
			bOk = false;

			size_t i = sFileNameList.find_first_of(";,");

			if (std::string::npos == i)
			{
				sTemp = sFileNameList;
				sFileNameList.clear();
			}
			else
			{
				sTemp = SvUl::Left(sFileNameList, i);
				sFileNameList.erase(0, i + 1);
			}

			if (!sTemp.empty())
			{
				m_FileNameVector.push_back(FileHelper(sTemp.c_str()));
				bOk = true;
			}
		}
	} while (bOk && !bDone);

	return bOk;
}
}