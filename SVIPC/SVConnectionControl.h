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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIPC\SVConnectionControl.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:09:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 10:30:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:24:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 11:24:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
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
 *    Rev 1.1   Mar 23 2000 16:34:00   mike
 * Fixed memory leak
 * 
 *    Rev 1.0   Mar 22 2000 17:25:18   mike
 * Initial revision.
*/