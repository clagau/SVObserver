// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConnectionControl
// * .File Name       : $Workfile:   SVConnectionControl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:09:34  $
// ******************************************************************************

#ifndef _INC_SVCONNECTIONCLASS_3879E54B01F4_INCLUDED
#define _INC_SVCONNECTIONCLASS_3879E54B01F4_INCLUDED

#include "SVStatusLibrary/SVRegistry.h"
#include "SVPipes/SVPipeConnection.h"
#include "SVTCPIP/SVTCPIPConnection.h"

#ifdef BUILD_SVCONNECTIONCONTROL_DLL
	#define SV_CONNECTION_EXTERN __declspec (dllexport)
#else
	#define SV_CONNECTION_EXTERN __declspec (dllimport)
#endif
#include "SVConnectionInterface.h"

class SVConnectionControlDlg;

//The SVConnectionControl class manages the loading/unloading 
//of topology specific communications modules and resolves 
//the addresses to member functions within those modules.
class SV_CONNECTION_EXTERN SVConnectionControl : public SVConnectionInterface
{
public:
  enum {NoProtocol = 0x0000, TCPIP = 0x0001, NamedPipes = 0x0002};

	DWORD GetState ();

	void * GetUserData ();

	SV_IOCOMPLETEPROC GetIOCompleteProc ();

	SVConnectionControl ();

  virtual ~SVConnectionControl ();

	//The SetupConnection (szConnection) establishes the 
	//connection specified by szConnection.
	//
	//Control Component Implementation: Determine the desired 
	//underlying protocol, load the boundary component that 
	//implements that protocol, then re-direct the remaining 
	//control component member functions to the coinciding 
	//functions in the boundary component.
	BOOL SetupConnection(CString& szConnection);

	//Initializes the object for data transfer. 
	//
	//In a Control Class implementation, this method looks up 
	//the connection entry in the registry, loads the 
	//protocol specific (boundary) dll for the active 
	//protocol, re-maps the entry point addresses into the 
	//boundary object, then passes the request to the 
	//boundary object's open method.
	//
	//When the Boundary class implementation is loaded, the 
	//address to this method is resolved to the boundary 
	//class Open method.
	 BOOL Open(CString& szConnection, DWORD dwFlags, SV_IOCOMPLETEPROC pfnCompletion, void *pUserData, DWORD dwTimeOut = INFINITE);

	//Closes the specified connection and all conversations 
	//on that connection.
	//
	//In a Control Class implementation, this method calls 
	//the Close method of the boundary object, then completes 
	//any necessary clean up to return the object to a state 
	//where Open() may be called again.
	 void Close( );

	//Starts a read operation on the specified connection.
	//
	//Returns: TRUE if Read was successfully initiated.
	//
	//In a Control Class implementation, this method is 
	//initially dummied out. When the Boundary class 
	//implementation is loaded, the address to this method is 
	//resolved to the boundary class Read method.
   BOOL Read(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE);

	//Sends data to the remote communicating process.
	//
	//In a Control Class implementation, this method is 
	//initially dummied out. When the Boundary class 
	//implementation is loaded, the address to this method is 
	//resolved to the boundary class Write method.
	 BOOL Write(void* pDataBuffer, int cbDataBuffer, DWORD dwTimeOut = INFINITE);
private:
#pragma warning ( push )
#pragma warning ( disable : 4251 )	// needs dll-interface to be used by clients
	SVPipeConnection mPipeConnection;

  CString mszConnectionTypeName;
	void * mpUserData;
	SV_IOCOMPLETEPROC mpfnIOCompleteProc;
	SVTCPIPConnection mTCPIPConnection;
  DWORD mdwConnectionType;

	//Displays a dialog to the user requesting which protocol 
	//is desired for a connection.
	int GetConnectionTypeDlg (SVRegistryClass *pConnectionKey, CString &szConnectionTypeName);
	//Returns a value that specifies the type of connection 
	//(TCPIP, NamedPipes, NoProtocol). If the connection is 
	//not yet defined in the registry, a dialog is presented 
	//to the user where a selection can be made.
	int GetConnectionType (CString &szConnection);
#pragma warning ( pop )
};

#undef SV_CONNECTION_EXTERN
#endif

