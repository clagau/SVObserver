//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Task.h
/// All Rights Reserved
//******************************************************************************
/// The Task is very much like a future. It is an handle for an async operation.
/// It can be fulfilled using the finish function or it can forward an error.
/// normaly t is response class
//******************************************************************************

#pragma once


#include <functional>

#include "SVProtoBuf/Envelope.h"

namespace SvRpc
{
template <typename T>
class Task
{
public:
	using OnFinishFn = std::function<void(T&& t)>;
	using OnErrorFn = std::function<void(const SvPenv::Error&)>;
	
	Task(OnFinishFn OnFinish, OnErrorFn OnError) : m_OnFinish(OnFinish), m_OnError(OnError)
	{
	}
	
	void finish(T&& t) const
	{
		if (m_OnFinish)
		{
			m_OnFinish(std::move(t));
		}
	}

	void error(const SvPenv::Error& rError) const
	{
		if (m_OnError)
		{
			m_OnError(std::move(rError));
		}
	}

public:
	OnFinishFn m_OnFinish;
	OnErrorFn m_OnError;
};

} // namespace SvRpc
