//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManagerImpl
//* .File Name       : $Workfile:   SVThreadManagerImpl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   09 Dec 2014 13:48:16  $
//******************************************************************************

#include "stdafx.h"
#include "sstream"
#include "SVThreadManagerImpl.h"

SVThreadManagerImpl& SVThreadManagerImpl::Instance()
{
	static SVThreadManagerImpl l_mgr;
	return l_mgr;
}


SVThreadManagerImpl::SVThreadManagerImpl()
	:m_hShutdown(nullptr)
	,m_hThread(nullptr)
	,m_hThreadComplete(nullptr)
{
	DWORD_PTR systemAffinityMask;
	DWORD_PTR myCurrentMask;

	BOOL bRet = ::GetProcessAffinityMask( GetCurrentProcess(), &systemAffinityMask, &myCurrentMask);	
	if( bRet )
	{
		// count the number of processors.
		DWORD_PTR i =  1;
		for( ; i < 64 ; i++ )
		{
			if( static_cast<DWORD_PTR>((1 << i)) > systemAffinityMask )
			{
				break;
			}
		}
		m_lNumPipes = static_cast<long>(i);
	}
	Create();
}

SVThreadManagerImpl::~SVThreadManagerImpl()
{
	Destroy();
}


HRESULT SVThreadManagerImpl::GetThreadInfo( std::list<std::string>& p_rStrList)
{
	HRESULT hr = S_OK;
	p_rStrList.clear();
	for( ThreadList::iterator l_it = m_threads.begin() ; l_it != m_threads.end() ; ++l_it)
	{
		HANDLE l_hThread = l_it->m_hThread;
		if( l_hThread == nullptr)
			continue;
		int iPriority = ::GetThreadPriority(l_hThread);
		if( iPriority == THREAD_PRIORITY_ERROR_RETURN )
		{
			continue;
		}

		std::ostringstream l_stream;
		l_stream << l_it->m_strName << "," ;
		if( (l_it->m_eAttribute & SVAffinityEditAllowed) == SVAffinityEditAllowed )
		{
			l_stream << l_it->m_lAffinity;
		}
		else
		{
			l_stream << "-";
		}
		l_stream << "," << iPriority << "," <<l_hThread;
		p_rStrList.push_back(l_stream.str());
	}
	return hr;
}

// Gets a list of SVThreadSetup filtered by Attribute.
HRESULT SVThreadManagerImpl::GetThreadInfo( std::list<SVThreadSetup>& rSetupList, SVThreadAttribute eFilter )
{
	HRESULT hr = S_OK;
	rSetupList.clear();
	for( ThreadList::iterator l_it = m_threads.begin() ; l_it != m_threads.end() ; ++l_it)
	{
		if( ((l_it->m_eAttribute & eFilter) == eFilter) /*&& (l_it->m_hThread != nullptr)*/)
		{
			rSetupList.push_back( *l_it );
		}
	}
	return hr;
}

// Gets a list of SVThreadSetup filtered by Attribute.
HRESULT SVThreadManagerImpl::IsAllowed( LPCTSTR strName,  SVThreadAttribute eAttrib )
{
	HRESULT hr = S_FALSE;
	for( ThreadList::iterator l_it = m_threads.begin() ; l_it != m_threads.end() ; ++l_it)
	{
		if( (l_it->m_strName == strName ))
		{
			if( (l_it->m_eAttribute & eAttrib)== eAttrib)
			{
				hr = S_OK;
				break;
			}
		}
	}
	return hr;
}


