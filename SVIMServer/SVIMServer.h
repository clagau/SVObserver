// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMServer
// * .File Name       : $Workfile:   SVIMServer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Feb 2014 16:31:02  $
// ******************************************************************************

#pragma once

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
	void SV_EXPORTDLL SetOpenConfiguration (PFN_BOOLCHAR pfnOpenConfiguration);
	void SV_EXPORTDLL SetSaveConfiguration (PFN_BOOLVOID pfnSaveConfiguration);

	void SV_EXPORTDLL SetCloseConfiguration (PFN_BOOLVOID pfnCloseConfiguration);

	//##ModelId=38E8FF8B006D
	void SV_EXPORTDLL SetSVObserverWnd(HWND hWnd);
	//##ModelId=38E8FF8B0063
	void SV_EXPORTDLL SetupConnection();
	//##ModelId=38E8FF8B005F
	virtual BOOL OnWriteBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer);

	//##ModelId=38E8FF8B0050
	virtual BOOL OnReadBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer);

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

