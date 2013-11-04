// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTCPIPConnection
// * .File Name       : $Workfile:   SVTCPIPConnection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 18:12:08  $
// ******************************************************************************

#ifndef SVTCPIPCONNECTION_H
#define SVTCPIPCONNECTION_H

#ifdef BUILD_SVTCPIPCONNECTION_DLL
	#define SV_CONNECTION_EXTERN __declspec (dllexport)
#else
	#define SV_CONNECTION_EXTERN __declspec (dllimport)
#endif

#include "SVIPC/SVConnectionInterface.h"

//Implements the SVConnectionInterface for TCP/IP 
//communications
#pragma warning ( push )
#pragma warning ( disable : 4275 )	// non dll-interface base class
class SV_CONNECTION_EXTERN SVTCPIPConnection : public CAsyncSocket, public SVConnectionInterface     
{
public:
	DWORD GetState ();
  enum {NoRole = 0x0000, Client = 0x0001, Server = 0x0002};
  enum {None = 0x00000000, Asynchronous = 0x00010000, Synchronous = 0x00020000};
	//Invokes a dialog to establish/modify values for the 
	//server connection.
	BOOL SetupConnection(CString& szConnection);

	//Initializes the object for receipt of data transfer. 
	//Implementation will differ for each type of underlying 
	//communication, but in all cases, successful 
	//initialization indicates that "Read" and "ReadUpdate" 
	//may be performed.
	 BOOL Open(CString& szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion, void *pUserData, DWORD dwTimeOut = INFINITE);

	//Closes the specified connection and all conversations 
	//on that connection.
	 void Close();

	//Starts a read operation on the specified connection.
	//
	//Returns: TRUE if Read was successfully initiated.
	//
	//Note: Reply cannot be used in conjunction with Read. 
	//Use ReadUpdate.
	 BOOL Read(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE);

	//Transfers data to a client over an existing  
	//connection. The client must have previously 
	//requested the server provide unsolicited data.
	//
	//Returns: TRUE if the Write operation successfully 
	//initiated.
	 BOOL Write(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE);

	SVTCPIPConnection();
	virtual ~SVTCPIPConnection();

private:
#pragma warning ( disable : 4251 )	// needs dll-interface to be used by clients
	CString szConnectionKey;
	void OnReceive (int nErrorCode);
  void OnSend (int nErrorCode);
  void OnAccept (int nErrorCode);
  void OnConnect (int nErrorCode);
  void OnClose (int nErrorCode);
	DWORD mPrimaryRC;
	CString GetRegistryKey (CString &szConnection);
	BOOL GetHost (CString &szConnection, CString &szHost);
	int GetPort (CString &szConnection);
};

#undef SV_CONNECTION_EXTERN
#pragma warning ( pop )
#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTCPIP\SVTCPIPConnection.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:12:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Oct 2010 09:02:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 10:58:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:25:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:25:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Apr 2000 17:13:54   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updates to fix SVFocus linking problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 23 2000 16:41:00   mike
 * Removed "SV_SHUTDOWN" callback message
 * 
 *    Rev 1.0   Mar 22 2000 17:33:12   mike
 * Initial revision.
*/