// Add - used to add the handle to the thread manager.
// If a name and Handle already exists then we have a duplicate name.
HRESULT SVThreadManagerImpl::Add( HANDLE hThread, LPCTSTR strName, SVThreadAttribute eAttrib )
{
	HRESULT hr = S_OK;
	std::string Name(strName);
	SVThreadSetup newThread;
	ThreadList::iterator it = m_threads.begin();
	for(  ; it != m_threads.end() ; ++it )
	{
		if( it->m_strName == Name )
		{
			if( it->m_hThread == nullptr )
			{
				break;
			}
		}
	}

	// Special cases for preset thread affinity here...
	if( (eAttrib & SVAffinityPPQ ) == SVAffinityPPQ )
	{
		SetNewAffinity(hThread, 1 );
		newThread.m_lAffinity = 1;
	}

	// Special cases for preset thread affinity here...
	if( (eAttrib & SVAffinityAcq ) == SVAffinityAcq )
	{
		SetNewAffinity(hThread, 1 );
		newThread.m_lAffinity = 1;
	}

	// Special cases for preset thread affinity
	if( (eAttrib & SVAffinityDummy ) == SVAffinityDummy )
	{
		int Pipe = 2;  // from JB requirement. According to Jim, Attempt to keep windows off this pipe.
		SetNewAffinity(hThread, Pipe );
		newThread.m_lAffinity = Pipe;
	}



	if( it == m_threads.end() )
	{
		newThread.m_eAttribute = eAttrib;
		newThread.m_hThread = hThread;
		newThread.m_strName = Name;
		m_threads.push_back(newThread);
	}
	else
	{
		if( it->m_hThread != nullptr )	// Duplicate Name...
		{
			newThread.m_eAttribute = eAttrib;
			newThread.m_hThread = hThread;
			newThread.m_strName = Name;
			m_threads.push_back(newThread);
		}
		else
		{
			it->m_hThread = hThread;
			newThread = *it;
		}

		if( (it->m_eAttribute & SVAffinityEditAllowed) == SVAffinityEditAllowed )
		{
			SetNewAffinity(hThread, it->m_lAffinity);
		}
	}
	return hr;
}

// setup is used to restore data from the configuration.
// At this point only the name and affinity are stored.
HRESULT SVThreadManagerImpl::Setup( LPCTSTR strName, long Affinity )
{
	HRESULT hr = S_OK;
	std::string Name(strName);


	SVThreadSetup newThread;
	newThread.m_eAttribute = SVAffinityEditAllowed;
	newThread.m_hThread = nullptr;
	newThread.m_strName = Name;
	newThread.m_lAffinity = Affinity;
	m_threads.push_back(newThread);
	return hr;
}

long SVThreadManagerImpl::GetPipeCount( )
{
	return m_lNumPipes;
}

// Set new affinity 
BOOL SVThreadManagerImpl::SetNewAffinity( HANDLE hThread, DWORD_PTR dwAffinityBitNum )
{
	// Set Thread Affinity
	DWORD_PTR myCurrentMask;
	DWORD_PTR systemAffinityMask;
	DWORD_PTR newAffinityBits;
	BOOL bRet = FALSE;
	if( dwAffinityBitNum > 0 )
		dwAffinityBitNum--;

	if( hThread != nullptr )
	{
		bRet = ::GetProcessAffinityMask( GetCurrentProcess(), &systemAffinityMask, &myCurrentMask);
		if( bRet )
		{
			if( dwAffinityBitNum >= 0 )
			{
				newAffinityBits = static_cast<unsigned long long>(1) << dwAffinityBitNum;
				if( (newAffinityBits | myCurrentMask) == myCurrentMask )
				{
					DWORD_PTR dwPreviousAff = ::SetThreadAffinityMask(hThread, newAffinityBits );
				}
				else
				{
					bRet = FALSE;
					//assert(0);
				}
			}
			else
			{
				::SetThreadAffinityMask( hThread, myCurrentMask ); // Set to all processors.
			}
		}
	}
	return bRet;
}

BOOL SVThreadManagerImpl::SetAffinity( LPCTSTR ThreadName, DWORD_PTR dwAffinityBitNum )
{
	BOOL bRet = FALSE;
	for( ThreadList::iterator it = m_threads.begin() ; it != m_threads.end() ; ++it)
	{
		if( it->m_strName == ThreadName )
		{
			it->m_lAffinity = static_cast<long>(dwAffinityBitNum);
			bRet = SetNewAffinity( it->m_hThread, dwAffinityBitNum);
		}
	}
	return bRet;
}

