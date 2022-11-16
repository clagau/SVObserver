///\copyright 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FileHelperPointerContainer.cpp

#pragma once

#include "stdafx.h"
#include "FileHelperPointerContainer.h"
#include "FileHelper.h"

namespace SvFs
{
const FileHelperPointerContainer FileHelperPointerContainer::operator=(const FileHelperPointerContainer& rRhs)
{
	m_fileHelpers = rRhs.getFileNamePtrVector();

	return *this;
}

SvDef::StringVector FileHelperPointerContainer::GetFileNameList() const
{
	SvDef::StringVector FileNameList;

	for (const auto& rFile : m_fileHelpers)
	{
		std::string FileName {rFile->GetFullFileName()};
		if (0 == _access(FileName.c_str(), 0))
		{
			//Don't place filenames already in list
			if (FileNameList.end() == std::find(FileNameList.begin(), FileNameList.end(), FileName))
			{
				FileNameList.emplace_back(FileName);
			}
		}
	}

	return FileNameList;
}

bool FileHelperPointerContainer::Lock()
{
	return mcmArrayLock.Lock(300000);
}

bool FileHelperPointerContainer::Unlock()
{
	return mcmArrayLock.Unlock();
}
}
