// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMServer
// * .File Name       : $Workfile:   SVIMServer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:07:32  $
// ******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVIMServer_38BD7095002E_INCLUDED
#define _INC_SVIMServer_38BD7095002E_INCLUDED

#ifdef SV_BUILD_SVIMSERVER
#define SV_EXPORTDLL __declspec (dllexport)
#else
#define SV_EXPORTDLL __declspec (dllimport)
#endif

#include "SVIMCommandServer.h"

typedef BOOL (* PFN_BOOLVOID)();
typedef BOOL (* PFN_BOOLCHAR)(char *);
typedef BOOL (* PFN_BOOLPDWORD)(DWORD *);

//##ModelId=38E8FF8A0251
class SVIMServer : public SVIMCommandServer
{
public:
	// IsStarted() returns true if the server connection successfully
	// opened and the server is listening.
	//##ModelId=392BCC1802DE
	BOOL SV_EXPORTDLL IsStarted ();
	//##ModelId=38E8FF8B00EB
	void SV_EXPORTDLL SetSVIMSendConfigInit (PFN_BOOLCHAR pfnSVIMSendConfigInit);
	//##ModelId=38E8FF8B00CC
	void SV_EXPORTDLL SetGoOffline (PFN_BOOLVOID pfnGoOffline);
	//##ModelId=38E8FF8B00BD
	void SV_EXPORTDLL SetGoOnline (PFN_BOOLVOID pfnGoOnline);
	//##ModelId=38E8FF8B00BB
	void SV_EXPORTDLL SetGetConfigurationName (PFN_BOOLCHAR pfnGetCurrentConfigPath);
	//##ModelId=38E8FF8B008C
	void SV_EXPORTDLL SetGetState (PFN_BOOLPDWORD pfnGetSVIMState);
	//##ModelId=38E8FF8B006F
	void SV_EXPORTDLL SetOpenConfiguration (PFN_BOOLCHAR pfnOpenSECFile);
	void SV_EXPORTDLL SetSaveConfiguration (PFN_BOOLVOID pfnSaveConfiguration);

	void SV_EXPORTDLL SetCloseConfiguration (PFN_BOOLVOID pfnCloseConfiguration);

	//##ModelId=38E8FF8B006D
	void SV_EXPORTDLL SetSVObserverWnd(HWND hWnd);
	//##ModelId=38E8FF8B0063
	void SV_EXPORTDLL SetupConnection();
	//##ModelId=38E8FF8B005F
	virtual BOOL OnWriteBlockComplete(SVException *psvException, char *pBuffer, int cbBuffer);

	//##ModelId=38E8FF8B0050
	virtual BOOL OnReadBlockComplete(SVException *psvException, char *pBuffer, int cbBuffer);

	//Called when a receive data thread completes.  The 
	//application should call GetLastSVIMError () to 
	//determine the status.
	//##ModelId=38E8FF8B004E
	virtual void OnSVIMReceiveComplete(CString& szReceiveFileName);

	//Called when a send thread completes. The application 
	//should call GetLastSVIMError () to determine the status.
	//##ModelId=38E8FF8B0044
	virtual void OnSVIMSendComplete(CString& szSendFileName);

	//##ModelId=38E8FF8B0043
	SV_EXPORTDLL SVIMServer();

	virtual SV_EXPORTDLL ~SVIMServer();

protected:
	//Sets the operating state to OFFLINE, then reads the 
	//specified configuration from the requesting application.
	//
	//Returns TRUE if the configuration was successfully read 
	//from the requesting application.
	//##ModelId=38E8FF8B0041
	virtual BOOL OnPutSVIMConfig(CString& szIMCSVIMConfigName);

	//Sets the operating state to OFFLINE, then transmits the 
	//specified configuration to the requesting application.
	//
	//Returns TRUE if the configuration was successfully 
	//transmitted to the requesting application.
	//##ModelId=38E8FF8B003F
	virtual BOOL OnGetSVIMConfig(CString& szIMCSVIMConfigName);

	//Sets the operating state to OFFLINE,  then transmits 
	//the currently loaded configuration to the requesting 
	//application.
	//
	//Returns TRUE if the configuration was successfully 
	//transmitted to the requesting application.
	//##ModelId=38E8FF8B003E
	virtual BOOL OnGetCurrentSVIMConfig(CString &szIMCSVIMConfigName);

	//Sets the operating state to OFFLINE, then transmits the 
	//current operating state to the requesting application.
	//
	//Returns TRUE if the status was successfully transmitted 
	//to the requesting application.
	//##ModelId=38E8FF8B0032
	virtual BOOL OnSVIMGoOffline();

	//Sets the operating state to ONLINE, then transmits the 
	//current operating state to the requesting application.
	//
	//Returns TRUE if the status was successfully transmitted 
	//to the requesting application.
	//##ModelId=38E8FF8B0031
	virtual BOOL OnSVIMGoOnline();

	//Transmit the current operating state to the requesting 
	//application.
	//
	//Returns TRUE if the status was successfully transmitted 
	//to the requesting application.
	//##ModelId=38E8FF8B002F
	virtual BOOL OnGetSVIMState(DWORD * pdwSVIMState);

	//##ModelId=38E8FF8B0003
	virtual BOOL OnSVIMData(char * pBuffer, UINT NumBytes);

private:
	//##ModelId=38E8FF8B0000
	PFN_BOOLVOID mpfnGoOffline;
	//##ModelId=38E8FF8A03CA
	PFN_BOOLVOID mpfnGoOnline;
	//##ModelId=38E8FF8A03AB
	PFN_BOOLCHAR mpfnGetCurrentConfigPath;
	//##ModelId=38E8FF8A038B
	PFN_BOOLPDWORD mpfnGetSVIMState;
	//##ModelId=38E8FF8A033C
	PFN_BOOLCHAR mpfnOpenConfiguration;
	//##ModelId=38E8FF8A030F
	PFN_BOOLVOID mpfnSaveConfiguration;
	PFN_BOOLVOID mpfnCloseConfiguration;
	//##ModelId=38E8FF8A02EF
	HWND mhWndSVObserver;
	//##ModelId=38E8FF8A02EE
	CString mszIMCSVIMConfigName;
	//##ModelId=38E8FF8A0272
	CFile * mpFile;
};

#endif /* _INC_SVIMServer_38BD7095002E_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIMServer\SVIMServer.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:07:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2001 09:28:20   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised functionality of this pipes server to be consistent with the equivalent COM server.  
 * Modified the following functions:  OnPutSVIMConfig, OnSVIMData, OnGetSVIMConfig.
 * Revised the follwoing function names: from SetGetCurrentConfigPath to SetGetConfigurationName, from SetGetSVIMState to SetGetState, from SetOpenSECFile to SetOpenConfiguration, from SetSVIMSendConfigInit to SetSaveConfiguration.
 * Added the function SetCloseConfiguration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 May 2000 08:47:44   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added IsStarted() method so applications using this class can determine if the server is actively listening.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Apr 2000 11:45:50   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Completed the PutConfig processing for SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Apr 2000 16:55:22   mike
 * Initial revision.
*/