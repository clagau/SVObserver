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

#include "stdafx.h"
#include "SVSharedMemorySettings.h"

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedMemorySettings.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:09:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:40:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/