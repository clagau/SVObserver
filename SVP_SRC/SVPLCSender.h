//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCSender
//* .File Name       : $Workfile:   SVPLCSender.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:04:20  $
//******************************************************************************

#pragma once
#include <map>
#include <comdef.h>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVSystemLibrary/SVAsyncProcedure.h"

class SVPLCBaseClass;

class SVPLCSender
{
public:
	typedef std::pair<SVPLCSender*, VARIANT > SVPLCSenderData; //VARIANT*
	typedef void (CALLBACK *APCSignalHandler)(DWORD);
	typedef boost::function<void ( bool& )> ThreadSignalHandler;

	SVPLCSender(void);
	SVPLCSender(CString p_strName);
	SVPLCSender( const SVPLCSender& p_rPLCSender );
	virtual ~SVPLCSender(void);
	SVPLCSender& operator=(const SVPLCSender& p_rPLCSender );

	static void CALLBACK OnAPCEvent(DWORD data);
	void OnThreadEvent( bool& p_WaitForEvents );

	// Public Members
	long m_QueueDepth;
	long m_MaxQueueDepth;
	SVPLCBaseClass* m_pPLC;
	CString m_strName;

	// These are the public interface functions...
	HRESULT SendBlock( VARIANT p_vData );
	HRESULT SetPLC( SVPLCBaseClass* p_pPLC );
	HRESULT TestCommunications();
	SVPLCBaseClass* GetPLC();
	HRESULT StartThread( CString p_strPLCID );
	void StopThread();

private:
	HRESULT SendIntBlock ( VARIANT p_pvData );
	HRESULT SendStringBlock ( VARIANT p_pvData );
	// Private Members
	SVAsyncProcedure<APCSignalHandler, ThreadSignalHandler> m_thread;
};

typedef std::map< _bstr_t, SVPLCSender > SVPLCMapType;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVPLCSender.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:04:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/