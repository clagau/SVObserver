//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ResetBlocker.h
/// All Rights Reserved
//*****************************************************************************
/// The class block the reset to guarantee the work for a TR-method correctly.
//******************************************************************************
#pragma once
#include "TriggerRecordController.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\SourceFileParams.h"
#include "SVMessage/SVMessage.h"

namespace SvTrc
{
	class ResetLocker final
	{
	public:
		explicit ResetLocker(volatile long& rRefCount) : m_rRefCount(rRefCount)
		{
			InterlockedIncrement(&m_rRefCount);
		};

		~ResetLocker()
		{
			long value = InterlockedDecrement(&m_rRefCount);
			if (0 > value)
			{
				assert(false);
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocker, SvStl::SourceFileParams(StdMessageParams));
				InterlockedExchange(&m_rRefCount, 0);
			}
		};

	public:
		typedef std::unique_ptr<ResetLocker> ResetLockerPtr;
		static ResetLockerPtr lockReset(long resetId)
		{
			auto* pTRC = getTriggerRecordControllerInstance();
			if (nullptr != pTRC)
			{
				volatile long* pResetLockCounter = pTRC->getResetLockCounterRef();
				if (nullptr != pResetLockCounter)
				{
					if (0 < pTRC->getResetId())
					{
						ResetLockerPtr retValue = std::make_unique<ResetLocker>(*pResetLockCounter);
						if (pTRC->getResetId() == resetId)
						{
							return std::move(retValue);
						}
					}
				}
			}
			return nullptr;
		}

	private:
		volatile long& m_rRefCount;
	};
} //namespace SvTrc
