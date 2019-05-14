//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ServerStreamContext.h
/// All Rights Reserved
//******************************************************************************
/// TODO DOCUMENT ME
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <atomic>
//Moved to precompiled header: #include <memory>

#include "SVSystemLibrary/SVFuture.h"

namespace SvRpc
{

class ServerStreamContext
{
public:
	using Ptr = std::shared_ptr<ServerStreamContext>;

	ServerStreamContext() : m_bIsCancelled(false)
	{
	}

	void cancel()
	{
		if (isCancelled())
		{
			return;
		}
		m_bIsCancelled.store(true);
		if (m_OnCancel)
		{
			m_OnCancel();
		}
	}

	bool isCancelled() const
	{
		return m_bIsCancelled.load();
	}

	void registerOnCancelHandler(std::function<void()> OnCancel)
	{
		m_OnCancel = OnCancel;
	}

	void enableThrottling(bool v = true)
	{
		m_bThrottlingEnabled = v;
	}

	bool isThrottlingEnabled() const
	{
		return m_bThrottlingEnabled;
	}

	void setMaxPendingAcks(size_t v)
	{
		m_maxPendingAcks = v;
	}

private:
	friend class RPCServer;
	std::tuple<bool, uint64_t, SvSyl::SVFuture<void>> new_throttled_response()
	{
		if (!m_bThrottlingEnabled)
		{
			throw new std::runtime_error("Do not call this function when throttling not enabled");
		}

		if (m_PendingAcks.size() > m_maxPendingAcks)
		{
			return {false, 0, SvSyl::SVFuture<void>::make_ready()};
		}

		auto seqNr = m_nextSeqNr++;
		auto promise = std::make_shared<SvSyl::SVPromise<void>>();
		m_PendingAcks.push_back({seqNr, promise});
		return {true, seqNr, promise->get_future()};
	}

	void on_ack(uint64_t seqNr)
	{
		for (auto it = m_PendingAcks.begin(); it != m_PendingAcks.end(); ++it)
		{
			if (it->SeqNr == seqNr)
			{
				auto promise = it->Promise;
				m_PendingAcks.erase(it);
				promise->set_value();
				return;
			}
		}
	}

private:
	struct PendingAck
	{
		uint64_t SeqNr;
		std::shared_ptr<SvSyl::SVPromise<void>> Promise;
	};
	size_t m_maxPendingAcks {0};
	uint64_t m_nextSeqNr {0};
	std::vector<PendingAck> m_PendingAcks;
	std::atomic_bool m_bThrottlingEnabled {false};
	std::atomic_bool m_bIsCancelled;
	std::function<void()> m_OnCancel;
};

} // namespace SvRpc
