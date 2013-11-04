//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketLibrary
//* .File Name       : $Workfile:   SVSocketLibrary.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:16:58  $
//******************************************************************************
#include "stdafx.h"
#include <winsock2.h>
#include "SVSocketLibrary.h"

SVSocketError::ErrorEnum SVSocketLibrary::Init()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	WSADATA hWSAData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &hWSAData);
	if (iResult != NO_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

SVSocketError::ErrorEnum SVSocketLibrary::Destroy()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	int iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSocketLibrary\SVSocketLibrary.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:16:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
