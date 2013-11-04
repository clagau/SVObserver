//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCInterfaceClass
//* .File Name       : $Workfile:   SVPLCInterfaceClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:03:46  $
//******************************************************************************

#pragma once
#include <map>
#include "SVCallbackdef.h"
#include "PLCBaseClass.h"
#include "SVPLCSender.h"

DWORD WINAPI SVHeartBeatThreadFunc( LPVOID lpParam );

class SV_PLCAPI_DS SVPLCInterfaceClass
{
public:
	SVPLCInterfaceClass(void);
	virtual ~SVPLCInterfaceClass(void);

	HRESULT Construct( LPCTSTR p_szPLCID, LPTSTR p_szConnection );
	HRESULT SetupConnection( LPTSTR p_szConnection, long p_lSize );

	HRESULT SendBlock ( LPCTSTR p_szPLCID, VARIANT p_pvData );

	HRESULT GetDataType( long* p_plSize, long* p_plType, VARIANT p_vAddress );
	HRESULT GetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, VARIANT p_vAddress );
	HRESULT AddOffsetToAddress( VARIANT* p_pvAddress, long p_lOffset, long p_lBit);

	void Destroy( LPCTSTR p_szPLCID );

	HRESULT WINAPI SetParameterValue( LPCTSTR p_szPLCID, unsigned long p_ulIndex, VARIANT p_varValue );
	HRESULT WINAPI GetParameterValue( LPCTSTR p_szPLCID, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SendHeartBeat( );	// This is called from the heartbeat thread.
	HRESULT TestCommunications(LPCTSTR p_szPLCID);

	long GetMaxDTSize( void );
	HRESULT GetDefaultPLCAddress( VARIANT* p_pvAddress );

	CString m_strHeartBeatAddr;
	long m_lHeartBeatCounter;
	long m_lHeartBeatTime;
	DWORD m_lHeartBeatData;

	HANDLE m_hHeartBeatThread;	// The Heartbeat thread handle.
	HANDLE m_hStopEvent;		// Used to stop the thread.
	HANDLE m_hResetEvent;		// Used to reset the timer everytime outputs are written.
	HRESULT SetHeartBeatCallback( SVGetStateCallback& p_callback );
	BOOL m_bHeartBeatThreadRunning;
private:
	SVGetStateCallback m_pfnGetState;
	void CloseHeartBeatThread();
	HRESULT SetHeartBeatData(SAFEARRAY* p_psa, long p_lIndex, long p_lValue, long p_lType );
	long ParseSetupParameterString (TCHAR* apszConnection);
	static long GetValue (TCHAR*   apczSourceString, 
							TCHAR*   apczKeyString, 
							TCHAR*   apValueString,
							long     alLength);

	SVPLCMapType m_PLCmap;

	// This is where the connection Data will be stored
	SVPLCConnectionParameters m_ConnectionPars;

	HINSTANCE svmhRSLinxLib;
	HINSTANCE svmhApplicomLib;
};

extern SVPLCInterfaceClass ThePLCInterface;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVPLCInterfaceClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:03:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/