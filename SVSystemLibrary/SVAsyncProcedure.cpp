//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncProcedure
//* .File Name       : $Workfile:   SVAsyncProcedure.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVAsyncProcedure.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

namespace SvSyl
{
SVAsyncProcedure::~SVAsyncProcedure()
{
	Destroy();
}

HRESULT SVAsyncProcedure::Create(PAPCFUNC apcHandler, LPCTSTR tag)
{
	m_tag = tag;
	m_apcHandler = apcHandler;

	HRESULT hr = m_thread.Create(m_tag.c_str());
	return hr;
}

HRESULT SVAsyncProcedure::Signal(const void* const pData)
{
	SvStl::MessageManager Exception(SvStl::MsgType::Log );

	HRESULT Result = S_OK;

	
	if (m_thread.IsDisabled())
	{
		return Result;
	}

	if( S_OK == Result )
	{
		if( 0 == ::QueueUserAPC(m_apcHandler, m_thread.GetThreadHandle(), reinterpret_cast<ULONG_PTR>(pData)))
		{
			Exception.setMessage( SVMSG_QUEUE_USER_APC_ERROR, m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
		Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams));
	}

	return Result;
}

void SVAsyncProcedure::Destroy()
{
	m_thread.Destroy();
}

int SVAsyncProcedure::GetPriority() const
{
	return m_thread.GetPriority();
}

void SVAsyncProcedure::SetPriority(int priority)
{
	m_thread.SetPriority(priority);
}

bool SVAsyncProcedure::IsActive() const
{
	return m_thread.IsActive();
}
} //namespace SvSyl