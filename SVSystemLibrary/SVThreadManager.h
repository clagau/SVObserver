//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManager
//* .File Name       : $Workfile:   SVThreadManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Dec 2014 08:18:16  $
//******************************************************************************


#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <list>
//Moved to precompiled header: #include <comdef.h>
#include "SVStatusLibrary/SourceFileParams.h"
#include "SVThreadManager/SVThreadAttribute.h"
#pragma endregion Includes


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

typedef HRESULT (WINAPI *GetThreadInfoPtr)( BSTR* bstList);
typedef HRESULT (WINAPI *GetThreadInfoFilterPtr)( BSTR* SetupList, SVThreadAttribute eFilter );
typedef HRESULT (WINAPI *IsAllowedPtr)( BSTR Name, SVThreadAttribute eAttrib );
typedef HRESULT (WINAPI *AddPtr)( HANDLE hThread, BSTR strName, SVThreadAttribute eAttrib );
typedef HRESULT (WINAPI *SetupPtr)( BSTR strName, long Affinity );
typedef HRESULT (WINAPI *GetPipeCountPtr)(long* pLPipeCount);
typedef BOOL (WINAPI *SetAffinityPtr)( BSTR ThreadName, DWORD_PTR dwNewAffinity );
typedef HRESULT (WINAPI *RemovePtr)( HANDLE p_hThread );
typedef HRESULT (WINAPI *ClearPtr)( );
typedef HRESULT (WINAPI *StartAffinityMgntPtr)();
typedef HRESULT (WINAPI *StopAffinityMgmtPtr)();

class SVThreadManager
{
#pragma region Public
public:
	typedef std::list<SVThreadSetup> ThreadList;
	SVThreadManager();
	~SVThreadManager();
	static SVThreadManager& Instance();

	// GetThreadInfo will create a list of information about each thread.
	// the list will be a comma seperated list of number, name, priority, Affinity
	HRESULT GetThreadInfo( std::list<std::string>& p_rStrList) const;

	// Gets a list of SVThreadSetup filtered by Attribute.
	HRESULT GetThreadInfo( std::list<SVThreadSetup>& rSetupList, SVThreadAttribute eFilter );

	HRESULT IsAllowed( LPCTSTR Name, SVThreadAttribute eAttrib );

	// Add(... will register a name with a Thread ID
	// GetThreadList will get a list of thread names and Thread Info.
	// The info will be part of the string describing the thread.
	HRESULT Add( HANDLE hThread, LPCTSTR strName, SVThreadAttribute eAttrib=SVNone );

	// Setup is used to restore thread data
	HRESULT Setup( LPCTSTR strName, long Affinity );

	HRESULT GetPipeCount( long& lPipeCount );

	//// Set new Affinity to thread handle.
	//BOOL SetNewAffinity( HANDLE hThread, DWORD_PTR dwNewAffinity );
	// Set Affinity based on Thread Name.
	BOOL SetAffinity( LPCTSTR ThreadName, DWORD_PTR dwNewAffinity );

	// Remove will delete the entry from the thread list.
	HRESULT Remove( HANDLE p_hThread );

	HRESULT Clear( );

	HRESULT Create();
	void Destroy();
	BOOL GetThreadAffinityEnabled() const;
	void SetThreadAffinityEnabled( BOOL bEnable );
	BOOL IsThreadManagerInstalled() const;

	static void setThreadError( DWORD MessageCode, LPCTSTR Message, SvStl::SourceFileParams SourceFile );

#pragma endregion

#pragma region Private
private:
	HMODULE m_hThreadManagerDll;

	GetThreadInfoPtr m_pGetThreadInfo;
	GetThreadInfoFilterPtr m_pGetThreadInfoFilter;
	IsAllowedPtr m_pIsAllowed;
	AddPtr m_pAdd;
	SetupPtr m_pSetup;
	GetPipeCountPtr m_pGetPipeCount;
	SetAffinityPtr m_pSetAffinity;
	RemovePtr m_pRemove;
	ClearPtr m_pClear;
	StartAffinityMgntPtr m_pStartAffinityMgnt;
	StopAffinityMgmtPtr m_pStopAffinityMgnt;

	BOOL m_bThreadAffinityEnabled;
	BOOL m_bThreadManagerInstalled;
	CRITICAL_SECTION m_CritSec;
#pragma endregion
};

