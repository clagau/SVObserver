//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncProcedure
//* .File Name       : $Workfile:   SVAsyncProcedure.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVMessage\SVMessage.h"

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::SVAsyncProcedure()
{
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::~SVAsyncProcedure()
{
	Destroy();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
HRESULT SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::Create(const SVAPCSignalHandler& apcHandler, const SVThreadSignalHandler& threadHandler, LPCTSTR tag, SVThreadAttribute eAttribute )
{
	m_tag = tag;
	m_apcHandler = apcHandler;

	HRESULT hr = m_thread.Create(threadHandler, m_tag.c_str(), eAttribute);
	return hr;
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
HRESULT SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::Signal(void* pData)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	HRESULT Result = S_OK;

	if( ! m_thread.IsActive() )
	{
		Exception.setMessage( SVMSG_THREAD_CREATION_ERROR, m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25037_AsyncProcedure );

		Result = m_thread.Restart();
	}

	if( S_OK == Result )
	{
		if( 0 == ::QueueUserAPC( m_apcHandler, m_thread.GetThreadHandle(), reinterpret_cast<ULONG_PTR>(pData) )  )
		{
			Exception.setMessage( SVMSG_QUEUE_USER_APC_ERROR, m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25038_AsyncProcedure );

			Result = m_thread.Restart();

			if( S_OK == Result )
			{
				if( 0 == ::QueueUserAPC( m_apcHandler, m_thread.GetThreadHandle(), reinterpret_cast<ULONG_PTR>(pData) )  )
				{
					Result = SVMSG_QUEUE_USER_APC_ERROR;

					Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25039_AsyncProcedure );
				}
			}
			else
			{
				Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25040_AsyncProcedure );
			}
		}
	}
	else
	{
		Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25041_AsyncProcedure );
	}

	return Result;
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
void SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::Destroy()
{
	m_thread.Destroy();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
unsigned long SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::GetThreadID() const
{
	return m_thread.GetThreadID();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
int SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::GetPriority() const
{
	return m_thread.GetPriority();
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
void SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::SetPriority(int priority)
{
	m_thread.SetPriority(priority);
}

template<typename SVAPCSignalHandler, typename SVThreadSignalHandler>
bool SVAsyncProcedure<SVAPCSignalHandler, SVThreadSignalHandler>::IsActive() const
{
	return m_thread.IsActive();
}

