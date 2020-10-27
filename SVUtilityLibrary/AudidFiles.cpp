//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/06,2020/10/06 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AudidFiles.cpp
/// DESCRIPTION
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "AudidFiles.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include "sha256.h"


#pragma endregion Includes




namespace bfs = boost::filesystem;



namespace SvUl
{

	std::string AudidFile::GetFormatedWriteDate() const
	{
		char mbstr[100];
		mbstr[0] = '\0';
		std::strftime(mbstr, sizeof(mbstr), "%d %m %Y %H:%M:%S ", std::localtime(&lastWriteDate));
		std::string ret = mbstr;
		return ret;
	}

	std::string AudidFile::GetFormatedSize() const
	{
		return std::to_string(size);
	}


	std::string   AudidFile::GetFormatedIgnoreFlag() const
	{
		return Flag2String(bignore);
	}
	std::string  AudidFile::GetFormatedHashFlag() const
	{
		return Flag2String(bhash);
	}




	void AudidFile::Trace() const
	{
		
		char mbstr[100];
		mbstr[0] = '\0';
		std::strftime(mbstr, sizeof(mbstr), "%d %m %Y %H:%M:%S ", std::localtime(&lastWriteDate));
		std::string ret = mbstr;

		std::stringstream ss;
		ss << Fullname << " " << size << " " << mbstr << " " << filename << "  " << extension << std::endl;
		::OutputDebugString(ss.str().c_str());

	}

	std::string  AudidFile::Flag2String(bool flag)
	{
		std::string ret;
		ret = flag ? "true" : "false";
		return ret;
	}


	bool    AudidFile::String2Flag(const std::string& flags)
	{
		if (flags.compare("true") == 0 || flags.compare("TRUE") == 0)
			return true;
		else
			return false;
	}



	std::string AudidFile::GetExtension() const
	{
		return extension;
	}
	std::string AudidFile::GetFilename() const
	{
		return filename;
	}
	std::string AudidFile::GetHashvalue() const
	{
		return hashvalue;
	}

	std::string AudidFile::GetFullname() const
	{
		return Fullname;
	}


	void CAudidFiles::SyncDefaultList(const std::vector<std::string>& rCrunfileList)
	{
		std::map < std::string, AudidFile> temp;
		for (auto& it : m_Files)
		{
			temp[it.Fullname] = std::move(it);
		}
		m_Files.clear();
		for (const auto& fileName : rCrunfileList)
		{
			auto AIt = temp.find(fileName);
			if (AIt == temp.end())
			{
				m_Files.emplace_back(fileName.c_str());
			}
			else
			{
				m_Files.push_back(std::move(AIt->second));
			}
		}
	}

	void  CAudidFiles::UpdateList()
	{
		for (auto& F : m_Files)
		{
			if (false == F.bignore)
			{

				bfs::path Path = F.Fullname.c_str();
				F.exist = (bfs::exists(Path) && bfs::is_regular_file(Path));
				if (F.exist)
				{
					F.size = bfs::file_size(Path);
					F.lastWriteDate = bfs::last_write_time(Path);

					F.filename = Path.filename().string();
					F.extension = Path.extension().string();
					F.Trace();
				}

			}

		}
	}
	void  CAudidFiles::CalculateSHA256()
	{

		std::string hash;
		for (auto& F : m_Files)
		{
			hash.clear();

			if (false == F.bignore && true == F.bhash)
			{
				try
				{
					hash = SHA256(F.GetFullname().c_str());
				}
				catch (const std::exception& e)
				{
					hash = e.what();;
				}
			}
			F.hashvalue = hash;


		}
	}


	size_t   CAudidFiles::SetValues(const std::vector< AudidFile>& Files, bool bOnlyFlags)
	{

		if (bOnlyFlags && Files.size() == m_Files.size())
		{
			int  len = int(Files.size());
			for (int i = 0; i < len; i++)
			{
				m_Files[i].bhash = Files[i].bhash;
				m_Files[i].bignore = Files[i].bignore;
			}
			return m_Files.size();
		}
		else
		{
			return SetValues(Files);
		}

	}
	size_t   CAudidFiles::SetValues(const std::vector< AudidFile>& Files)
	{

		m_Files = Files;
		return m_Files.size();
	}
	size_t   CAudidFiles::SetValues(std::vector< AudidFile>&& Files)
	{

		m_Files = std::move(Files);
		return m_Files.size();
	}
}
