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

#pragma warning (push,2)
#include "SVProtoBuf/envelope.pb.h"
#pragma warning (pop)

namespace SVRPC
{
template <typename T> class Task
{
public:
	using FinishFkt = std::function<void(T&& t)>;
	using ErrorFkt = std::function<void(const Error&)>;
	
	Task(FinishFkt finish, ErrorFkt error) : m_finish(finish), m_error(error)
	{
	}
	
void finish(T&& t) const
	{
		if (m_finish)
		{
			m_finish(std::move(t));
		}
	}
	void error(const Error& err) const
	{
		if (m_error)
		{
			m_error(std::move(err));
		}
	}

public:
	std::function<void(T&& t)> m_finish;
	std::function<void(const Error&)> m_error;
};
}
