//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ClientStreamContext.h
/// All Rights Reserved
//******************************************************************************
/// TODO DOCUMENT ME
//******************************************************************************

#pragma once

namespace SvRpc
{

class ClientStreamContext
{
public:
	using OnCancelFn = std::function<void()>;

	explicit ClientStreamContext(OnCancelFn OnCancel) : m_OnCancel(OnCancel)
	{
	}

	void cancel()
	{
		if (m_OnCancel)
		{
			m_OnCancel();
		}
	}

private:
	OnCancelFn m_OnCancel;
};

} // namespace SvRpc
