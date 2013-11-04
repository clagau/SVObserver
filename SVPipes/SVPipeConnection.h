// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPipeConnection
// * .File Name       : $Workfile:   SVPipeConnection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 14:01:28  $
// ******************************************************************************

#ifndef SVPIPECONNECTION_H
#define SVPIPECONNECTION_H

#ifdef BUILD_SVPIPESCONNECTION_DLL
	#define SV_CONNECTION_EXTERN __declspec (dllexport)
#else
	#define SV_CONNECTION_EXTERN __declspec (dllimport)
#endif
#include "SVIPC/SVConnectionInterface.h"

class SVConnectionInterface;
class SVPipesConnectionDlg;

class CEvent;

//The SVPipeConnectionClass provides interprocess 
//communications via named pipes.
class SV_CONNECTION_EXTERN SVPipeConnection : public SVConnectionInterface
{
public:
	DWORD GetFlags ();

	CEvent * GetThreadWaitEvent();

	void WaitThreadStart();

  enum {NoRole = 0x0000, Client = 0x0001, Server = 0x0002};

	int GetDataBufferSize ();
	void * GetDataBuffer ();
	void SetThread(HANDLE hThread);
	DWORD SetState (DWORD dwNewState);
	DWORD GetState ();
	void * GetUserData ();
	SV_IOCOMPLETEPROC GetCompletionProc ();
	HANDLE GetPipeHandle ();
	//Sets the registry values specific to named pipe 
	//communications.
	BOOL SetupConnection (CString &szConnection);
	//Sends data accross a communications link.
	BOOL Write(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE);
	//Reads data from the communication link.
	BOOL Read(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE);
	//Terminates a communication link.
	void Close ();
	//Opens the named pipe communication link and prepares 
	//for data transfer. The connection may be opened for 
	//client or server operation.
	BOOL Open(CString& szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion, void *pUserData, DWORD dwTimeOut = INFINITE);
	//Creates a named pipe communication link.
	SVPipeConnection();
	//Terminates a named pipe communication link.
	virtual ~SVPipeConnection();

private:
#pragma warning ( push )
#pragma warning ( disable : 4251 )	// needs dll-interface to be used by clients
	void Close (BOOL bDestroy);
	CEvent mThreadSyncEvent;

	int mcbDataBuffer;
	void * mpDataBuffer;
	DWORD mdwState;
	void * mpUserData;
	HANDLE mhThread;
	DWORD mdwFlags;
	//Handle to the named pipe.
	HANDLE mhPipe;
	//Retrieves the name of the pipe for the specified 
	//connection name.
	BOOL GetPipeName (CString &szConnection, CString &szPipe);
	//Retrieves the name of the server for the specified 
	//connection name.
  BOOL GetServerName (CString &szConnection, CString &szServer);
	//Constructs mszConnectionKey based upon a connection 
	//name.
	CString GetRegistryKey (CString &szConnection);
	//Contains the registry key where parameters specific to 
	//the desired connection are stored.
  CString mszConnectionKey;
	//Pointer to a function that is called whenever an I/O 
	//operation completes.
   SV_IOCOMPLETEPROC mpfnCompletion;
#pragma warning ( pop )
};

#undef SV_CONNECTION_EXTERN
#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVPipes\SVPipeConnection.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 14:01:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 10:51:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:26:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Apr 2000 17:12:16   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updates to fix SVFocus linking problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 23 2000 16:38:26   mike
 * Removed "SV_SHUTDOWN" callback message
 * 
 *    Rev 1.0   Mar 22 2000 17:29:28   mike
 * Initial revision.
*/