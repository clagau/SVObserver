//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRCClientSocket
//* .File Name       : $Workfile:   SVRCClientSocket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Oct 2014 10:04:32  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <comdef.h>
#include <string>
#include <istream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "SVSocketLibrary/SVClientSocket.h"
#include "SVSocketLibrary/SVSocketThread.h"
#include "SVSocketLibrary/SVJsonStreamPacket.h"
#include "SVCriticalSection.h"
#include "SVAutoLockAndReleaseTemplate.h"
#pragma endregion Includes


namespace SvSol
{
	enum ConnectionState
	{
		Disconnected = 0,
		Connecting = 1,
		Connected = 2
	};

	template<typename API>
	class SVRCClientSocket
	{
	public:
		typedef typename SvSol::SVClientSocket<API> ClientSocket;
		typedef boost::function< void(SvSol::ConnectionState ) > ConnectionStatusCallback;
		typedef boost::function< void( std::istream& is ) > DataReceivedCallback;

		SVRCClientSocket();
		~SVRCClientSocket();

		void SetConnectionStatusCallback( ConnectionStatusCallback func );
		void SetDataReceivedCallback( DataReceivedCallback func );

		SvSol::SVSocketError::ErrorEnum BuildConnection( const _bstr_t& p_rAddr, unsigned short portNo, DWORD p_TimeoutInMilliseconds );
		void Disconnect();
		bool IsConnected() const;
		void Close();
		SvSol::SVSocketError::ErrorEnum Write( const std::string& data );

		std::string GetLocalIPAddr() const;

		SvSol::SVSocketError::ErrorEnum SetNonBlocking();
		SvSol::SVSocketError::ErrorEnum SetBlocking();

	private:
		typedef boost::function< void() > SVSocketThreadFunc;

		ClientSocket m_socket;
		_bstr_t m_serverIPAddr;
		unsigned short m_serverPortNo;
		ConnectionState m_state;
		SvSol::SVJsonStreamPacket m_dataBuffer;

		ConnectionStatusCallback m_connectionStatusFunc;
		DataReceivedCallback m_dataReceivedFunc;

		SvSol::SVSocketThread< SVSocketThreadFunc > m_thread;

		HANDLE m_WaitHandle;

		mutable SVCriticalSection m_CriticalSection;
		_bstr_t m_newServerIPAddr;
		unsigned short m_newServerPortNo;

		void ThreadFunc();
		void Destroy();
		void OnConnectionStatus();
		void OnDataReceived(std::istream& is);
		void ProcessDataRead(const char* p_Buf);

		HRESULT NotifyRequestComplete() const;
		HRESULT WaitReset() const;
		HRESULT WaitTimeout() const;
		HRESULT WaitForRequest( DWORD p_TimeoutInMilliseconds ) const;
	};
}

#include "SVRCClientSocket.inl"

typedef SvSol::SVRCClientSocket<SvSol::TcpApi> SVRCClientSocket;
