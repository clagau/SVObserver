///\copyright 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FileHelperManager.cpp

#pragma once

#include "stdafx.h"
#include "FileHelperManager.h"
#include "FileHelper.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"

namespace SvFs
{
FileHelperManager& FileHelperManager::Instance()
{
	static FileHelperManager fileNameMgr;
	return fileNameMgr;
}

FileHelperManager::FileHelperManager()
{
	std::string csRunPathName = GetRunPathName();

	m_iCurrentItem = 0;

	if (csRunPathName.empty())
	{
		SetRunPathName(SvStl::GlobalPath::Inst().GetRunPath().c_str());
	}
}

bool FileHelperManager::SetRunPathName(LPCTSTR PathName)
{
	bool Result(false);

	if (m_svFileNameList.Lock())
	{
		Result = true;

		if (nullptr != PathName)
		{
			m_RunPathName = PathName;
		}
		else
		{
			m_RunPathName.clear();
		}

		if (!m_RunPathName.empty())
		{
			Result = CreatePath(m_RunPathName.c_str());
		}

		m_svFileNameList.Unlock();
	}

	return Result;
}

bool FileHelperManager::SetConfigurationPathName(LPCTSTR szPathName)
{
	bool Result(false);

	FileHelperManager& rFileMgr = Instance();

	if (rFileMgr.m_svFileNameList.Lock())
	{
		Result = true;

		if (szPathName)
		{
			rFileMgr.m_ConfigurationPathName = szPathName;
		}
		else
		{
			rFileMgr.m_ConfigurationPathName.clear();
		}

		if (!rFileMgr.m_ConfigurationPathName.empty())
		{
			Result = CreatePath(rFileMgr.m_ConfigurationPathName.c_str());
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return Result;
}

bool FileHelperManager::FindItem(const FileHelper* const svpFileNameClass)
{
	bool bOk = false;

	FileHelperManager& rFileMgr = Instance();

	if (0 < rFileMgr.m_svFileNameList.size())
	{
		bool bDone = false;

		rFileMgr.m_iCurrentItem = static_cast<int> (rFileMgr.m_svFileNameList.size());

		for (int i = 0; !bDone && i < static_cast<int> (rFileMgr.m_svFileNameList.size()); i++)
		{
			bDone = svpFileNameClass <= rFileMgr.m_svFileNameList[i];
			if (bDone)
			{
				bOk = rFileMgr.m_svFileNameList[i] == svpFileNameClass;
				rFileMgr.m_iCurrentItem = i;
			}
		}
	}
	else
	{
		rFileMgr.m_iCurrentItem = 0;
	}

	return bOk;
}

bool FileHelperManager::AddItem(FileHelper* pFileName)
{
	bool Result(false);

	if (m_svFileNameList.Lock())
	{
		Result = FindItem(pFileName);

		if (!Result)
		{
			try
			{
				FileHelperPointerContainer::iterator Iter(m_svFileNameList.begin());

				if (0 < m_iCurrentItem)
				{
					std::advance(Iter, m_iCurrentItem);
				}
				m_svFileNameList.insert(Iter, 1, pFileName);
				Result = LoadItem(m_svFileNameList[m_iCurrentItem]);
			}
			catch (std::bad_alloc& e)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(static_cast<DWORD> (E_UNEXPECTED), e.what(), SvStl::SourceFileParams(StdMessageParams));
			}
		}

		m_svFileNameList.Unlock();
	}

	return Result;
}

bool FileHelperManager::RemoveItem(const FileHelper* const pFileName)
{
	bool Result = false;

	if (m_svFileNameList.Lock())
	{
		Result = FindItem(pFileName);

		if (Result)
		{
			m_svFileNameList.erase(m_svFileNameList.begin() + m_iCurrentItem);

			if (0 < m_svFileNameList.size())
			{
				if (static_cast<int> (m_svFileNameList.size()) <= m_iCurrentItem)
				{
					m_iCurrentItem = static_cast<int> (m_svFileNameList.size() - 1);
				}
			}
			else
			{
				m_iCurrentItem = 0;
			}
		}

		m_svFileNameList.Unlock();
	}

	return Result;
}

FileHelper* FileHelperManager::GetItem()
{
	FileHelper* pItem = nullptr;

	FileHelperManager& rFileMgr = Instance();

	if (rFileMgr.m_svFileNameList.Lock())
	{
		if (rFileMgr.m_iCurrentItem < static_cast<int> (rFileMgr.m_svFileNameList.size()))
		{
			pItem = rFileMgr.m_svFileNameList[rFileMgr.m_iCurrentItem];
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return pItem;
}

bool FileHelperManager::CreatePath(LPCTSTR PathName)
{
	std::string Path(PathName);

	bool Result = _access(Path.c_str(), 0) == 0;

	if (!Result)
	{
		bool bDone = false;

		size_t Start = 0;
		if (SvUl::Left(Path, 2) == _T("\\\\"))	// UNC path
		{
			Start = SvUl::Mid(Path, 2).find('\\') + 1;
		}
		else	// drive letter
		{
			Start = Path.find(':');
		}

		assert(Start > 0);

		do
		{
			bDone = Path.size() <= Start + 1;

			if (!bDone)
			{
				if (Path[Start + 1] == '\\')
				{
					Start++;

					bDone = Path.size() <= Start + 1;

					if (!bDone)
					{
						bDone = Path[Start + 1] == '\\';
						Result = !bDone;
					}
				}

				if (!bDone)
				{
					size_t NextBS = Path.find('\\', Start + 1);
					size_t NextFS = Path.find('/', Start + 1);

					std::string MakePath;

					if (std::string::npos == NextBS && std::string::npos == NextFS)
					{
						MakePath = Path;
					}
					else
					{
						if (std::string::npos == NextFS || std::string::npos != NextBS && NextBS < NextFS)
						{
							MakePath = SvUl::Left(Path, NextBS);
						}
						else
						{
							MakePath = SvUl::Left(Path, NextFS);
						}
					}

					Result = _access(MakePath.c_str(), 0) == 0;

					if (!Result)
					{
						Result = _mkdir(MakePath.c_str()) == 0;
					}

					Start = MakePath.size();
				}
			}
		} while (Result && !bDone);
	}

	return Result;
}

bool FileHelperManager::LoadItem(FileHelper* pFileName)
{
	bool Result = true;

	if (!std::string(pFileName->GetFileName()).empty())
	{
		std::string FullName = pFileName->GetFullFileName();
		std::string ConfigFullName = pFileName->GetFullFileName();
		std::string RunFullName = std::string(GetRunPathName()) + _T("\\") + pFileName->GetFileName();

		if (!GetConfigurationPathName().empty())
		{
			ConfigFullName = GetConfigurationPathName() + _T("\\") + pFileName->GetFileName();
		}

		SvUl::searchAndReplace(RunFullName, _T("/"), _T("\\"));
		SvUl::searchAndReplace(ConfigFullName, _T("/"), _T("\\"));
		SvUl::searchAndReplace(FullName, _T("/"), _T("\\"));

		Result = SvUl::CompareNoCase(RunFullName, FullName) == 0 && _access(RunFullName.c_str(), 0) == 0;

		if (!Result)
		{
			if (0 != SvUl::CompareNoCase(ConfigFullName, FullName))
			{
				if (_access(FullName.c_str(), 0) == 0)
				{
					Result = CopyFile(FullName.c_str(), RunFullName.c_str(), false) ? true : false;
					_chmod(RunFullName.c_str(), _S_IREAD | _S_IWRITE);
				}
			}

			if (!Result)
			{
				if (_access(ConfigFullName.c_str(), 0) == 0)
				{
					Result = CopyFile(ConfigFullName.c_str(), RunFullName.c_str(), false) ? true : false;
					_chmod(RunFullName.c_str(), _S_IREAD | _S_IWRITE);
				}
			}

			if (!Result)
			{
				Result = _access(RunFullName.c_str(), 0) == 0;
			}

			if (Result)
			{
				pFileName->SetPathName(GetRunPathName().c_str());
			}
		}
	}

	return Result;
}

SvDef::StringVector FileHelperManager::GetFileNameList()
{
	return m_svFileNameList.GetFileNameList();
}

bool FileHelperManager::RemoveUnusedFiles()
{
	bool Result(true);

	Result = RemoveUnusedFiles(GetRunPathName());
	return Result;
}

bool FileHelperManager::RemoveUnusedFiles(const std::string& rPath)
{
	bool Result(true);

	if (!rPath.empty())
	{
		std::string FileMask = rPath + _T("\\*.*");

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		hFind = FindFirstFile(FileMask.c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			BOOL bRun = true;

			while (bRun)
			{
				bool bFound = false;
				std::string FileName = FindFileData.cFileName;

				for (int i = 0; !bFound && i < static_cast<int> (m_svFileNameList.size()); i++)
				{
					bFound = (0 == SvUl::CompareNoCase(FileName, m_svFileNameList[i]->GetFileName()));
				}

				if (!bFound)
				{
					std::string FullFileName = rPath;

					FullFileName += _T("\\");
					FullFileName += FileName;

					_chmod(FullFileName.c_str(), _S_IREAD | _S_IWRITE);
					Result = DeleteFile(FullFileName.c_str()) ? true : false;
				}

				bRun = FindNextFile(hFind, &FindFileData);
			}

			FindClose(hFind);
		}
	}

	return Result;
}

}