//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocket
//* .File Name       : $Workfile:   SVSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 Aug 2013 11:11:40  $
//******************************************************************************
#pragma once

#include <winsock2.h>
#include <string>
#include "SVSocketError.h"

class SVSocket
{
protected:
	SOCKET m_socket;
	sockaddr_in m_addr;

public:
	SVSocket();
	virtual ~SVSocket();
	
	virtual SVSocketError::ErrorEnum Create();
	virtual void Destroy();

	ULONG GetAddr() const;

	SVSocketError::ErrorEnum SetNonBlocking();
	SVSocketError::ErrorEnum SetBlocking();
	SVSocketError::ErrorEnum SetKeepAliveOption(bool bEnable);
	SVSocketError::ErrorEnum SetLingerOption(bool bEnable, unsigned short nTime);
	SVSocketError::ErrorEnum SetOptionReuseAddr();
	SVSocketError::ErrorEnum DisableDelay();
	SVSocketError::ErrorEnum EnableDelay();
	
	SVSocketError::ErrorEnum Write(const std::string& data);
	SVSocketError::ErrorEnum Write(const unsigned char* buffer, size_t len);
	SVSocketError::ErrorEnum Read(unsigned char* buffer, size_t len, size_t& amtRead);

	operator SOCKET() const;
	void Attach(SOCKET socket, const sockaddr_in& addr);
	SOCKET Release();
	bool IsValidSocket() const;
	
protected:
	SVSocketError::ErrorEnum Bind(const char* hostAddr, unsigned short portNo);

private:
	// not copyable
	SVSocket(const SVSocket& socket); 
	SVSocket& operator=(const SVSocket& socket) const; 

};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVSocket.h_v  $
 * 
 *    Rev 1.1   06 Aug 2013 11:11:40   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed disconnect problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:15:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:27:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
