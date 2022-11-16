//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file Heapwalk.cpp
//*****************************************************************************
// Functions for logging heap information
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <locale>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <boost\algorithm\string\case_conv.hpp>
#include "Heapwalk.h"
#include "FilesystemUtilities/FilepathUtilities.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"

#pragma endregion Includes

#ifdef LOG_HEAP_INFO
namespace SvUl
{
enum class EntryType { unknown, usedBlock, emptyBlock, regionHeader, emptyRange, entryTypeMax };
std::set<EntryType> allEntryTypes {EntryType::usedBlock, EntryType::emptyBlock, EntryType::regionHeader, EntryType::emptyRange};

using EntryInfo = std::tuple<size_t, EntryType, BYTE>;

void logHeap(const std::string& rTitle);

std::string getInfoString(const std::string rTypename, size_t memSize, BYTE regionIndex)
{
	static std::string oldTypename = "";
	BYTE oldRegionIndex = 0;

	if (rTypename != oldTypename || oldRegionIndex != regionIndex)
	{
		oldTypename = rTypename;
		oldRegionIndex = regionIndex;

		if ("" != rTypename)
		{
			return std::format("\n\t\t{}({}): {} B", rTypename, regionIndex, memSize);
		}

		return "";
	}
	else
	{
		return std::format(", {} B", memSize);
	}
}

EntryInfo analyze_entry(PROCESS_HEAP_ENTRY& rEntry)
{
	constexpr bool trace_analysis_details = false;

	auto type = EntryType::unknown;

	if ((rEntry.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0)
	{
		if constexpr (trace_analysis_details)
		{
			::OutputDebugString(_T("Used  block"));
		}
		type = EntryType::usedBlock;
	}
	else if ((rEntry.wFlags & PROCESS_HEAP_REGION) != 0)
	{
		if constexpr (trace_analysis_details)
		{
			::OutputDebugString(std::format(_T("{} bytes committed\n") \
				_T("  {} bytes uncommitted\n  First block address: %{}\n") \
				_T("  Last block address: %{}\n"),
				rEntry.Region.dwCommittedSize,
				rEntry.Region.dwUnCommittedSize,
				rEntry.Region.lpFirstBlock,
				rEntry.Region.lpLastBlock).c_str());
			::OutputDebugString(_T("         Region"));
		}
		type = EntryType::regionHeader;
	}
	else if ((rEntry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE) != 0)
	{
		if constexpr (trace_analysis_details)
		{
			::OutputDebugString(_T("Empty range"));
		}
		type = EntryType::emptyRange;
	}
	else
	{
		if constexpr (trace_analysis_details)
		{
			::OutputDebugString(_T("Empty Block"));
		}
		type = EntryType::emptyBlock;
	}

	if constexpr (trace_analysis_details)
	{
		::OutputDebugString(std::format(_T("({}): @{:10d}: {} + {} =  {} B\n"),
			rEntry.iRegionIndex,
			(size_t)(rEntry.lpData),
			rEntry.cbData,
			rEntry.cbOverhead,
			rEntry.cbData + rEntry.cbOverhead).c_str());
	}

	return {rEntry.cbData + rEntry.cbOverhead, type, rEntry.iRegionIndex};
}


std::string memoryString(size_t mem)
{
	std::string unit = "B";
	if (mem > 1024)
	{
		mem /= 1024;
		unit = "kB";
		if (mem > 1024)
		{
			mem /= 1024;
			unit = "MB";
			if (mem > 1024)
			{
				mem /= 1024;
				unit = "GB";
			}
		}
	}
	return std::format("{:3} {}", mem, unit);
}


class HeapInfo
{
	std::map<EntryType, size_t> m_totalMemSize;
	std::map<EntryType, size_t> m_entryTypeCount;
	std::set<uint16_t> m_regionIndices;
	size_t m_largestFreeBlockSize = 0;

	static std::map<EntryType, std::string> ms_entryTypeName;

public:
	static void setTypenames();

	HeapInfo();

	bool walk(HANDLE heapHandle, std::ofstream& rOutputFileDetails);
	size_t writeToFile(std::ofstream& rOutputFileDetails);
};


std::map<EntryType, std::string> HeapInfo::ms_entryTypeName;

HeapInfo::HeapInfo()
{
	for (auto type : allEntryTypes)
	{
		m_totalMemSize[type] = 0;
		m_entryTypeCount[type] = 0;
	}

	m_regionIndices = {};
	m_largestFreeBlockSize = 0;
}

void HeapInfo::setTypenames()
{
	ms_entryTypeName[EntryType::unknown] = "<unknown type>";
	ms_entryTypeName[EntryType::usedBlock] = "used block";
	ms_entryTypeName[EntryType::emptyBlock] = "empty block";
	ms_entryTypeName[EntryType::emptyRange] = "empty range";
	ms_entryTypeName[EntryType::regionHeader] = "region header";
}


size_t HeapInfo::writeToFile(std::ofstream& rOutputFileDetails)
{
	rOutputFileDetails << "\n\t";

	for (auto type : allEntryTypes)
	{
		rOutputFileDetails << memoryString(m_totalMemSize[type]) << std::format(" in {} {}s, ", m_entryTypeCount[type], ms_entryTypeName[type]);
	}

	rOutputFileDetails << "(" << m_regionIndices.size() << " regions):";

	for (auto regionIndex : m_regionIndices)
	{
		rOutputFileDetails << " " << regionIndex;
	}

	auto freeMem = m_totalMemSize[EntryType::emptyBlock] + m_totalMemSize[EntryType::emptyRange];
	rOutputFileDetails << "\n\t";
	rOutputFileDetails << "Free memory: " << memoryString(freeMem) << ", largest free 'block': " << memoryString(m_largestFreeBlockSize);
	rOutputFileDetails << "\n\t";
	rOutputFileDetails << "=> Fragmentation = " << 1 - (double)m_largestFreeBlockSize / (double)(freeMem) << "\n\n";

	return freeMem;
}


bool HeapInfo::walk(HANDLE heapHandle, std::ofstream& rOutputFileDetails)
{
	constexpr bool logInfoStrings = false;

	if (HeapLock(heapHandle) == FALSE) //Lock the heap to prevent other threads from accessing the heap during enumeration.
	{
		rOutputFileDetails << std::format("Failed to lock heap with LastError {}.\n", GetLastError());
		return false;
	}

	PROCESS_HEAP_ENTRY Entry = {NULL};
	Entry.lpData = NULL;

	while (HeapWalk(heapHandle, &Entry) != FALSE)
	{
		auto [memSize, type, regionIndex] = analyze_entry(Entry);
		m_entryTypeCount[type] ++;
		m_regionIndices.insert(regionIndex);

		m_totalMemSize[type] += memSize;
		if constexpr (logInfoStrings)
		{
			rOutputFileDetails << getInfoString(ms_entryTypeName[type], memSize, regionIndex);
		}

		if (EntryType::usedBlock != type && EntryType::regionHeader != type)
		{
			m_largestFreeBlockSize = std::max(memSize, m_largestFreeBlockSize);
		}
	}

	if (GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		rOutputFileDetails << std::format("HeapWalk failed with LastError {}.\n", GetLastError());
		return false;
	}

	getInfoString("", 0, 0);

	if (HeapUnlock(heapHandle) == FALSE) // Unlock the heap to allow other threads to access the heap after enumeration has completed.
	{
		rOutputFileDetails << std::format("Failed to unlock heap with LastError {}.\n", GetLastError());
		return false;
	}
	return true;
}


class HeapFragmentationChecker
{
	const std::string m_title;
	static const std::string m_OutputFilePathStump;
	static std::ofstream m_OverviewFilestream;
	static std::ofstream m_DetailsFilestream;

public:
	static void openFilestreams();
	static void closeFilestreams();
	static std::pair<std::string, std::string> getFilename(const std::string& rType, time_t in_time_t);

	HeapFragmentationChecker(const std::string& rTitle) : m_title(rTitle) {}
	void logAllHeaps();
};


const std::string HeapFragmentationChecker::m_OutputFilePathStump = "D:\\SVObserver\\Testing\\Logs\\logHeap";
std::ofstream HeapFragmentationChecker::m_OverviewFilestream;
std::ofstream HeapFragmentationChecker::m_DetailsFilestream;


void HeapFragmentationChecker::openFilestreams()
{
	static time_t in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::pair<std::string, std::string> filenameAndDirectoryPaths = getFilename("Details.log", in_time_t);
	SvFs::ensureDirectoryExistsVar(filenameAndDirectoryPaths.second);
	m_DetailsFilestream.open(filenameAndDirectoryPaths.first, std::ios_base::app);

	filenameAndDirectoryPaths = getFilename("Overview.log", in_time_t);
	SvFs::ensureDirectoryExistsVar(filenameAndDirectoryPaths.second);
	m_OverviewFilestream.open(filenameAndDirectoryPaths.first, std::ios_base::app);
}

void HeapFragmentationChecker::closeFilestreams()
{
	m_DetailsFilestream.close();
	m_OverviewFilestream.close();
}

std::pair<std::string, std::string> HeapFragmentationChecker::getFilename(const std::string& rType, time_t in_time_t)
{
	std::_Timeobj t_o = std::put_time(std::localtime(&in_time_t), _T("%Y%m%d_%H%M%S"));
	std::stringstream filenameStream;
	filenameStream << m_OutputFilePathStump << t_o << rType;
	std::filesystem::path p = filenameStream.str();
	auto dir = p.remove_filename().string();

	auto filename = filenameStream.str();

	return std::pair<std::string, std::string>{filename, dir};
}


void HeapFragmentationChecker::logAllHeaps()
{
	if (!m_OverviewFilestream.is_open() || !m_DetailsFilestream.is_open())
	{
		SvStl::MessageManager fileNotOpen(SvStl::MsgType::Log);
		fileNotOpen.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LoggingFileNotOpen, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	m_OverviewFilestream << m_title;
	m_DetailsFilestream << m_title;
	time_t in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto timestamp = std::put_time(std::localtime(&in_time_t), _T("%d/%m/%Y %H:%M:%S"));
	m_OverviewFilestream << _T(" @") << timestamp << _T(":") << std::endl;;
	m_DetailsFilestream << _T(" @") << timestamp << _T(":") << std::endl;;

	constexpr DWORD maxNumberOfHandles = 50; //this is more or less an arbitrary number

	HANDLE handles[maxNumberOfHandles];

	auto actualNumberOfHandles = GetProcessHeaps(maxNumberOfHandles, handles);

	size_t maxFreeMemory = 0;
	HeapInfo heapInfoMaxFreeMem;
	std::vector<size_t> freeMemSizes;

	for (DWORD i = 0; i < actualNumberOfHandles; i++)
	{
		HeapInfo heapInfo;
		auto heapHandle = handles[i];

		m_DetailsFilestream << std::format("\tWalking heap {} (", heapHandle) << i + 1 << " of " << actualNumberOfHandles << "):\n";

		if (false == heapInfo.walk(heapHandle, m_DetailsFilestream))
		{
			m_DetailsFilestream << "Walk aborted!";
			break;
		}

		auto freeMem = heapInfo.writeToFile(m_DetailsFilestream);

		freeMemSizes.push_back(freeMem);

		if (freeMem > maxFreeMemory)
		{
			maxFreeMemory = freeMem;
			heapInfoMaxFreeMem = heapInfo;
		}
	}

	m_OverviewFilestream << "free memory in Heaps :";
	std::sort(freeMemSizes.begin(), freeMemSizes.end(), std::greater<size_t>());
	for (auto size : freeMemSizes)
	{
		m_OverviewFilestream << " " << memoryString(size);
	}
	m_OverviewFilestream << std::endl;

	m_OverviewFilestream << "largest: ";
	heapInfoMaxFreeMem.writeToFile(m_OverviewFilestream);
}

void startHeaplogs()
{
	HeapFragmentationChecker::openFilestreams();
}

void endHeaplogs()
{
	HeapFragmentationChecker::closeFilestreams();
}

void logHeap(const std::string& rTitle)
{
	HeapInfo::setTypenames();
	HeapFragmentationChecker walker(rTitle);

	walker.logAllHeaps();
}
} // namespace SvUl
#endif 