// Remove just clears the handle
// the information stays.
HRESULT SVThreadManagerImpl::Remove( HANDLE p_hThread )
{
	HRESULT hr = S_FALSE;

	for( ThreadList::iterator it = m_threads.begin() ; it != m_threads.end() ; ++it)
	{
		if( it->m_hThread == p_hThread )
		{
			it->m_hThread = nullptr;
			//m_threads.remove( *it );
			hr = S_OK;
			break;
		}
	}
	return hr;
}

// Clear removes all entries with a null handle
HRESULT SVThreadManagerImpl::Clear( )
{
	HRESULT hr = S_OK;

	for( ThreadList::iterator it = m_threads.begin() ; it != m_threads.end() ; )
	{
		if( it->m_hThread == nullptr )
		{
			it = m_threads.erase( it );
		}
		else
		{
			++it;
		}
	}
	return hr;
}


HRESULT SVThreadManagerImpl::Create()
{
	HRESULT hr=S_FALSE;

	if( m_hShutdown == nullptr)
	{
		m_hShutdown = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
	}
	else
	{
		return hr;
	}
	if( m_hThreadComplete == nullptr)
	{
		m_hThreadComplete = ::CreateEvent( nullptr, TRUE, TRUE, nullptr );
	}
	else
	{
		return hr;
	}


	DWORD dwThreadID=0;
	m_hThread = ::CreateThread( nullptr, 0, &SVThreadManagerImpl::Process, LPVOID(this),0, &dwThreadID );
	if( m_hThread != nullptr)
	{
		hr = Add( m_hThread, _T("Dummy Thread"), SVAffinityDummy );
	}
	return hr;
}

void SVThreadManagerImpl::Destroy()
{
	if( m_hThread )
	{
		if( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT )
		{
			::SetEvent( m_hShutdown );

			if( ::WaitForSingleObject( m_hThreadComplete, 5000 ) != WAIT_OBJECT_0 )
			{
				DWORD l_ErrorCode = GetLastError();

				::TerminateThread( m_hThread, E_FAIL );

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( ::WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT )
			{
				DWORD l_ErrorCode = GetLastError();

				::TerminateThread( m_hThread, E_FAIL );
			}
		}

		::CloseHandle( m_hThread );
		SVThreadManagerImpl::Instance().Remove( m_hThread );
		m_hThread = nullptr;
	}

	if( m_hThreadComplete )
	{
		::CloseHandle( m_hThreadComplete );
		m_hThreadComplete = nullptr;
	}

	if( m_hShutdown )
	{
		::CloseHandle( m_hShutdown );
		m_hShutdown = nullptr;
	}
}

// Dummy Thread as per Jim Brown to 
// take up space and do nothing...???
DWORD WINAPI SVThreadManagerImpl::Process(LPVOID lpParam)
{
	HRESULT hrOk=S_OK;
	if(lpParam)
	{
		SVThreadManagerImpl* pThread = static_cast<SVThreadManagerImpl*>(lpParam);

		::ResetEvent( pThread->m_hThreadComplete );

		bool l_Continue = true;
		bool l_WaitForEvents = true;

		while( l_Continue )
		{
			if( l_WaitForEvents )
			{
				l_Continue = ::WaitForSingleObjectEx( pThread->m_hShutdown, INFINITE, TRUE ) == WAIT_IO_COMPLETION;
			}

			if( l_Continue )
			{
				l_Continue = ::WaitForSingleObjectEx( pThread->m_hShutdown, 0, FALSE ) == WAIT_TIMEOUT;

				if( l_Continue )
				{
					l_WaitForEvents = true;

					//pThread->m_threadHandler( l_WaitForEvents );
				}
			}
		}


		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		DWORD errorCode = GetLastError();

		hrOk = -1;

	}

	return hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVThreadManager\SVThreadManagerImpl.cpp_v  $
 * 
 *    Rev 1.2   09 Dec 2014 13:48:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed hard coded dummy thread to be on pipe 2 as per Jim's modifications of the IF00100.S960.002-InterfaceSpecificationForPipeThreadManagement.doc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2014 07:25:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 64 Bit Platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Dec 2014 11:21:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
