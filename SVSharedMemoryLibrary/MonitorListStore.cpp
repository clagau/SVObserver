//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file MonitorListStore.cpp
/// All Rights Reserved 
//*****************************************************************************
/// class for holding memory for the MonitorList in SharedMemory 
//******************************************************************************
#include "StdAfx.h"
#include "MonitorListStore.h"
#include "SVStatusLibrary\ErrorNumbers.h"
#include "Definitions\StringTypeDef.h"
#include "SVUtilityLibrary\SVMetrics.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "SVStatusLibrary\MessageTextEnum.h"
#include "MesManHelper.h"

namespace SvSml
{

MonitorListStore::MonitorListStore()
{
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	m_AllocationGranularity = siSysInfo.dwAllocationGranularity;
	if (m_AllocationGranularity == 0)
	{
		m_AllocationGranularity = 1;
	}
}

MonitorListStore::~MonitorListStore()
{
	CloseConnection();
}

void   MonitorListStore::CloseConnection()
{
	if (m_pViewData)
	{
		if (UnmapViewOfFile(m_pViewData))
		{
			m_pViewData = nullptr;
		}
		else
		{
			MesManHelper::LogUnMapViewOfFileFailed(m_MapFileName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16235);
		}
	}
	if (m_hMapFileData)
	{
		if (CloseHandle(m_hMapFileData))
		{
			m_hMapFileData = NULL;
		}
		else
		{
			MesManHelper::LogUnMapViewOfFileFailed(m_MapFileName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16235);
		}
	}
}

void MonitorListStore::CreateMonitorStore(LPCTSTR StoreName, DWORD datasize, const SMParameterStruct& rParam)
{
	m_StoreName = StoreName;
	m_DataSize = 0;
	BuildMapFileName();
	CloseConnection();
	m_SharedSize = ((datasize + sizeof(DWORD)) / m_AllocationGranularity + 1) *m_AllocationGranularity;

	for (int time = 0; time <= rParam.SMCreateTimeout; time += rParam.SMCreateWaitTime)
	{
		SetLastError(0x0);
		m_hMapFileData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_SharedSize, m_MapFileName.c_str());
		if (nullptr != m_hMapFileData &&  GetLastError() != ERROR_ALREADY_EXISTS)
		{
			break;
		}
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//Wait for the clients   to closehandle to SharedMemory 
			CloseHandle(m_hMapFileData);
			m_hMapFileData = nullptr;
			Sleep(rParam.SMCreateWaitTime);
			continue;
		}
		else
		{
			break;
		}
	}

	if (nullptr == m_hMapFileData)
	{
		MesManHelper::ThrowCreateFileMappingFailed(m_MapFileName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
	}

	m_pViewData = MapViewOfFile(m_hMapFileData, FILE_MAP_ALL_ACCESS, 0, 0, m_SharedSize);
	if (m_pViewData == nullptr)
	{
		MesManHelper::ThrowMapViewOfFileFailed(m_MapFileName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16227);
	}
	DWORD* pDatasize = reinterpret_cast<DWORD*>(m_pViewData);
	m_DataSize = datasize;
	*pDatasize = m_DataSize;

}


bool  MonitorListStore::OpenMonitorStore(LPCTSTR StoreName)
{
	m_StoreName = StoreName;
	BuildMapFileName();
	CloseConnection();
	m_hMapFileData = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_MapFileName.c_str());
	if (m_hMapFileData == nullptr)
	{
		DWORD error = GetLastError();
		if (error == ERROR_FILE_NOT_FOUND)
		{
			return false;
		}
		else
		{
			MesManHelper::ThrowMapViewOfFileFailed(m_MapFileName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16230);
		}
	}
	m_pViewData = MapViewOfFile(m_hMapFileData, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (nullptr == m_pViewData)
	{
		MesManHelper::ThrowMapViewOfFileFailed(m_MapFileName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16231);
	}

	DWORD* pDatasize = reinterpret_cast<DWORD*>(m_pViewData);
	m_DataSize = *pDatasize;
	return true;
}

BYTE* MonitorListStore::GetPtr()
{
	BYTE *pByte = nullptr;
	if (!m_pViewData || m_DataSize == 0)
	{
		return pByte;
	}
	pByte = (BYTE*)m_pViewData;
	pByte += sizeof(DWORD);
	return pByte;

}

LPCTSTR MonitorListStore::GetMapFileName() const
{
	return m_MapFileName.c_str();
}

DWORD MonitorListStore::GetSize() const
{
	return m_DataSize;
}
void MonitorListStore::BuildMapFileName()
{
	m_MapFileName.clear();
	m_MapFileName = _T("Global\\MonitorListFileMapping.");
	m_MapFileName += m_StoreName;

}


}


