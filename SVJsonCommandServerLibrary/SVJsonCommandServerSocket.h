//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServerSocket
//* .File Name       : $Workfile:   SVJsonCommandServerSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   06 Aug 2013 11:09:26  $
//******************************************************************************

#ifndef SVJSONCOMMANDSERVERSOCKET_H
#define SVJSONCOMMANDSERVERSOCKET_H

#include <deque>
#include <boost/function.hpp>
#include "SVSocketLibrary/SVJsonPacket.h"
#include "SVSocketLibrary/SVServerSocket.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVSystemLibrary/SVThread.h"

class SVJsonCommandServerSocket
{
public:
	typedef boost::function<void ()> ConnectionAcceptedCallback;
	typedef boost::function<void ( const std::string& data )> DataReceivedCallback;

	SVJsonCommandServerSocket();
	virtual ~SVJsonCommandServerSocket();

	void SetConnectionAcceptedCallback(ConnectionAcceptedCallback func);
	void SetDataReceivedCallback(DataReceivedCallback func);

	bool Start(unsigned short portNo);
	void Stop();
	bool Write(const std::string& data);

	bool CanAccept() const;
	bool HasClient() const;
	
private:
	typedef std::deque< std::string > SVWriteDeque;
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	static void CALLBACK OnAPCEvent(DWORD data);

	void OnAccept();
	void OnDataReceived(const std::string& data);
	void ProcessDataRead(const char* p_pBuf);

	void CloseClient();

	bool SameClient(const sockaddr_in& addr) const;
	void CheckConnection();
	void ThreadProcessHandler(bool& bWaitEvents);

	SVServerSocket m_server;
	SVSocket m_client;
	ConnectionAcceptedCallback m_connectionAcceptedFunc;
	DataReceivedCallback m_dataReceivedFunc;
	SVJsonPacket m_dataBuffer;
	SVThread<SVThreadProcessHandler> m_thread;

	SVCriticalSection m_WriteQueueLock;
	SVWriteDeque m_WriteQueue;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonCommandServerSocket.h_v  $
 * 
 *    Rev 1.2   06 Aug 2013 11:09:26   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed disconnect problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2013 15:11:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 May 2013 08:48:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to create a write queue for the socket and update client socket options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:15:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
