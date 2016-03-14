//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManager
//* .File Name       : $Workfile:   SVThreadManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Dec 2014 09:34:06  $
//******************************************************************************

#include "stdafx.h"
#include "sstream"
#include "SVThreadManager.h"
#include "SVMessage/SVMessage.h"

SVThreadManager& SVThreadManager::Instance()
{
	static SVThreadManager l_mgr;
	return l_mgr;
}


SVThreadManager::SVThreadManager()
: m_pGetThreadInfo(nullptr)
 ,m_pGetThreadInfoFilter(nullptr)
 ,m_pIsAllowed(nullptr)
 ,m_pAdd(nullptr)
 ,m_pSetup(nullptr)
 ,m_pSetAffinity(nullptr)
 ,m_pRemove(nullptr)
 ,m_pClear(nullptr)
 ,m_bThreadAffinityEnabled(FALSE)
 ,m_bThreadManagerInstalled(FALSE)
{
	::InitializeCriticalSection(&m_CritSec);
	// Temperary place to de-activate SVThreadManager
	m_bThreadManagerInstalled = GetPrivateProfileInt( _T( "Settings" ), _T( "ThreadManagerActive" ), 1, _T( "c:\\SVObserver\\Bin\\SVim.ini" ) );
	if( m_bThreadManagerInstalled )
	{
		Create();
	}
}

SVThreadManager::~SVThreadManager()
{
	Destroy();
}


HRESULT SVThreadManager::GetThreadInfo( std::list<std::string>& p_rStrList) const
{
	HRESULT hr = S_FALSE;
	if( m_pGetThreadInfo != nullptr )
	{
		BSTR List;
		hr = m_pGetThreadInfo( &List );
		_bstr_t bstList;
		bstList.Attach(List);
	}
	return hr;
}

// Gets a list of SVThreadSetup filtered by Attribute.
HRESULT SVThreadManager::GetThreadInfo( std::list<SVThreadSetup>& rSetupList, SVThreadAttribute eFilter )
{
	HRESULT hr = S_FALSE;
	if( m_pGetThreadInfoFilter != nullptr )
	{
		BSTR List=nullptr;
		hr = m_pGetThreadInfoFilter( &List, eFilter );
		_bstr_t bstList;
		bstList.Attach(List); // Auto delete the bstr.
		// build ThreadSetup list
		char* pList = bstList;
		char *next_token = nullptr;
		char *token1 = nullptr;
		char seps[]="\n\r";
		token1 = strtok_s(pList, seps, &next_token);
		std::list<char*> pLines;
		while( token1 != nullptr )
		{
			// parse individual lines.
			pLines.push_back( token1 );
			token1 = strtok_s(nullptr, seps, &next_token);
		}
		for( std::list<char*>::const_iterator it = pLines.begin() ; it != pLines.end() ; ++it )
		{
			SVThreadSetup tsu;
			next_token = nullptr;
			token1 = strtok_s(*it, ",", &next_token);
			if( token1 != nullptr) // Name
			{
				tsu.m_strName = token1;
			}
			token1 = strtok_s(nullptr, ",", &next_token);
			if( token1 != nullptr) // Attribute
			{
				tsu.m_eAttribute = static_cast<SVThreadAttribute>(atol(token1));
			}
			token1 = strtok_s(nullptr, ",", &next_token);
			if( token1 != nullptr) // Affinity
			{
				tsu.m_lAffinity = atol(token1);
			}
			token1 = strtok_s(nullptr, ",", &next_token);
			if( token1 != nullptr) // Handle
			{
				tsu.m_hThread = reinterpret_cast<HANDLE>(atol(token1)); // 
			}
			rSetupList.push_back( tsu );
		}

	}
	return hr;
}

// Gets a list of SVThreadSetup filtered by Attribute.
HRESULT SVThreadManager::IsAllowed( LPCTSTR strName,  SVThreadAttribute eAttrib )
{
	HRESULT hr = S_FALSE;
	if( m_pIsAllowed != nullptr )
	{
		_bstr_t bstName(strName);
		hr = m_pIsAllowed(bstName.Detach(), eAttrib );
	}
	return hr;
}


// Add - used to add the handle to the thread manager.
// If a name and Handle already exists then we have a duplicate name.
HRESULT SVThreadManager::Add( HANDLE hThread, LPCTSTR strName, SVThreadAttribute eAttrib )
{
	HRESULT hr = S_FALSE;
	::EnterCriticalSection( &m_CritSec );
	if( m_pAdd != nullptr )
	{
		_bstr_t bstName(strName);
		hr = m_pAdd(hThread, bstName.Detach(), eAttrib);
	}
	::LeaveCriticalSection( &m_CritSec );
	return hr;
}

// setup is used to restore data from the configuration.
// At this point only the name and affinity are stored.
HRESULT SVThreadManager::Setup( LPCTSTR strName, long Affinity )
{
	HRESULT hr = S_FALSE;
	long lPipeCount = 0;
	GetPipeCount(lPipeCount);
	if( lPipeCount >= 8 )
	{
		if( m_pSetup != nullptr )
		{
			_bstr_t bstName(strName);
			hr = m_pSetup(bstName.Detach(), Affinity);
		}
	}
	return hr;
}

