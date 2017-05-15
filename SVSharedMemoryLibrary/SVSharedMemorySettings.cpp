//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySettings
//* .File Name       : $Workfile:   SVSharedMemorySettings.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:22  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedMemorySettings.h"
#pragma endregion Includes

namespace SvSml
{
	SVSharedMemorySettings::SVSharedMemorySettings()
	: m_monitorStoreSize(DefaultMonitorStoreSize)
	, m_productStoreSize(DefaultProductStoreSize)
	, m_dataStoreSize(DefaultDataStoreSize)
	{
	}

	SVSharedMemorySettings::SVSharedMemorySettings(long monitorSz, long prodStoreSize, long dataStoreSize)
	: m_monitorStoreSize(monitorSz)
	, m_productStoreSize(prodStoreSize)
	, m_dataStoreSize(dataStoreSize)
	{
	}

	SVSharedMemorySettings::SVSharedMemorySettings(const SVSharedMemorySettings& rSettings)
	: m_monitorStoreSize(rSettings.m_monitorStoreSize)
	, m_productStoreSize(rSettings.m_productStoreSize)
	, m_dataStoreSize(rSettings.m_dataStoreSize)
	{
	}

	SVSharedMemorySettings& SVSharedMemorySettings::operator=(const SVSharedMemorySettings& rSettings)
	{
		if (this != &rSettings)
		{
			m_monitorStoreSize = rSettings.m_monitorStoreSize;
			m_productStoreSize = rSettings.m_productStoreSize;
			m_dataStoreSize = rSettings.m_dataStoreSize;
		}
		return *this;
	}

	long SVSharedMemorySettings::ProductStoreSize() const
	{
		return m_productStoreSize;
	}

	long SVSharedMemorySettings::DataStoreSize() const
	{
		return m_dataStoreSize;
	}

	long SVSharedMemorySettings::MonitorStoreSize() const
	{
		return m_monitorStoreSize;
	}
} //namespace SvSml
