//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManagerExports
//* .File Name       : $Workfile:   SVThreadManagerExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   01 Dec 2014 11:21:48  $
//******************************************************************************
#ifndef _THREADMANAGEREXPORTS_
#define _THREADMANAGEREXPORTS_

#include <comdef.h>
#include "SVThreadManagerImpl.h"
#include <list>
#include <string>

// GetThreadInfo will create a list of information about each thread.
// the list will be a comma seperated list of number, name, priority, Affinity
HRESULT WINAPI SVGetThreadInfo( BSTR* bstrInfo );

// Gets a comma seperated list information filtered by Attribute.
HRESULT WINAPI SVGetThreadInfoFilter( BSTR* bstInfo, SVThreadAttribute eFilter );

HRESULT WINAPI SVIsAllowed( BSTR Name, SVThreadAttribute eAttrib );

// Add(... will register a name with a Thread ID
// GetThreadList will get a list of thread names and Thread Info.
// The info will be part of the string describing the thread.
HRESULT WINAPI SVAddThread( HANDLE hThread, BSTR strName, SVThreadAttribute eAttrib );

// Setup is used to restore thread data
HRESULT WINAPI SVSetup( BSTR strName, long Affinity );

HRESULT WINAPI SVGetPipeCount( long* lPipeCount );

// Set new Affinity to thread handle.
//BOOL WINAPI SVSetNewAffinity( HANDLE hThread, DWORD_PTR dwNewAffinity );
// Set Affinity based on Thread Name.
BOOL WINAPI SVSetAffinity( BSTR ThreadName, DWORD_PTR dwNewAffinity );

// Remove will delete the entry from the thread list.
HRESULT WINAPI SVRemove( HANDLE p_hThread );

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVThreadManager\SVThreadManagerExports.h_v  $
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