BOOL SVThreadManager::GetThreadAffinityEnabled() const
{
	return m_bThreadAffinityEnabled;
}

void SVThreadManager::SetThreadAffinityEnabled( BOOL bEnable )
{
	HRESULT hr = S_FALSE;
	if( bEnable && !m_bThreadAffinityEnabled )
	{
		if( m_pStartAffinityMgnt != nullptr )
		{
			hr = m_pStartAffinityMgnt();
		}
	}
	if( !bEnable && m_bThreadAffinityEnabled )
	{
		if(m_pStopAffinityMgnt!= nullptr)
		{
			hr = m_pStopAffinityMgnt();
		}
	}
	if( hr == S_OK )
	{
		m_bThreadAffinityEnabled = bEnable;
	}
}

BOOL SVThreadManager::IsThreadManagerInstalled() const
{
	return m_bThreadManagerInstalled;
}


HRESULT SVThreadManager::GetPipeCount( long& lrPipeCount)
{
	HRESULT hr = S_FALSE;
	if( m_pGetPipeCount != nullptr )
	{
		long value = 0;
		hr = m_pGetPipeCount( &value );
		if( hr == S_OK )
		{
			lrPipeCount = value;
		}
	}
	return hr;
}

BOOL SVThreadManager::SetAffinity( LPCTSTR ThreadName, DWORD_PTR dwAffinityBitNum )
{
	BOOL bRet = FALSE;
	if(m_pSetAffinity != nullptr)
	{
		_bstr_t bstName(ThreadName);
		bRet = m_pSetAffinity(bstName.Detach(), dwAffinityBitNum);
	}
	return bRet;
}

// Remove just clears the handle
// the information stays.
HRESULT SVThreadManager::Remove( HANDLE p_hThread )
{
	HRESULT hr = S_FALSE;
	::EnterCriticalSection( &m_CritSec );
	if(m_pRemove != nullptr)
	{
		hr = m_pRemove(p_hThread);
	}
	::LeaveCriticalSection( &m_CritSec );
	return hr;
}

// Remove just clears the handle
// the information stays.
HRESULT SVThreadManager::Clear( )
{
	HRESULT hr = S_FALSE;
	if(m_pClear != nullptr)
	{
		hr = m_pClear();
	}
	return hr;
}

HRESULT SVThreadManager::Create()
{
	HRESULT hr=S_FALSE;
	// LoadLibrary
	m_hThreadManagerDll = LoadLibrary(_T("SVThreadManager.dll"));
	if( m_hThreadManagerDll != nullptr)
	{
		m_pGetThreadInfo = reinterpret_cast<GetThreadInfoPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVGetThreadInfo")));
		m_pGetThreadInfoFilter = reinterpret_cast<GetThreadInfoFilterPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVGetThreadInfoFilter")));
		m_pIsAllowed = reinterpret_cast<IsAllowedPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVIsAllowed")));
		m_pAdd = reinterpret_cast<AddPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVAddThread")));
		m_pSetup = reinterpret_cast<SetupPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVSetup")));
		m_pSetAffinity = reinterpret_cast<SetAffinityPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVSetAffinity")));
		m_pRemove = reinterpret_cast<RemovePtr>(GetProcAddress(m_hThreadManagerDll, _T("SVRemove")));
		m_pClear = reinterpret_cast<ClearPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVClear")));
		m_pGetPipeCount = reinterpret_cast<GetPipeCountPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVGetPipeCount")));
		m_pStartAffinityMgnt = reinterpret_cast<StartAffinityMgntPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVStartAffinityMgnt")));
		m_pStopAffinityMgnt = reinterpret_cast<StopAffinityMgmtPtr>(GetProcAddress(m_hThreadManagerDll, _T("SVStopAffinityMgmt")));

		if( m_pGetThreadInfo != nullptr &&
			m_pGetThreadInfoFilter != nullptr &&
			m_pIsAllowed != nullptr &&
			m_pAdd != nullptr &&
			m_pSetup != nullptr &&
			m_pSetAffinity != nullptr &&
			m_pRemove != nullptr &&
			m_pClear != nullptr &&
			m_pGetPipeCount != nullptr &&
			m_pStartAffinityMgnt != nullptr &&
			m_pStopAffinityMgnt != nullptr)
		{
			hr = S_OK;
		}
	}

	return hr;
}

void SVThreadManager::Destroy()
{
	m_pGetThreadInfo = nullptr;
	m_pGetThreadInfoFilter = nullptr;
	m_pIsAllowed = nullptr;
	m_pAdd = nullptr;
	m_pSetup = nullptr;
	m_pSetAffinity = nullptr;
	m_pRemove = nullptr;
	m_pClear = nullptr;
	m_pGetPipeCount = nullptr;
	// FreeLibrary
	if( m_hThreadManagerDll != nullptr )
	{
		FreeLibrary( m_hThreadManagerDll );
		m_hThreadManagerDll = nullptr;
	}
	::DeleteCriticalSection( &m_CritSec );
}


