//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	: m_NumProductSlot(DefaultNumProductSlot),
	m_CreateTimeout(DefaultConnectionTimout),
	m_CreateWaitTime(DefaultCreateWaitTime)
	{
	}

	int SVSharedMemorySettings::GetNumProductSlot() const
	{
		return m_NumProductSlot;
	}
	void  SVSharedMemorySettings::SetNumProductSlot(int Size)
	{
		m_NumProductSlot = Size;
	}
	int SVSharedMemorySettings::GetCreateTimeout() const
	{
		return m_CreateTimeout;
	}
	void  SVSharedMemorySettings::SeCreateTimout(int StoreSize)
	{
		m_CreateTimeout = StoreSize;
	}
	int SVSharedMemorySettings::GetCreateWaitTime() const
	{
		return m_CreateWaitTime;
	}
	void  SVSharedMemorySettings::SetCreateWaitTime(int StoreSize)
	{
		m_CreateWaitTime = StoreSize;
	}

	
} //namespace SvSml
