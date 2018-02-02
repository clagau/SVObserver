//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ResetBlocker.h
/// All Rights Reserved
//*****************************************************************************
/// The class block the reset to guarantee the work for a TR-method correctly.
//******************************************************************************
#pragma once
#include "TriggerRecordController.h"


namespace SvTRC
{
	class ResetLocker
	{
	private:
		ResetLocker(long& rRefCount) : m_rRefCount(rRefCount) 
		{
			long value = m_rRefCount;
			{
				value = m_rRefCount;
			}
			while (InterlockedCompareExchange(&m_rRefCount, value + 1, value) == value);
		};

	public:
		~ResetLocker()
		{
			long value = m_rRefCount;
			{
				value = m_rRefCount;
			}
			while (InterlockedCompareExchange(&m_rRefCount, value - 1, value) == value);
		};

	public:
		typedef std::unique_ptr<ResetLocker> ResetLockerPtr;
		static ResetLockerPtr lockReset(time_t resetTime)
		{
			static auto& controller = TriggerRecordController::getTriggerRecordControllerInstance();
			long& resetLockCounter = controller.getResetLockCounterRef();
			ResetLockerPtr retValue = ResetLockerPtr(new ResetLocker(resetLockCounter));
			if (controller.getResetTime() == resetTime)
			{
				return retValue;
			}
			return ResetLockerPtr();
		}

	private:
		long& m_rRefCount;
	};
} //namespace SvTRC