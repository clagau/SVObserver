//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManagerImpl
//* .File Name       : $Workfile:   SVThreadManagerImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   01 Dec 2014 11:21:48  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <map>
#include <list>
#pragma endregion

enum SVThreadAttribute
{
	SVNone = 0,
	SVAffinityEditAllowed = 1,
	SVPriorityEditAllowed = 2,
	SVOperatorViewable = 4,
	SVAffinityPPQ = 8, // Not editable but on a seperate pipe. PPQs, Triggers, Acq.
	SVAffinityAcq = 0x10,
	SVAffinityTrigger = 0x20,
	SVAffinityDummy = 0x40,
};

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
#pragma endregion

#pragma region private
private:
	// Set new Affinity to thread handle.
	BOOL SetNewAffinity( HANDLE hThread, DWORD_PTR dwNewAffinity );
	public:
	// Set Affinity based on Thread Name.
	BOOL SetAffinity( LPCTSTR ThreadName, DWORD_PTR dwNewAffinity );

	// Remove will delete the entry from the thread list.
	HRESULT Remove( HANDLE p_hThread );

	HRESULT Create();
	void Destroy();

	static DWORD WINAPI Process(LPVOID lpParam);
	HANDLE m_hShutdown;
	HANDLE m_hThreadComplete;
	HANDLE m_hThread;

	// Current active threads
	ThreadList m_threads;
	long m_lNumPipes;
#pragma endregion
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVThreadManager\SVThreadManagerImpl.h_v  $
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
