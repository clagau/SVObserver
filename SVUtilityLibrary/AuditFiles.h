//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/05,2020/10/05 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AuditFiles.h
///  CAuditfiles is a container for the external files used in Audittrail. In the Audittrail 
/// some properties and  ahash code of external files will be included  
//******************************************************************************

#pragma once
namespace SvUl
{
	enum class  AuditFileType
	{
		unknown
	};
	enum class AuditListType
	{
		default, white
	};

	struct AuditFile
	{
		explicit AuditFile(LPCSTR fullname) :Fullname(fullname)
		{};
		explicit AuditFile(LPCSTR fullname, bool bh, bool bi) :Fullname(fullname),
			bhash(bh),
			bignore(bi)
		{};

		AuditFile() = default;
		~AuditFile() = default;

		void Trace() const;
		std::string GetFormatedWriteDate() const;
		std::string GetFormatedSize() const;
		std::string  GetFormatedIgnoreFlag() const;
		std::string  GetFormatedHashFlag() const;

		std::string GetExtension() const;
		std::string GetFilename() const;
		std::string GetFullname() const;
		std::string GetHashvalue() const;

		static std::string Flag2String(bool flag);
		static bool   String2Flag(const std::string& flags);
		friend bool operator<(const AuditFile& l, const AuditFile& r)
		{
			return l.Fullname < r.Fullname;
		};


	public:
		std::string Fullname;
		AuditFileType type{ AuditFileType::unknown };
		bool bhash{ true };
		bool bignore{ false };

		uintmax_t size = 0;
		std::time_t lastWriteDate = 0;
		std::string extension;
		std::string filename;
		std::string hashvalue;
		bool exist{ false };
	};




	class CAuditFiles
	{
	public:

		//Fills the m_Files;
		size_t  SetValues(const std::vector< AuditFile>& Files, bool bOnlyFlags);
		size_t  SetValues(std::vector< AuditFile>&& Files);
		size_t  SetValues(const std::vector< AuditFile>& Files);


		//Ensure DefaultList contains the same files as RunfileList
		void SyncDefaultList(const std::vector<std::string>& rCrunfileList);

		//fills properties size date and existFlag
		void UpdateList();

		//calculate Hashcode
		void CalculateSHA256();

		//sort in order of full Filename
		void Sort();

		std::vector< AuditFile>& GetFiles() { return m_Files; };

	private:
		std::vector<AuditFile> m_Files;


	};

}
