//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManagerImpl
//* .File Name       : $Workfile:   SVThreadManagerImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Dec 2014 07:31:16  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <map>
#include <list>
#include "SVThreadAttribute.h"
#pragma endregion


struct SVThreadSetup
{	SVThreadSetup():m_lAffinity(0),m_lPriority(0),m_hThread(nullptr){};
	bool operator==( const SVThreadSetup& rhs){ return rhs.m_hThread == m_hThread; };
	SVThreadAttribute m_eAttribute;
	long m_lPriority;
	long m_lAffinity;
	// Threads will be setup based on thread name or partial thread name.
	std::string m_strName; // Thread name
	HANDLE m_hThread;
};

class SVThreadManagerImpl
{
#pragma region Public
public:
	typedef std::list<SVThreadSetup> ThreadList;
	SVThreadManagerImpl();
	~SVThreadManagerImpl();
	static SVThreadManagerImpl& Instance();

	// GetThreadInfo will create a list of information about each thread.
	// the list will be a comma seperated list of number, name, priority, Affinity
	HRESULT GetThreadInfo( std::list<std::string>& p_rStrList);

	// Gets a list of SVThreadSetup filtered by Attribute.
	HRESULT GetThreadInfo( ThreadList& rSetupList, SVThreadAttribute eFilter );

	HRESULT IsAllowed( LPCTSTR Name, SVThreadAttribute eAttrib );

	// Add(... will register a name with a Thread ID
	// GetThreadList will get a list of thread names and Thread Info.
	// The info will be part of the string describing the thread.
	HRESULT Add( HANDLE hThread, LPCTSTR strName, SVThreadAttribute eAttrib=SVNone );

	// Clear removes all items from the list that have null handles.
	HRESULT Clear( );

	// Setup is used to restore thread data
	HRESULT Setup( LPCTSTR strName, long Affinity );

	long GetPipeCount( );

	HRESULT StopAffinityMgmt();
	HRESULT StartAffinityMgnt();

#pragma endregion

#pragma region private
private:
	// Set new Affinity to thread handle.
	BOOL SetThreadHandleAffinity( HANDLE hThread, DWORD_PTR dwNewAffinity );
	public:
	// Set Affinity based on Thread Name.
	BOOL SetAffinity( LPCTSTR ThreadName, DWORD_PTR dwNewAffinity );

	// Remove will delete the entry from the thread list.
	HRESULT Remove( HANDLE p_hThread );


	static DWORD WINAPI Process(LPVOID lpParam);
	HANDLE m_hShutdown;
	HANDLE m_hThreadComplete;
	HANDLE m_hThread;

	// Current active threads
	ThreadList m_threads;
	long m_lNumPipes;
	bool m_bThreadManagementActive;
#pragma endregion
};

