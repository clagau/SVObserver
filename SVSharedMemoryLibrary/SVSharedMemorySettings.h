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

namespace SvSml
{
	class SVSharedMemorySettings
	{
	private:
		long m_monitorStoreSize;	// in MB
		long m_dataStoreSize;		// in MB
		long m_productStoreSize;	// in MB

	public:
		enum Defaults
		{
			DefaultMonitorStoreSize = 4,
			DefaultProductStoreSize = 8,
			DefaultDataStoreSize = 150
		};
		SVSharedMemorySettings();
		SVSharedMemorySettings(long monitorSz, long prodStoreSz, long dataStoreSz);

		SVSharedMemorySettings(const SVSharedMemorySettings& rSettings);
		SVSharedMemorySettings& operator=(const SVSharedMemorySettings& rSettings);
		long MonitorStoreSize() const;
		long ProductStoreSize() const;
		long DataStoreSize() const;
	};

	/////////////////////////////////////////////////////////////////////////////////
	// Structure to hold the Inspection Share Creation Info
	/////////////////////////////////////////////////////////////////////////////////
	struct CreationInfo
	{
		size_t num_entries; // number of items
		size_t names_size;	// size of the names
	
		CreationInfo(size_t numEntries, size_t namesSize) : num_entries(numEntries), names_size(namesSize) {}
	};
} //namespace SvSml
