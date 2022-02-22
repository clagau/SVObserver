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
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

namespace SvSyl
{
SVAsyncProcedure::SVAsyncProcedure()
{
}

SVAsyncProcedure::~SVAsyncProcedure()
{
	Destroy();
}

HRESULT SVAsyncProcedure::Create(PAPCFUNC apcHandler, const ProcessThread& rProceesThread, LPCTSTR tag, SVThreadAttribute eAttribute )
{
	m_tag = tag;
	m_apcHandler = apcHandler;

	HRESULT hr = m_thread.Create(rProceesThread, m_tag.c_str(), eAttribute);
	return hr;
}

HRESULT SVAsyncProcedure::Signal(void* pData)
{
	SvStl::MessageManager Exception(SvStl::MsgType::Log );

	HRESULT Result = S_OK;

	
	if (m_thread.IsDisabled())
	{
		return Result;
	}

	if( ! m_thread.IsActive() )
	{
		Exception.setMessage( SVMSG_THREAD_CREATION_ERROR, m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25037_AsyncProcedure );

		Result = m_thread.Restart();
	}

	if( S_OK == Result )
	{
		if( 0 == ::QueueUserAPC( m_apcHandler, m_thread.GetThreadHandle(), reinterpret_cast<ULONG_PTR>(pData) )  )
		{
			Exception.setMessage( SVMSG_QUEUE_USER_APC_ERROR, m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25038_AsyncProcedure );

			Result = m_thread.Restart();

			if( S_OK == Result )
			{
				if( 0 == ::QueueUserAPC( m_apcHandler, m_thread.GetThreadHandle(), reinterpret_cast<ULONG_PTR>(pData) )  )
				{
					Result = SVMSG_QUEUE_USER_APC_ERROR;

					Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25039_AsyncProcedure );
				}
			}
			else
			{
				Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25040_AsyncProcedure );
			}
		}
	}
	else
	{
		Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25041_AsyncProcedure );
	}

	return Result;
}

void SVAsyncProcedure::Destroy()
{
	m_thread.Destroy();
}

unsigned long SVAsyncProcedure::GetThreadID() const
{
	return m_thread.GetThreadID();
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