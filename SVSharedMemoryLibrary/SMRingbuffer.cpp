//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SMRingbuffer.cpp
/// All Rights Reserved 
//*****************************************************************************
// Class implements Slot managment  
/// the class holds two ringbuffer of Bufferelemnt one for rejects  
//******************************************************************************
#include "stdafx.h"
#include "SVUtilityLibrary\SVString.h"
#include "SMRingbuffer.h"
#include "SVStatusLibrary\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageTextEnum.h"
#include "SVMessage\SVMessage.h"
#include "SMParameterStruct.h"

namespace SvSml
{
	const   LPCTSTR SMRingBuffer::s_BaseName = _T("Global\\RingBuffer.");
	SMRingBuffer::SMRingBuffer() :m_TotalSlotCount(0),
		m_RejectSlotCount(0),
		m_hMapFile(0),
		m_pRingBufferInfos(0),
		m_pRingBufferLast(0),
		m_pRingBufferReject(0),
		m_Retry(20)
	{
		SYSTEM_INFO siSysInfo;
		GetSystemInfo(&siSysInfo);
		m_AllocationGranularity = siSysInfo.dwAllocationGranularity;
		m_InfoSize = sizeof(RingbufferInfos);
		m_MatchedInfoSize = MatchedSize(m_InfoSize, m_AllocationGranularity);
		
	}
	SMRingBuffer::~SMRingBuffer()
	{
		CloseConnection();
	}
	void SMRingBuffer::CreateConnection(LPCTSTR Name, int nTotal, int nRejects, const SMParameterStruct& rParam)
	{
		CloseConnection();
		if (Name == nullptr || nTotal == 0 || nRejects >= nTotal)
		{
			throw std::exception("Bad Argument in Create");
		}
		m_FileName = s_BaseName;
		m_FileName.append(Name);
		m_TotalSlotCount = nTotal;
		m_RejectSlotCount = nRejects;
		
		
		DWORD EntrySize = (m_TotalSlotCount + m_RejectSlotCount) * sizeof(BufferElement);
		// Create the file mapping
		for (int time = 0; time <= rParam.SMCreateTimeout; time += rParam.SMCreateWaitTime)
		{
			SetLastError(0x0);
			m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_MatchedInfoSize + EntrySize, m_FileName.c_str());
			if (nullptr != m_hMapFile &&  GetLastError() != ERROR_ALREADY_EXISTS)
			{
				break;
			}
			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				//Wait for the clients   to closehandle to SharedMemory 
				CloseHandle(m_hMapFile);
				m_hMapFile = nullptr;
				Sleep(rParam.SMCreateWaitTime);
				continue;
			}
			else
			{
				break;
			}
		}
		if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE)
		{
			ThrowCreateFileMappingFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16237);
		}
		MapViewOfFileWriteInfos();
	}
	void SMRingBuffer::MapViewOfFileWriteInfos()
	{
		DWORD EntrySize = (m_TotalSlotCount + m_RejectSlotCount) * sizeof(BufferElement);
		m_pRingBufferInfos = (RingbufferInfos*)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0,m_InfoSize);
		if (m_pRingBufferInfos == NULL)
		{
			ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16238);
		}
		memset(m_pRingBufferInfos, 0, m_InfoSize);
		m_pRingBufferLast = (BufferElement*)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0,m_MatchedInfoSize, EntrySize);
		if (m_pRingBufferLast == NULL)
		{
			ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16239);
		}

		memset(m_pRingBufferLast, 0, EntrySize);
		for (int slot = 0; slot < m_TotalSlotCount; slot++)
		{
			m_pRingBufferLast[slot].SlotNumberLast = slot;
			m_pRingBufferLast[slot].SlotNumberReject = -1;
		}
		m_pRingBufferInfos->TotalSlotCount = m_TotalSlotCount;
		m_pRingBufferInfos->RejectSlotCount = m_RejectSlotCount;
		m_pRingBufferInfos->LastEntry = -1;
		m_pRingBufferInfos->LastReject = -1;
		m_pRingBufferInfos->LastUsedRejectSlot = -1;

		m_pRingBufferReject = (m_pRingBufferLast + m_TotalSlotCount);
		for (int slot = 0; slot < m_RejectSlotCount; slot++)
		{
			m_pRingBufferReject[slot].SlotNumberLast = -1;
			m_pRingBufferReject[slot].SlotNumberReject = slot;
		}
	}
	void  SMRingBuffer::OpenConnection(LPCTSTR Name)
	{
		CloseConnection();
		if (Name == nullptr)
		{
			throw std::exception("Bad Argument in Create");
		}
		m_FileName = s_BaseName;
		m_FileName.append(Name);
		m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_FileName.c_str());
		if (m_hMapFile == nullptr)
		{
			ThrowCreateFileMappingFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16240);
		}
		
		// Map to the file
		m_pRingBufferInfos = (RingbufferInfos*)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, m_InfoSize);
		if (m_pRingBufferInfos == NULL || m_pRingBufferInfos->TotalSlotCount == 0)
		{
			std::stringstream ss;
			if (!m_pRingBufferInfos)
				ss << "Create File Mapping failed: " << GetLastError();
			else
				ss << "OpenConnection failed for  : " << Name << ": Entry empty" << std::endl;;
			throw std::exception(ss.str().c_str());
		}
		m_TotalSlotCount = m_pRingBufferInfos->TotalSlotCount;
		m_RejectSlotCount = m_pRingBufferInfos->RejectSlotCount;
		DWORD EntrySize = (m_TotalSlotCount + m_RejectSlotCount) * sizeof(BufferElement);
		m_pRingBufferLast = (BufferElement*)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, m_MatchedInfoSize, EntrySize);
		if (m_pRingBufferLast == NULL)
		{
			ThrowCreateFileMappingFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16241);
		}
		m_pRingBufferReject = m_pRingBufferLast + m_TotalSlotCount;
	}
	void SMRingBuffer::CloseConnection()
	{
		if (m_pRingBufferInfos && UnmapViewOfFile(m_pRingBufferInfos))
		{
			m_pRingBufferInfos = 0;
		}
		if (m_pRingBufferLast && UnmapViewOfFile(m_pRingBufferLast))
		{
			m_pRingBufferLast = 0;
			m_TotalSlotCount = 0;
			m_pRingBufferReject = 0;
		}
		if (m_hMapFile && CloseHandle(m_hMapFile))
		{
			m_hMapFile = 0;
		}
	}
	int  SMRingBuffer::GetReaderSlotLastWritten(SlotType  type)
	{
		if (!m_pRingBufferInfos)
		{
			throw std::exception("No Ringbuffer");
		}

		int   slot(-1);
		for (int i = 0; i <= m_Retry; i++)
		{
			if (type == Reject)
			{
				slot = m_pRingBufferInfos->LastReject;
			}
			else
			{
				slot = m_pRingBufferInfos->LastEntry;
			}
			if (slot < 0)
			{
				return -1;
			}

			long prevSync = m_pRingBufferLast[slot].SyncDWord;
			if (WRITE_FLAG & prevSync)
				continue;
			long newSync = prevSync + 1;

			if (InterlockedCompareExchange(&(m_pRingBufferLast[slot].SyncDWord), newSync, prevSync) == prevSync)
			{
#if defined(TRACE_MANAGER) 
				SVString DebugStr = SvUl_SF::Format("GetReaderSlotLastWritten:(slot,sync): (%i%i)\n", slot, newSync);
				::OutputDebugString(DebugStr.c_str());
#endif
				return slot;

			}
		}
#if defined(TRACE_MANAGER) 
		::OutputDebugString("GetReaderSlotLastWritten:-1\n");
#endif
		return  -1;
	}
	int  SMRingBuffer::GetReaderSlotByTrigger(DWORD Triggercount)
	{

		if (!m_pRingBufferInfos)
		{
			throw std::exception("No Ringbuffer");
		}
		int slot = m_pRingBufferInfos->LastEntry;

		for (int i = 0; i < m_TotalSlotCount; i++, DecreaseSlotNumber(slot, m_TotalSlotCount))
		{
			DWORD prevSync = m_pRingBufferLast[slot].SyncDWord;
			if (WRITE_FLAG & prevSync)
			{
				continue;
			}
			if (m_pRingBufferLast[slot].TriggerNumber != Triggercount)
			{
				continue;
			}

			for (int Retry = 0; Retry <= m_Retry; Retry++)
			{
				long newSync = prevSync + 1;
				if (InterlockedCompareExchange(&(m_pRingBufferLast[slot].SyncDWord), newSync, prevSync) == prevSync)
				{
#if defined(TRACE_MANAGER) 
					SVString DebugStr = SvUl_SF::Format("GetReaderSlotByTrigger:(slot,t,sync): (%i,%i,%i)\n", slot, Triggercount, newSync);
					::OutputDebugString(DebugStr.c_str());
#endif
					return slot;
				}

				prevSync = m_pRingBufferLast[slot].SyncDWord;
				if (WRITE_FLAG & prevSync)
				{
					continue;
				}
			}
			break;
		}
#if defined(TRACE_MANAGER) 
		SVString DebugStr = SvUl_SF::Format("GetReaderSlotByTrigger:(slot,t): (-1,%i)\n", Triggercount);
		::OutputDebugString(DebugStr.c_str());
#endif
		return -1;

	}
	void SMRingBuffer::ReleaseReaderSlot(int  readerslot)
	{
		if (nullptr == m_pRingBufferLast || readerslot < 0)
		{
			throw std::exception("No Ringbuffer or invalid slot");
		}


		long sync = InterlockedDecrement(&(m_pRingBufferLast[readerslot].SyncDWord));
#if defined(TRACE_MANAGER) 
		SVString DebugStr = SvUl_SF::Format("Release ReaderSlot: %i to %i \n", readerslot, sync);
		::OutputDebugString(DebugStr.c_str());
#endif
	}
	void SMRingBuffer::SetToReject(int writerslot)
	{
		if (!m_pRingBufferInfos || !m_pRingBufferLast || writerslot < 0)
		{
			throw std::exception("No Ringbuffer or invalid slot");
		}
		m_pRingBufferLast[writerslot].isReject = TRUE;
		return;
	}
	int   SMRingBuffer::GetNextWriteSlot()
	{

		if (!m_pRingBufferInfos)
		{
			throw std::exception("No Ringbuffer");
		}

		int  slot = m_pRingBufferInfos->LastEntry;

		for (int i = 0; i < m_TotalSlotCount; i++)
		{
			IncreaseSlotNumber(slot, m_TotalSlotCount);
			if (InterlockedCompareExchange(&(m_pRingBufferLast[slot].SyncDWord), WRITE_FLAG, 0x0) == 0)
			{
#if defined(TRACE_MANAGER) 
				SVString DebugStr = SvUl_SF::Format("GetNextWriteSlot: %i\n", slot);
				::OutputDebugString(DebugStr.c_str());
#endif 
				return slot;

			}
		}
#if defined(TRACE_MANAGER) 

		::OutputDebugString("GetNextWriteSlot: -1\n");
#endif

		return  -1;

	}
	
	void	SMRingBuffer::ReleaseWriteSlot(int slot, DWORD triggerNumber, BOOL isValid)
	{
		if (!m_pRingBufferInfos || !m_pRingBufferLast || slot < 0)
		{

#if defined(TRACE_MANAGER)
			SVString DebugStr = SvUl_SF::Format("Release FAILED Write Slot : %i T%i  \n", slot, triggerNumber);
			::OutputDebugString(DebugStr.c_str());
#endif 
			throw std::exception("No Ringbuffer or invalid slot");
		}
		assert(m_pRingBufferLast[slot].SlotNumberLast == slot);
		BOOL isReject = m_pRingBufferLast[slot].isReject;
		if (isValid)
		{
			m_pRingBufferLast[slot].TriggerNumber = triggerNumber;
			m_pRingBufferInfos->LastEntry = slot;
		}

		//reset reject flag 
		m_pRingBufferLast[slot].isReject = FALSE;
		if (isValid && isReject)
		{
			m_pRingBufferLast[slot].SyncDWord = 0x1;
			m_pRingBufferInfos->LastReject = slot;
			Push_ToReject(triggerNumber, slot);
		}
		else
		{
			m_pRingBufferLast[slot].SyncDWord = 0x0;

		}
#if defined(TRACE_MANAGER) 
		SVString DebugStr = SvUl_SF::Format("Release Write Slot : %i T:%i \n", slot, triggerNumber);
		if (isValid && isReject)
		{
			DebugStr = SvUl_SF::Format("Release Write Slot to 0x1: %i T:%i\n", slot, triggerNumber);
		}
		::OutputDebugString(DebugStr.c_str());
#endif 

	}
	DWORD SMRingBuffer::GetTriggerNumber(int slot) const
	{
		if (!m_pRingBufferInfos || !m_pRingBufferLast || slot < 0)
		{
			throw std::exception("No Ringbuffer or invalid slot");
		}
		return m_pRingBufferLast[slot].TriggerNumber;
	}
	int SMRingBuffer::GetRejects(std::vector<DWORD> &rRejects)  const
	{
		if (!m_pRingBufferInfos || !m_pRingBufferReject)
		{
			throw std::exception("No Ringbuffer");
		}
		rRejects.reserve(m_RejectSlotCount);
		rRejects.clear();
		int  rejectSlot = m_pRingBufferInfos->LastUsedRejectSlot;
		if (rejectSlot < 0)
		{
			//no rejects!
			return 0;
		}
		int maxcount = m_RejectSlotCount;

		for (int i = 0; i < maxcount; i++)
		{
			IncreaseSlotNumber(rejectSlot, m_RejectSlotCount);
			if (m_pRingBufferReject[rejectSlot].SlotNumberLast >= 0)
			{
				DWORD TriggerNumber = m_pRingBufferReject[rejectSlot].TriggerNumber;
				rRejects.push_back(TriggerNumber);
			}

		}
		return static_cast<int>(rRejects.size());
	}
	int SMRingBuffer::GetTotalSlotCount() const
	{
		return m_TotalSlotCount;
	}
	int SMRingBuffer::GetRejectSlotCount() const
	{
		return m_RejectSlotCount;
	}
	void  SMRingBuffer::IncreaseSlotNumber(int &rSlotnumber, int SlotCount)
	{
		rSlotnumber = (++rSlotnumber) % SlotCount;
	}
	void  SMRingBuffer::DecreaseSlotNumber(int &rSlotnumber, int SlotCount)
	{
		rSlotnumber--;
		if (rSlotnumber < 0)
		{
			rSlotnumber += SlotCount;
		}
	}
	DWORD SMRingBuffer::MatchedSize(DWORD size, DWORD  AllocationGranularity)
	{
		DWORD ret;
		ret = (size / AllocationGranularity) * AllocationGranularity;
		if (size % AllocationGranularity)
			ret += AllocationGranularity;
		return ret;
	}
	BOOL SMRingBuffer::Push_ToReject(DWORD triggerNumber, int slotNrLast)
	{

		if (!m_pRingBufferInfos || !m_pRingBufferLast || !m_pRingBufferReject)
		{
			throw std::exception("No Ringbuffer");
		}
		BOOL sucess(false);
		int  rejectSlot = m_pRingBufferInfos->LastUsedRejectSlot;

		for (int i = 0; i < m_RejectSlotCount; i++)
		{
			IncreaseSlotNumber(rejectSlot, m_RejectSlotCount);
			if (InterlockedCompareExchange(&(m_pRingBufferReject[rejectSlot].SyncDWord), WRITE_FLAG, 0x0) == 0x0)
			{
				sucess = TRUE;
				break;
			}
		}
		if (sucess)
		{
			if (m_pRingBufferReject[rejectSlot].SlotNumberLast >= 0)
			{
				ReleaseReaderSlot(m_pRingBufferReject[rejectSlot].SlotNumberLast);

			}

			m_pRingBufferInfos->LastUsedRejectSlot = rejectSlot;
			assert(m_pRingBufferReject[rejectSlot].SlotNumberReject == rejectSlot);
			m_pRingBufferReject[rejectSlot].SlotNumberLast = slotNrLast;
			m_pRingBufferReject[rejectSlot].TriggerNumber = triggerNumber;
			m_pRingBufferReject[rejectSlot].SyncDWord = 0x0;


		}
		return sucess;
	}

	void SMRingBuffer::ThrowCreateFileMappingFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode)
	{
		SVString LastError = SvUl_SF::Format(_T("%s LastError:  %i"), m_FileName.c_str(), GetLastError());
		SVStringVector msgList;
		msgList.push_back(LastError);

		SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
		MesMan.setMessage(SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
		MesMan.Throw();
	}
	void SMRingBuffer::ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode)
	{
		SVString LastError = SvUl_SF::Format(_T("%s LastError:  %i"), m_FileName.c_str(), GetLastError());
		SVStringVector msgList;
		msgList.push_back(LastError);

		SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
		MesMan.setMessage(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
		MesMan.Throw();
	}


}

