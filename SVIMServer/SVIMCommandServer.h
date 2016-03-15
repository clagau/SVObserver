// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMCommandServer
// * .File Name       : $Workfile:   SVIMCommandServer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:07:24  $
// ******************************************************************************

#ifndef INC_SVIMCOMMANDSERVER_38B56AEB0243_INCLUDED
#define INC_SVIMCOMMANDSERVER_38B56AEB0243_INCLUDED

#include "SVPipes/SVPipeConnection.h"
#include "SVIMCommand/SVIMCommand.h"

//The SVIMCommandServer processes commands from a remote 
//SVIMCommandClient.
class SVIMCommandServer 
{
protected:
	SVPipeConnection *mpConnection;
	CString mszConnection;
	//Signaled when a foreground SVIM command completes.
	CEvent mCommandCompleteEvent;
	//Buffer to receive commands from an SVIM client.
	char mWriteDataBuffer[SVIOBUFFSIZE];
	char mReadDataBuffer[SVIOBUFFSIZE];


public:
	virtual BOOL OnPutFile(char* szFileName, char * pData  );

	virtual BOOL IsStarted();

	//Called whenever a write to the client completes.
	virtual BOOL OnWriteBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer);

	//Called when a data block has been read from the client.
	virtual BOOL OnReadBlockComplete(SvStl::MessageContainer *psvException, char *pBuffer, int cbBuffer);

	virtual BOOL OnSVIMData(char * pBuffer, UINT NumBytes);

	BOOL GetConnectionName (CString &szConnection);
	//Returns the address of the object's read data buffer.
	char* GetReadDataBuffer();

	//Returns the address of the object's write data buffer.
	char * GetWriteDataBuffer();

	//Called when a receive data thread completes.  The 
	//application should call GetLastSVIMError () to 
	//determine the status.
	virtual void OnSVIMReceiveComplete(CString & szReceiveFileName);

	//Called when a send thread completes. The application 
	//should call GetLastSVIMError () to determine the status.
	virtual void OnSVIMSendComplete(CString & szSendFileName);

	//Returns a pointer to an SVException object that 
	//describes the last error that occured.
	SvStl::MessageContainer* GetLastSVIMError();

	//Called when a communication error occurs.
	//
	//The SVException* paramenter contains information about 
	//the error.  Overridden implementations of this function 
	//should call the base class implementation so that the 
	//SVException object can be deleted.
	//
	//Return TRUE from this function to attempt 
	//re-connection, FALSE to terminate the connection.  If 
	//FALSE is returned, the application must call Open() to 
	//re-establish communications.
	virtual BOOL OnSVIMError(SvStl::MessageContainer* pSVException);

	//Transmit the current operating state to the requesting 
	//application.
	//
	//Returns TRUE if the status was successfully transmitted 
	//to the requesting application.
	virtual BOOL OnGetSVIMState(DWORD *pdwSVIMState);
	//Sets the operating state to ONLINE, then transmits the 
	//current operating state to the requesting application.
	//
	//Returns TRUE if the status was successfully transmitted 
	//to the requesting application.
	virtual BOOL OnSVIMGoOnline();
	//Sets the operating state to OFFLINE, then transmits the 
	//current operating state to the requesting application.
	//
	//Returns TRUE if the status was successfully transmitted 
	//to the requesting application.
	virtual BOOL OnSVIMGoOffline();
	//Sets the operating state to OFFLINE,  then transmits 
	//the currently loaded configuration to the requesting 
	//application.
	//
	//Returns TRUE if the configuration was successfully 
	//transmitted to the requesting application.
	virtual BOOL OnGetCurrentSVIMConfig(CString &szIMCSVIMConfigName);
	//Sets the operating state to OFFLINE, then transmits the 
	//specified configuration to the requesting application.
	//
	//Returns TRUE if the configuration was successfully 
	//transmitted to the requesting application.
	virtual BOOL OnGetSVIMConfig(CString &szIMCSVIMConfigName);
	//Sets the operating state to OFFLINE, then reads the 
	//specified configuration from the requesting application.
	//
	//Returns TRUE if the configuration was successfully read 
	//from the requesting application.
	virtual BOOL OnPutSVIMConfig(CString &szIMCSVIMConfigName);

	//Returns the CEvent object used for synchronization 
	//between the calling application and communications 
	//sub-system. Tampering with this object can produce 
	//unexpected results!
	CEvent * GetCommandCompleteEvent();

	//Establishes a command server object for an SVIM.
	SVIMCommandServer(CString & szConnection);

	~SVIMCommandServer();

private:
	BOOL mbStarted;

	//Contains the last error information that occured.
	SvStl::MessageContainer mSVIMLastException;

};

#endif

