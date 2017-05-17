#pragma region Includes
#include "StdAfx.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVString.h"
#include "SharedImageStore.h"

#pragma endregion Includes

namespace SvSml
{
	const LPCTSTR ShareNames[] = { ".Images.Reject", ".Images.Last" };

	SharedImageStore::SharedImageStore() :m_slotCount(0),
		m_slotSize(0),
		m_StoreType(last),
		m_hMapFileImage(0),
		m_pViewHeader(nullptr)
	{
		SYSTEM_INFO siSysInfo;
		GetSystemInfo(&siSysInfo);
		m_AllocationGranularity = siSysInfo.dwAllocationGranularity;
		m_ISHeadersize = (sizeof(ISHeader) / m_AllocationGranularity + 1) *  m_AllocationGranularity;

	}
	SharedImageStore::~SharedImageStore()
	{
		CloseConnection();
	}
	void   SharedImageStore::CloseConnection()
	{
		UnmapViewOfFile(m_pViewHeader);
		m_pViewHeader = nullptr;
		for (auto &vp : m_pViewImagesVector)
		{
			UnmapViewOfFile(vp);
			vp = nullptr;
		}
		CloseHandle(m_hMapFileImage);
	}


	void SharedImageStore::BuildMapFileName()
	{
		m_MapFileName.clear();
		m_MapFileName = _T("Global\\ImageFileMapping.");
		m_MapFileName += m_InspectionName;
		if (reject == m_StoreType)
		{
			m_MapFileName += ShareNames[0];
		}
		else
		{
			m_MapFileName += ShareNames[1];
		}
	}

	bool SharedImageStore::CreateImageStore(const SVString &InspectionName, StoreType t, DWORD slotsize, DWORD  slots)
	{
		m_InspectionName = InspectionName;
		m_StoreType = t;
		m_slotCount = slots;
		BuildMapFileName();
		CloseConnection();
		m_slotSize = (slotsize / m_AllocationGranularity + 1) *  m_AllocationGranularity;
		DWORD SharedSize = m_slotCount * m_slotSize + m_ISHeadersize;
		SetLastError(0);
		m_hMapFileImage = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SharedSize, m_MapFileName.c_str());
		if (m_hMapFileImage == nullptr || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
			SVStringVector msgList;
			msgList.push_back(LastError);
			SvStl::MessageContainer message(SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
			throw message;
		}

		m_pViewHeader = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, 0, m_ISHeadersize);
		if (m_pViewHeader == nullptr)
		{
			SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
			SVStringVector msgList;
			msgList.push_back(LastError);
			SvStl::MessageContainer message(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16227);
			throw message;

		}

		ISHeader* pHeader = reinterpret_cast<ISHeader*>(m_pViewHeader);
		pHeader->slotCount = m_slotCount;
		pHeader->slotSize = m_slotSize;
		m_pViewImagesVector.resize(m_slotCount);
		for (DWORD i = 0; i < m_slotCount; i++)
		{
			DWORD offset = m_ISHeadersize + i* m_slotSize;
			m_pViewImagesVector[i] = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, offset, m_slotSize);

			if (nullptr == m_pViewImagesVector[i])
			{
				SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
				SVStringVector msgList;
				msgList.push_back(LastError);
				SvStl::MessageContainer message(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16228);
				throw message;
			}
		}
		return true;
	}

	bool SharedImageStore::OpenImageStore(const SVString &InspectionName, StoreType t)
	{
		m_InspectionName = InspectionName;
		m_StoreType = t;
		BuildMapFileName();
		CloseConnection();

		m_hMapFileImage = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_MapFileName.c_str());
		if (m_hMapFileImage == nullptr)
		{
			DWORD error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND)
			{
				return false;
			}
			else
			{
				SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
				SVStringVector msgList;
				msgList.push_back(LastError);
				SvStl::MessageContainer message(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16230);
				throw message;
			}
		}

		m_pViewHeader = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, 0, m_ISHeadersize);
		if (nullptr == m_pViewHeader)
		{
			SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
			SVStringVector msgList;
			msgList.push_back(LastError);
			SvStl::MessageContainer message(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16231);
			throw message;
		}

		ISHeader* pHeader = reinterpret_cast<ISHeader*>(m_pViewHeader);
		m_slotCount = pHeader->slotCount;
		m_slotSize = pHeader->slotSize;

		if (m_slotCount <= 0 || m_slotSize < m_AllocationGranularity)
		{
			SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
			SVStringVector msgList;
			msgList.push_back(LastError);
			SvStl::MessageContainer message(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16232);
			throw message;
		}

		m_pViewImagesVector.resize(m_slotCount);
		for (DWORD i = 0; i < m_slotCount; i++)
		{
			DWORD offset = m_ISHeadersize + i* m_slotSize;
			m_pViewImagesVector[i] = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, offset, m_slotSize);

			if (m_pViewImagesVector[i] == nullptr)
			{
				SVString LastError = SvUl_SF::Format(_T("%i"), GetLastError());
				SVStringVector msgList;
				msgList.push_back(LastError);
				SvStl::MessageContainer message(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16233);
				throw message;
			}
		}

		return true;
	}

	bool SharedImageStore::Destroy()
	{
		CloseConnection();
		return true;
	}

	BYTE* SharedImageStore::GetPtr(DWORD slot, DWORD offset)
	{
		if (!m_hMapFileImage || slot >= m_slotCount || offset >= m_slotSize)
		{
			return NULL;
		}
		BYTE *res = (BYTE*)m_pViewImagesVector[slot];
		if (res)
		{
			res += offset;
		}
		return res;
	}

	LPCTSTR SharedImageStore::GetMapFileName() const
	{
		return m_MapFileName.c_str();
	}
	DWORD SharedImageStore::GetSlotCount() const
	{
		return m_slotCount;
	}
	DWORD SharedImageStore::GetSlotSize() const
	{
		return m_slotSize;
	}
} //namespace SvSml
