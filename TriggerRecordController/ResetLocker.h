//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ResetBlocker.h
/// All Rights Reserved
//*****************************************************************************
/// The class block the reset to guarantee the work for a TR-method correctly.
//******************************************************************************
#pragma once
#include "TriggerRecordController.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\SourceFileParams.h"
#include "SVMessage\SVMessage.h"

namespace SvTrc
{
	class ResetLocker final
	{
	public:
		ResetLocker(long& rRefCount) : m_rRefCount(rRefCount)
		{
			InterlockedIncrement(&m_rRefCount);
		};

		~ResetLocker()
		{
			long value = InterlockedDecrement(&m_rRefCount);
			if (0 > value)
			{
				assert(false);
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocker, SvStl::SourceFileParams(StdMessageParams));
				InterlockedExchange(&m_rRefCount, 0);
			}
		};

	public:
		typedef std::unique_ptr<ResetLocker> ResetLockerPtr;
		static ResetLockerPtr lockReset(long resetId)
		{
			static auto& controller = getTriggerRecordControllerInstance();
			long* pResetLockCounter = controller.getResetLockCounterRef();
			if (nullptr != pResetLockCounter)
			{
				ResetLockerPtr retValue = std::make_unique<ResetLocker>(*pResetLockCounter);
				if (controller.getResetId() == resetId)
				{
					return std::move(retValue);
				}
			}
			return nullptr;
		}

	private:
		long& m_rRefCount;
	};
} //namespace SvTrc
