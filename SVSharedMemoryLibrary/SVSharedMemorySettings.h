//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySettings
//* .File Name       : $Workfile:   SVSharedMemorySettings.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:22  $
//******************************************************************************
#pragma once

class SVSharedMemorySettings
{
private:
	long m_monitorStoreSize;	// in MB
	long m_dataStoreSize;		// in MB
	long m_productStoreSize;	// in MB
	long m_productNumSlots;		// used for both PPQ and Inspections
	long m_rejectsNumSlots;

public:
	enum Defaults
	{
		DefaultMonitorStoreSize = 4,
		DefaultProductStoreSize = 8,
		DefaultDataStoreSize = 20,
		DefaultProductNumSlots = 64,
		DefaultRejectsNumSlots = 100
	};
	SVSharedMemorySettings();
	SVSharedMemorySettings(long monitorSz, long prodStoreSz, long dataStoreSz, long numProductSlots, long numRejectSlots);

	SVSharedMemorySettings(const SVSharedMemorySettings& rSettings);
	SVSharedMemorySettings& operator=(const SVSharedMemorySettings& rSettings);
	long MonitorStoreSize() const;
	long ProductStoreSize() const;
	long DataStoreSize() const;
	long NumProductSlots() const;
	void SetNumProductSlots(long numProductSlots);
	long NumRejectSlots() const;
	void SetNumRejectSlots(long numRejectSlots);
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedMemorySettings.h_v  $
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
 *    Rev 1.0   19 Dec 2011 13:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

