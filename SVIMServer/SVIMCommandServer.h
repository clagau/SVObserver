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
	virtual BOOL OnWriteBlockComplete(SvStl::MessageHandler *psvException, char *pBuffer, int cbBuffer);

	//Called when a data block has been read from the client.
	virtual BOOL OnReadBlockComplete(SvStl::MessageHandler *psvException, char *pBuffer, int cbBuffer);

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
	SvStl::MessageHandler* GetLastSVIMError();

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
	virtual BOOL OnSVIMError(SvStl::MessageHandler* pSVException);

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
	SvStl::MessageHandler mSVIMLastException;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIMServer\SVIMCommandServer.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:07:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Oct 2010 09:08:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Dec 2009 11:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 Jul 2001 09:29:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  218
 * SCR Title:  Fix issues involved with SVFocusNT pipe communications
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed SVPipeConnectionClass attribute to a pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Jul 2000 17:06:22   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  25
 * SCR Title:  Download OCR files
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added new SVIM command "PutFile" that writes a single line of data to the specified file.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 May 2000 08:46:44   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added IsStarted() method so applications using this class can determine if the server is actively listening.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 May 2000 17:15:24   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  12
 * SCR Title:  Stage 2: Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Corrected error handling in constructor.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 May 2000 10:02:34   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Eliminated dependency on the SVConnectionControl class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Apr 2000 11:44:24   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Fixed TRACE statements that exceeded the text output limit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Apr 2000 17:05:44   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix linking problem with SVFocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:23:42   mike
 * Initial revision.
*/