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
#include "SVStatusLibrary/GlobalPath.h"

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
	m_bThreadManagerInstalled = GetPrivateProfileInt( _T( "Settings" ), _T( "ThreadManagerActive" ), 1, SvStl::GlobalPath::Inst().GetSVIMIniPath());
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
	if( nullptr != m_pGetThreadInfo )
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
	if( nullptr != m_pGetThreadInfoFilter )
	{
		BSTR List = nullptr;
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
		while( nullptr != token1 )
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
			if( nullptr != token1 ) // Name
			{
				tsu.m_strName = token1;
			}
			token1 = strtok_s(nullptr, ",", &next_token);
			if( nullptr != token1 ) // Attribute
			{
				tsu.m_eAttribute = static_cast<SVThreadAttribute>(atol(token1));
			}
			token1 = strtok_s(nullptr, ",", &next_token);
			if( nullptr != token1 ) // Affinity
			{
				tsu.m_lAffinity = atol(token1);
			}
			token1 = strtok_s(nullptr, ",", &next_token);
			if( nullptr != token1 ) // Handle
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
	if( nullptr != m_pIsAllowed )
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
	if( nullptr != m_pAdd )
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
		if( nullptr != m_pSetup )
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
		if( nullptr != m_pStartAffinityMgnt )
		{
			hr = m_pStartAffinityMgnt();
		}
	}
	if( !bEnable && m_bThreadAffinityEnabled )
	{
		if( nullptr != m_pStopAffinityMgnt )
		{
			hr = m_pStopAffinityMgnt();
		}
	}
	if( S_OK == hr )
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
	if( nullptr != m_pGetPipeCount )
	{
		long value = 0;
		hr = m_pGetPipeCount( &value );
		if( S_OK == hr )
		{
			lrPipeCount = value;
		}
	}
	return hr;
}

BOOL SVThreadManager::SetAffinity( LPCTSTR ThreadName, DWORD_PTR dwAffinityBitNum )
{
	BOOL bRet = FALSE;
	if( nullptr != m_pSetAffinity )
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
	if( nullptr != m_pRemove )
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
	if( nullptr != m_pClear )
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
	if( nullptr != m_hThreadManagerDll )
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

		if( nullptr != m_pGetThreadInfo &&
			nullptr != m_pGetThreadInfoFilter &&
			nullptr != m_pIsAllowed &&
			nullptr != m_pAdd &&
			nullptr != m_pSetup &&
			nullptr != m_pSetAffinity &&
			nullptr != m_pRemove &&
			nullptr != m_pClear &&
			nullptr != m_pGetPipeCount &&
			nullptr != m_pStartAffinityMgnt &&
			nullptr != m_pStopAffinityMgnt )
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
	if( nullptr != m_hThreadManagerDll )
	{
		FreeLibrary( m_hThreadManagerDll );
		m_hThreadManagerDll = nullptr;
	}
	::DeleteCriticalSection( &m_CritSec );
}


