//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/06,2020/10/06 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AuditFiles.cpp
///  CAuditfiles is a container for the external files used in Audittrail. In the Audittrail 
/// some properties and  a hash code of external files will be included  
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "AuditFiles.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include "sha256.h"

#pragma endregion Includes

namespace bfs = boost::filesystem;

namespace SvUl
{

	std::string AuditFile::GetFormatedWriteDate() const
	{
		char mbstr[100];
		mbstr[0] = '\0';
		std::strftime(mbstr, sizeof(mbstr), "%d %m %Y %H:%M:%S ", std::localtime(&lastWriteDate));
		std::string ret = mbstr;
		return ret;
	}

	std::string AuditFile::GetFormatedSize() const
	{
		return std::to_string(size);
	}


	
	std::string AuditFile::GetFormatedDuration() const
	{
		int durationInMycro = static_cast<int> (1000.0 * duration);
		return std::to_string(durationInMycro);
	}

	void AuditFile::Trace() const
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_AUDITFILE)	
		char cbuf[100];
		cbuf[0] = '\0';
		std::strftime(cbuf, sizeof(cbuf), "%d %m %Y %H:%M:%S ", std::localtime(&lastWriteDate));
		std::string strDate = cbuf;

		std::stringstream ss;
		ss << Fullname << " " << size << " " << strDate << " " << filename << "  " << extension << std::endl;
		::OutputDebugString(ss.str().c_str());
#endif 
	}

	
	std::string AuditFile::GetExtension() const
	{
		return extension;
	}
	std::string AuditFile::GetFilename() const
	{
		return filename;
	}
	std::string AuditFile::GetHashvalue() const
	{
		return hashvalue;
	}

	std::string AuditFile::GetFullname() const
	{
		return Fullname;
	}


	void AuditFiles::SyncDefaultList(const std::vector<std::string>& rCrunfileList)
	{
		std::map < std::string, AuditFile> temp;
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

	void  AuditFiles::UpdateList()
	{
		for (auto& F : m_Files)
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

	void  AuditFiles::CalculateSHA256(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_Files.size()))
		{
			return;
		}

		auto& F = m_Files[index];
		std::string hash;
		hash.clear();
		double duration = 0.0;
		if (false == F.bignore && true == F.bhash)
		{
			try
			{
				hash = SHA256(F.GetFullname().c_str(), &duration);
			}
			catch (const std::exception& e)
			{
				hash = e.what();
				duration = 0;
			}
		}
		F.hashvalue = hash;
		F.duration = duration;


	}
	void  AuditFiles::CalculateSHA256()
	{

		std::string hash;
		for (auto& F : m_Files)
		{
			hash.clear();
			double duration = 0.0;
			if (false == F.bignore && true == F.bhash)
			{
				try
				{
					hash = SHA256(F.GetFullname().c_str(), &duration);
				}
				catch (const std::exception& e)
				{
					hash = e.what();
					duration = 0;
				}
			}
			F.hashvalue = hash;
			F.duration = duration;

		}
	}

	void  AuditFiles::Sort()
	{
		std::sort(m_Files.begin(), m_Files.end());

	}

	size_t   AuditFiles::SetValues(const std::vector< AuditFile>& Files, bool bOnlyFlags)
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
	size_t   AuditFiles::SetValues(const std::vector< AuditFile>& Files)
	{
		m_Files = Files;
		return m_Files.size();
	}

	size_t   AuditFiles::SetValues(std::vector< AuditFile>&& Files)
	{
		m_Files = std::move(Files);
		return m_Files.size();
	}
}
