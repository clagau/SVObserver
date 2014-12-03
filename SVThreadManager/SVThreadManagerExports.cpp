//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManagerExports
//* .File Name       : $Workfile:   SVThreadManagerExports.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   01 Dec 2014 11:21:46  $
//******************************************************************************
#include "stdafx.h"
#include "SVThreadManagerExports.h"
#include "SVThreadManagerImpl.h"
#include <sstream>


SVThreadManagerImpl gThreadManager;


// GetThreadInfo will create a list of information about each thread.
// the list will be a comma seperated list of number, name, priority, Affinity
HRESULT WINAPI SVGetThreadInfo( BSTR* bstrInfo)
{
	HRESULT hr = S_OK;
	std::list<std::string> threadList;
	hr = gThreadManager.GetThreadInfo(threadList);

	std::string str;
	for( std::list<std::string>::const_iterator it = threadList.begin() ; it != threadList.end() ; ++it)
	{
		str.append(*it);
		str.append( "\n");
	}
	_bstr_t bstVal(str.c_str());
	*bstrInfo = bstVal.Detach();

	return hr;
}

// Gets a list of SVThreadSetup filtered by Attribute.
HRESULT WINAPI SVGetThreadInfoFilter( BSTR* bstrInfo, SVThreadAttribute eFilter )
{
	SVThreadManagerImpl::ThreadList InfoList;
	HRESULT hr = gThreadManager.GetThreadInfo(InfoList, eFilter);
	if( hr == S_OK )
	{
		std::string strTmp;
		for( SVThreadManagerImpl::ThreadList::const_iterator it = InfoList.begin() ; it != InfoList.end() ; ++it)
		{
			char line[256];
			sprintf_s(line, 256, "%s,%d,%d,%ld\n", it->m_strName.c_str(), it->m_eAttribute, it->m_lAffinity, it->m_hThread);

			strTmp.append( line );
		}
		_bstr_t bstVal( strTmp.c_str());
		*bstrInfo = bstVal.Detach();
	}
	return hr;
}

HRESULT WINAPI SVIsAllowed( BSTR Name, SVThreadAttribute eAttrib )
{
	_bstr_t bstName;
	bstName.Attach( Name );

	HRESULT hr = gThreadManager.IsAllowed( bstName, eAttrib);
	return hr;
}

// Add(... will register a name with a Thread ID
// GetThreadList will get a list of thread names and Thread Info.
// The info will be part of the string describing the thread.
HRESULT WINAPI SVAddThread( HANDLE hThread, BSTR Name, SVThreadAttribute eAttrib )
{
	_bstr_t bstName;
	bstName.Attach( Name );
	HRESULT hr = gThreadManager.Add(hThread, bstName, eAttrib);
	return hr;
}

// Setup is used to restore thread data
HRESULT WINAPI SVSetup( BSTR Name, long Affinity )
{
	_bstr_t bstName;
	bstName.Attach(Name);
	HRESULT hr = gThreadManager.Setup( bstName, Affinity);
	return hr;
}

HRESULT WINAPI SVGetPipeCount( long* lPipeCount )
{
	*lPipeCount =  gThreadManager.GetPipeCount();
	return S_OK;
}

// Set Affinity based on Thread Name.
BOOL WINAPI SVSetAffinity( BSTR ThreadName, DWORD_PTR dwNewAffinity )
{
	_bstr_t bstName;
	bstName.Attach( ThreadName );
	BOOL bOk = gThreadManager.SetAffinity( bstName, dwNewAffinity);
	return bOk;
}

// Remove will delete the entry from the thread list.
HRESULT WINAPI SVRemove( HANDLE p_hThread )
{
	HRESULT hr = gThreadManager.Remove(p_hThread);
	return hr;
}

// Clear will delete entries that have a null thread handle.
HRESULT WINAPI SVClear( )
{
	HRESULT hr = gThreadManager.Clear( );
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVThreadManager\SVThreadManagerExports.cpp_v  $
 * 
 *    Rev 1.0   01 Dec 2014 11:21:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
