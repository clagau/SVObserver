//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedDataStore.cpp
/// All Rights Reserved 
//*****************************************************************************
///class for holding memory for the dataBufferes in SharedMemory
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SharedDataStore.h"
#include "SMParameterStruct.h"
#pragma endregion Includes

namespace SvSml
{
	SharedDataStore::SharedDataStore() :m_bUseOneViewPerSlot(false),
		m_slotCount(0),
		m_slotSize(0),
		m_hMapFileImage(0),
		m_pViewHeader(nullptr),
		m_pViewData(nullptr)
	{
		SYSTEM_INFO siSysInfo;
		GetSystemInfo(&siSysInfo);
		m_AllocationGranularity = siSysInfo.dwAllocationGranularity;
		m_DataStoreHeaderSize = (sizeof(DataStoreHeader) / m_AllocationGranularity + 1) *  m_AllocationGranularity;
	}

	SharedDataStore::~SharedDataStore()
	{
		CloseConnection();
	}

	void   SharedDataStore::CloseConnection()
	{
		UnmapViewOfFile(m_pViewHeader);
		m_pViewHeader = nullptr;
		if (m_bUseOneViewPerSlot)
		{
			for (auto &vp : m_pViewDataVector)
			{
				if (vp != nullptr)
				{
					if (UnmapViewOfFile(vp))
					{
						vp = nullptr;
					}
					else
					{
						LogUnMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16235);
					}
				}
			}
		}
		else
		{
			if (UnmapViewOfFile(m_pViewData))
			{
				m_pViewData = nullptr;
			}
		}
		if (m_hMapFileImage)
		{
			if (CloseHandle(m_hMapFileImage))
			{
				m_hMapFileImage = NULL;
			}
			else
			{
				LogUnMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16235);
			}
		}
	}

	void SharedDataStore::CreateDataStore(LPCTSTR StoreName, DWORD slotsize, DWORD  slots, const SMParameterStruct& rParam )
	{
		m_StoreName = StoreName;
		m_slotCount = slots;
		BuildMapFileName();
		CloseConnection();
		if (slotsize > m_AllocationGranularity * GranularityFactor)
		{
			m_bUseOneViewPerSlot = true;
			m_slotSize = (slotsize / m_AllocationGranularity + 1) *  m_AllocationGranularity;
		}
		else
		{
			m_bUseOneViewPerSlot = false;
			m_slotSize = slotsize;
		}

		DWORD SharedSize = m_slotCount * m_slotSize + m_DataStoreHeaderSize;
		
		for (int time = 0; time <= rParam.SMCreateTimeout; time += rParam.SMCreateWaitTime)
		{
			SetLastError(0x0);
			m_hMapFileImage = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SharedSize, m_MapFileName.c_str());
			if (nullptr != m_hMapFileImage &&  GetLastError() != ERROR_ALREADY_EXISTS)
			{
				break;
			}
			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				//Wait for the clients   to closehandle to SharedMemory 
				CloseHandle(m_hMapFileImage);
				m_hMapFileImage = nullptr;
				Sleep(rParam.SMCreateWaitTime);
				continue;
			}
			else
			{
				break;
			}
		}

		if (nullptr == m_hMapFileImage)
		{
			ThrowCreateFileMappingFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
		}
		MapViewOfFileWriteInfos();
	}
	void SharedDataStore::MapViewOfFileWriteInfos()
	{

		m_pViewHeader = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, 0, m_DataStoreHeaderSize);
		if (m_pViewHeader == nullptr)
		{
			ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16227);
		}

		DataStoreHeader* pHeader = reinterpret_cast<DataStoreHeader*>(m_pViewHeader);
		pHeader->slotCount = m_slotCount;
		pHeader->slotSize = m_slotSize;
		pHeader->UseOneViewPerSlot = m_bUseOneViewPerSlot;
		if (m_bUseOneViewPerSlot)
		{
			m_pViewDataVector.resize(m_slotCount);
			for (DWORD i = 0; i < m_slotCount; i++)
			{
				DWORD offset = m_DataStoreHeaderSize + i* m_slotSize;
				m_pViewDataVector[i] = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, offset, m_slotSize);
				m_pViewData = nullptr;
				if (nullptr == m_pViewDataVector[i])
				{
					ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16228);
				}
			}
		}
		else
		{
			m_pViewDataVector.clear();
			if (m_slotSize * m_slotCount == 0)
			{
				m_pViewData = nullptr;
			}
			else
			{
				DWORD offset = m_DataStoreHeaderSize;
				m_pViewData = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, offset, m_slotSize * m_slotCount);
				if (nullptr == m_pViewData)
				{
					ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16228);
				}
			}
		}


	}

	bool SharedDataStore::OpenDataStore(LPCTSTR StoreName)
	{
		m_StoreName = StoreName;
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
				ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16230);
			}
		}
		m_pViewHeader = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, 0, m_DataStoreHeaderSize);
		if (nullptr == m_pViewHeader)
		{
			ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16231);
		}
		DataStoreHeader* pHeader = reinterpret_cast<DataStoreHeader*>(m_pViewHeader);
		m_slotCount = pHeader->slotCount;
		m_slotSize = pHeader->slotSize;
		m_bUseOneViewPerSlot = pHeader->UseOneViewPerSlot ? true: false;
		if (m_slotCount <= 0)
		{
			ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16232);
		}
		if (m_bUseOneViewPerSlot)
		{
			m_pViewData = nullptr;
			m_pViewDataVector.resize(m_slotCount);
			for (DWORD i = 0; i < m_slotCount; i++)
			{
				DWORD offset = m_DataStoreHeaderSize + i* m_slotSize;
				m_pViewDataVector[i] = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, offset, m_slotSize);

				if (m_pViewDataVector[i] == nullptr)
				{
					ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16233);
				}
			}
		}
		else
		{
			m_pViewDataVector.clear();
			if (m_slotSize * m_slotCount == 0)
			{
				m_pViewData = nullptr;
			}
			else
			{
				DWORD offset = m_DataStoreHeaderSize;
				m_pViewData = MapViewOfFile(m_hMapFileImage, FILE_MAP_ALL_ACCESS, 0, offset, m_slotSize * m_slotCount);
				if (m_pViewData == nullptr)
				{
					ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16236);
				}
			}
		}
		return true;
	}

	BYTE* SharedDataStore::GetPtr(DWORD slot, DWORD offset)
	{
		BYTE *res = nullptr;
		if (!m_hMapFileImage || slot >= m_slotCount || offset >= m_slotSize)
		{
			return NULL;
		}
		if (m_bUseOneViewPerSlot)
		{
			res = (BYTE*)m_pViewDataVector[slot];
			if (res)
			{
				res += offset;
			}
		}
		else
		{
			res = ((BYTE*)m_pViewData) + slot * m_slotSize + offset;
		}
		return res;

	}

	LPCTSTR SharedDataStore::GetMapFileName() const
	{
		return m_MapFileName.c_str();
	}
	DWORD SharedDataStore::GetSlotCount() const
	{
		return m_slotCount;
	}
	DWORD SharedDataStore::GetSlotSize() const
	{
		return m_slotSize;
	}
	void SharedDataStore::BuildMapFileName()
	{
		m_MapFileName.clear();
		m_MapFileName = _T("Global\\ImageFileMapping.");
		m_MapFileName += m_StoreName;

	}

	void SharedDataStore::ThrowCreateFileMappingFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode)
	{
		std::string LastError = SvUl::Format(_T("%s LastError:  %i"), m_MapFileName.c_str(), GetLastError());
		SvDef::StringVector msgList;
		msgList.push_back(LastError);

		SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
		MesMan.setMessage(SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
		MesMan.Throw();
	}
	void SharedDataStore::ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode)
	{
		std::string LastError = SvUl::Format(_T("%s LastError:  %i"), m_MapFileName.c_str(), GetLastError());
		SvDef::StringVector msgList;
		msgList.push_back(LastError);

		SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
		MesMan.setMessage(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
		MesMan.Throw();
	}

	void SharedDataStore::LogUnMapViewOfFileFailedFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode)
	{
		std::string LastError = SvUl::Format(_T("%s LastError:  %i"), m_MapFileName.c_str(), GetLastError());
		SvDef::StringVector msgList;
		msgList.push_back(LastError);
		SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
		MesMan.setMessage(SVMSG_SVO_5082_UNMAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
	}
}





