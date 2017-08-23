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
	public:
		static const int 	DefaultMonitorStoreSize = 150;
		static const int 	DefaultNumProductSlot = 48;
		static const int	DefaultNumRejectSlotDelta = 5;
		static const int	DefaultConnectionTimout = 2000;
		static const int	DefaultCreateWaitTime = 200;

		SVSharedMemorySettings();
		int GetMonitorStoreSize() const;
		int GetNumProductSlot() const;
		int GetCreateTimeout() const;
		int GetCreateWaitTime() const;
		
		void SetMonitorStoreSize(int) ;
		void SetNumProductSlot(int) ;
		void SeCreateTimout(int);
		void SetCreateWaitTime(int);
		
	private:
		int m_MonitorStoreSize;	// in MB
		int m_NumProductSlot ;
		int m_CreateTimeout;
		int  m_CreateWaitTime;;
	};
} //namespace SvSml
