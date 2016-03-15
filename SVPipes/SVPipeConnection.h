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

