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

#pragma once

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

