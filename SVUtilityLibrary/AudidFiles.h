//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/05,2020/10/05 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AudidFiles.h
///  CAudidfiles is a container for the external files used in Audidtrail. In the Audidtrail 
/// some properties and  ahash code of external files will be included  
//******************************************************************************

#pragma once
namespace SvUl
{
	enum class  AudidFileType
	{
		unknown
	};
	enum class AudidListType
	{
		default, white
	};

	struct AudidFile
	{
		explicit AudidFile(LPCSTR fullname) :Fullname(fullname)
		{};
		explicit AudidFile(LPCSTR fullname, bool bh, bool bi) :Fullname(fullname),
			bhash(bh),
			bignore(bi)
		{};

		AudidFile() = default;
		~AudidFile() = default;

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
		friend bool operator<(const AudidFile& l, const AudidFile& r)
		{
			return l.Fullname < r.Fullname;
		};


	public:
		std::string Fullname;
		AudidFileType type{ AudidFileType::unknown };
		bool bhash{ true };
		bool bignore{ false };

		uintmax_t size = 0;
		std::time_t lastWriteDate = 0;
		std::string extension;
		std::string filename;
		std::string hashvalue;
		bool exist{ false };
	};




	class CAudidFiles
	{
	public:

		//Fills the m_Files;
		size_t  SetValues(const std::vector< AudidFile>& Files, bool bOnlyFlags);
		size_t  SetValues(std::vector< AudidFile>&& Files);
		size_t  SetValues(const std::vector< AudidFile>& Files);


		//Ensure DefaultList contains the same files as RunfileList
		void SyncDefaultList(const std::vector<std::string>& rCrunfileList);

		//fills properties size date and existFlag
		void UpdateList();

		//calculate Hashcode
		void CalculateSHA256();

		std::vector< AudidFile>& GetFiles() { return m_Files; };

	private:
		std::vector<AudidFile> m_Files;


	};

}
