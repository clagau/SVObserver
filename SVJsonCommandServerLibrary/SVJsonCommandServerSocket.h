//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCommandServerSocket
//* .File Name       : $Workfile:   SVJsonCommandServerSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 09:26:18  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <boost/function.hpp>
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

	static void CALLBACK OnAPCEvent( ULONG_PTR data );

	void OnAccept();
	void OnDataReceived(const std::string& data);
	void ProcessDataRead(const char* p_pBuf);

	void CloseClient();

	bool SameClient(const sockaddr_in& addr) const;
	void CheckConnection();
	void ThreadProcessHandler(bool& bWaitEvents);

	SvSol::SVServerSocket<SvSol::TcpApi> m_server;
	SvSol::SVSocket<SvSol::TcpApi> m_client;
	ConnectionAcceptedCallback m_connectionAcceptedFunc;
	DataReceivedCallback m_dataReceivedFunc;
	SvSol::SVJsonPacket m_dataBuffer;
	SVThread<SVThreadProcessHandler> m_thread;

	SVCriticalSection m_WriteQueueLock;
	SVWriteDeque m_WriteQueue;
};

