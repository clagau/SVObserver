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

namespace Seidenader { namespace SVSharedMemoryLibrary
{
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
			DefaultDataStoreSize = 150,
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

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
