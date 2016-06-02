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

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedMemorySettings::SVSharedMemorySettings()
	: m_monitorStoreSize(DefaultMonitorStoreSize)
	, m_productStoreSize(DefaultProductStoreSize)
	, m_dataStoreSize(DefaultDataStoreSize)
	, m_productNumSlots(DefaultProductNumSlots)
	, m_rejectsNumSlots(DefaultRejectsNumSlots)
	{
	}

	SVSharedMemorySettings::SVSharedMemorySettings(long monitorSz, long prodStoreSize, long dataStoreSize, long numProductSlots, long numRejectSlots)
	: m_monitorStoreSize(monitorSz)
	, m_productStoreSize(prodStoreSize)
	, m_dataStoreSize(dataStoreSize)
	, m_productNumSlots(numProductSlots)
	, m_rejectsNumSlots(numRejectSlots)
	{
	}

	SVSharedMemorySettings::SVSharedMemorySettings(const SVSharedMemorySettings& rSettings)
	: m_monitorStoreSize(rSettings.m_monitorStoreSize)
	, m_productStoreSize(rSettings.m_productStoreSize)
	, m_dataStoreSize(rSettings.m_dataStoreSize)
	, m_productNumSlots(rSettings.m_productNumSlots)
	, m_rejectsNumSlots(rSettings.m_rejectsNumSlots)
	{
	}

	SVSharedMemorySettings& SVSharedMemorySettings::operator=(const SVSharedMemorySettings& rSettings)
	{
		if (this != &rSettings)
		{
			m_monitorStoreSize = rSettings.m_monitorStoreSize;
			m_productStoreSize = rSettings.m_productStoreSize;
			m_dataStoreSize = rSettings.m_dataStoreSize;
			m_productNumSlots = rSettings.m_productNumSlots;
			m_rejectsNumSlots = rSettings.m_rejectsNumSlots;
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

	long SVSharedMemorySettings::NumProductSlots() const
	{
		return m_productNumSlots;
	}

	void SVSharedMemorySettings::SetNumProductSlots(long numProductSlots)
	{
		m_productNumSlots = numProductSlots;
	}

	long SVSharedMemorySettings::NumRejectSlots() const
	{
		return m_rejectsNumSlots;
	}

	void SVSharedMemorySettings::SetNumRejectSlots(long numRejectSlots)
	{
		m_rejectsNumSlots = numRejectSlots;
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
