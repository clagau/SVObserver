// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConnectionInterface
// * .File Name       : $Workfile:   SVConnectionInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:09:46  $
// ******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SV_CONNECTION_EXTERN
#define SV_CONNECTION_EXTERN __declspec (dllimport)
#endif

typedef BOOL (CALLBACK * SV_IOCOMPLETEPROC)(void *pConnection, int nErrorCode, void *pBuffer, int cbBuffer, int Operation, void *pUserData);

// Operation codes used for I/O completion
#define SV_READ 0x0000
#define SV_WRITE 0x0001
#define SV_OPEN 0x0002
#define SV_CLOSE 0x0003

// Class States
#define UNINITIALIZED 0x0000
#define CONNECTWAIT 0x0001
#define CONNECTED   0x0002
#define READING     0x0004
#define WRITING     0x0008
#define CLOSING     0x0010
#define CLOSED      0x0000

//SVResearch Interprocess Communication (IPC) interface.
//
//This interface provides a single method of exchanging data 
//between applications without concern about the underlying 
//communications system (DDE, TCP/IP, etc).
//
//A class wishing to implement the SVConnectionInterface must 
//be derived from this class. There are 2 types of classes 
//that can be derived from this interface: Control and 
//Boundary.
//
//A control class implements the SetupConnection method 
//(function) only, with all other methods "dummied" out. The 
//SetupConnection method determines (either from a user or 
//program) which communications protocol is desired for the 
//underlying connection.
//
//A boundary class implements all methods of the interface, 
//including the SetupConnection. When it is loaded by the 
//control component, the dummied out methods of the control 
//component are re-directed to the implementation of those 
//methods in the boundary class. Additionally, the 
//SetupConnection method of the boundary class can be called 
//from the SetupConnection of the control class so that 
//protocol-specific parameters can be set.
//
//It follows, then, that at least 2 components (Dll's) must 
//be developed in order to implement this interface. First, a 
//control component (Dll) that can determine which one of 
//possibly several different boundary components (Dlls) is to 
//be loaded, and second, at least 1 boundary component (Dll) 
//must be implemented that will be loaded by the managing 
//control component.
//##ModelId=386A347B001F
class SVConnectionInterface  
{
public:
  enum {NoProtocol, TCPIP, NamedPipes};
  enum {NoRole = 0x0000, Client = 0x0001, Server = 0x0002};
	//Closes the specified connection and all conversations 
	//on that connection.
	//
	//Control Component Implementation:
	//##ModelId=386A347B0083
	virtual void Close() = 0;

	//Starts a read operation on the specified connection.
	//
	//Returns: TRUE if Read was successfully initiated.
	//
	//Note: Reply cannot be used in conjunction with Read. 
	//Use ReadUpdate.
	//##ModelId=386A347B0080
	virtual BOOL Read(void * pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE) = 0;

	//Initializes the object for receipt of data transfer. 
	//Implementation will differ for each type of underlying 
	//communication, but in all cases, successful 
	//initialization indicates that "Read" and "ReadUpdate" 
	//may be performed.
	//##ModelId=386A347B007B
	virtual BOOL Open(CString & szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion , void *pUserData, DWORD dwTimeOut = INFINITE) = 0;

	//Transfers data to a client over an existing 
	//co connection. The client must have previously 
	//requested the server provide unsolicited data.
	//
	//Returns: TRUE if the Write operation successfully 
	//initiated.
	//##ModelId=386A347B0078
	virtual BOOL Write(void * pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE) = 0;

	//The SetupConnection (szConnection) establishes the 
	//connection specified by szConnection.
	//
	//Control Component Implementation: Determine the desired 
	//underlying protocol, load the boundary component that 
	//implements that protocol, then re-direct the remaining 
	//control component member functions to the coinciding 
	//functions in the boundary component.
	//
	//Boundary Component Implementation: Obtains the protocol 
	//specific values necessary to establish a connection.
	//
	//If the connection specified by szConnection does not 
	//exist, SetupConnection (szConnection) calls 
	//SetupConnection().
	//##ModelId=386A347B005D
  virtual BOOL SetupConnection (CString & szConnection) = 0;

	//Returns a doubleword describing the current state of 
	//the connection object. Values may be one of the 
	//following: 
	//
	//UNINITIALIZED (Same as CLOSED)
	//CONNECTWAIT
	//CONNECTED
	//READING
	//WRITING
	//CLOSING
	//CLOSED (Same as UNINITIALIZED)
	//##ModelId=38873430001F
  virtual DWORD GetState () = 0;
};

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIPC\SVConnectionInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:09:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 May 2000 10:00:32   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  19
 * SCR Title:  Module dependency analysis
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Eliminated #defines for non-standard error codes.  Classes that implement this interface must be dependent on SVMessage.h
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Apr 2000 17:07:06   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updates to fix SVFocus linking problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Mar 23 2000 16:34:14   mike
 * Fixed memory leak
 * 
 *    Rev 1.0   Mar 22 2000 17:25:24   mike
 * Initial revision.
*/