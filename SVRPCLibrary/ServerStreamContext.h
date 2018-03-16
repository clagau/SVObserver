//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file ServerStreamContext.h
/// All Rights Reserved
//******************************************************************************
/// TODO DOCUMENT ME
//******************************************************************************

#pragma once

#include <atomic>
#include <memory>

namespace SVRPC
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

private:
	std::atomic_bool m_bIsCancelled;
	std::function<void()> m_OnCancel;
};

} // namespace SVRPC
