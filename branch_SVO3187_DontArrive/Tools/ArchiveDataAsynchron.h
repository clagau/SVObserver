//******************************************************************************
/// \copyright (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ArchiveDataAsynchron.h
/// All Rights Reserved
//******************************************************************************
// Singelton class for asynchron writing of Archivetool entries
//******************************************************************************
#pragma once
#include <atomic>

#pragma region Inlcudes
#pragma endregion Includes

namespace SvTo
{

class SVArchiveTool;
class SVArchiveRecord;


class ArchiveDataAsynchron
{

public:
	class  ArchiveData
	{
	public:	
		ArchiveData(const std::string& en, std::ofstream* pFile):m_entry(en), m_pfileArchive(pFile){};
		ArchiveData(std::string&& en, std::ofstream* pFile):m_entry(en), m_pfileArchive(pFile){};
		ArchiveData() :m_pfileArchive(nullptr){};
		void writeToFile();
		
	//private:	
		std::string m_entry;
		std::ofstream* m_pfileArchive =nullptr;
	};
	
	static ArchiveDataAsynchron& Instance();
	~ArchiveDataAsynchron();
	ArchiveDataAsynchron(ArchiveDataAsynchron const&) = delete;
	void operator=(ArchiveDataAsynchron const&) = delete;
	bool GoOnline();
	bool GoOffline();
	bool QueueData(const ArchiveData& data);
	bool QueueData(ArchiveData&& data);

private:
	static   const size_t MAXQUEUESIZE;
	void ThreadFunction();
	ArchiveDataAsynchron();
	std::atomic<bool> m_Finish_Thread {false};
	std::mutex m_mutex;
	std::deque<ArchiveData> m_ArchiveData_queue;
	std::condition_variable m_data_cond;
	std::thread  m_thread;

};

}