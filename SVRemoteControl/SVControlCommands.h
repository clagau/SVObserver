//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVControlCommands
//* .File Name       : $Workfile:   SVControlCommands.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.14  $
//* .Check In Date   : $Date:   16 Oct 2014 10:19:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "JsonLib/include/json.h"
#include "SVSocketLibrary/SVClientSocket.h"
#include "SVSocketLibrary/SVSocketLibrary.h"
#include "SVCommandStatus.h"
#include "SVControlResponse.h"
#include "SVJsonCommandHelper.h"
#include "SVRCClientSocket.h"
#include "SVRemoteCtrl.h"
#include <Thread>
#include <boost/asio/io_service.hpp>
#include "WebsocketLibrary/clientservice.h"
#include "SVRPCLibrary/RPCClient.h"
#pragma endregion Includes


typedef CComPtr<ISVRemoteCtrl> RemoteCtrlPtr;
typedef CComPtr<ISVErrorObject> ErrorPtr;
typedef CComPtr<ISVErrorObjectList> ErrorList;
typedef CComPtr<ISVImageObject> ImagePtr;
typedef CComPtr<ISVImageObjectList> ImageList;
typedef CComPtr<ISVProductItems> ProductPtr;
typedef CComPtr<ISVValueObject> ValuePtr;
typedef CComPtr<ISVValueObjectList> ValueList;
typedef CComPtr<ISVDataDefObject> DataDefPtr;
typedef CComPtr<ISVDataDefObjectList> DataDefList;
typedef boost::function< void(_variant_t&, SVNotificationTypesEnum) > NotifyFunctor;

typedef SvSol::SVRCClientSocket<SvSol::UdpApi> SVRCRunpageSocket;



class SVControlCommands
{
public:
	virtual ~SVControlCommands();
	//! Connect to RRS only when connectToRRS is true
	SVControlCommands(NotifyFunctor p_Func, bool connectToRRS);

	HRESULT SetConnectionData(const _bstr_t& p_rServerName, unsigned short p_CommandPort, long timeout);

	HRESULT GetVPName(_bstr_t& p_rVPName, SVCommandStatus& p_rStatus);
	HRESULT GetVersion(_bstr_t& p_rSVObserverVersion, _bstr_t& p_rRunRejectServerVersion, SVCommandStatus& p_rStatus);
	HRESULT GetState(unsigned long& p_rState, SVCommandStatus& p_rStatus);
	HRESULT GetOfflineCount(unsigned long& p_rCount, SVCommandStatus& p_rStatus);
	HRESULT GetConfigReport(BSTR& p_rReport, SVCommandStatus& p_rStatus);
	HRESULT GetMode(unsigned long& p_rMode, SVCommandStatus& p_rStatus);
	HRESULT SetMode(unsigned long p_Mode, bool p_UseSyncSocket, SVCommandStatus& p_rStatus);
	HRESULT GetItems(CComVariant p_pItemNames, ISVProductItems** p_ppItems, LONG p_scale, SVCommandStatus& p_rStatus);
	HRESULT SetItems(ISVProductItems* p_pItems, ISVProductItems** faults, bool p_UseSyncSocket, SVCommandStatus& p_rStatus);
	HRESULT GetConfig(const _bstr_t& p_rFilePath, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus);
	HRESULT PutConfig(const _bstr_t& p_rFilePath, const _bstr_t& p_rProductName, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus);
	HRESULT PutFile(const _bstr_t& p_rSourcePath, const _bstr_t& p_rDestinationPath, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus);
	HRESULT GetFile(const _bstr_t& p_rSourcePath, const _bstr_t& p_rDestinationPath, long& p_rCancelFlag, bool p_UseSyncSocket, SVCommandStatus& p_rStatus);
	HRESULT GetDataDefinitionList(const _bstr_t& p_rInspectionName, long p_ListType, ISVDataDefObjectList** p_ppEntries, SVCommandStatus& p_rStatus);
	HRESULT RegisterMonitorList(const _bstr_t & listName, const _bstr_t & ppqName, int rejectDepth, const CComVariant & productItemList,
		const CComVariant & rejectCondList, const CComVariant & failStatusList, ISVErrorObjectList ** errors, SVCommandStatus& p_rStatus);
	HRESULT QueryProductList(const _bstr_t & listName, const std::string & cmd, CComVariant & itemNames, SVCommandStatus& p_rStatus);

	HRESULT GetProduct(bool bGetReject, const _bstr_t & listName, long triggerCount, long imageScale, ISVProductItems ** currentViewItems, SVCommandStatus& p_rStatus);
	HRESULT GetProduct(const _bstr_t & listName, long triggerCount, long imageScale, ISVProductItems ** currentViewItems, SVCommandStatus& p_rStatus);
	HRESULT GetRejects(const _bstr_t & listName, long triggerCount, long imageScale, ISVProductItems ** currentViewItems, SVCommandStatus& p_rStatus);
	HRESULT ActivateMonitorList(const _bstr_t & listName, bool activ, SVCommandStatus& p_rStatus);
	HRESULT GetFailStatus(const _bstr_t & listName, CComVariant & values, SVCommandStatus& p_rStatus);
	HRESULT ShutDown(SVShutdownOptionsEnum options, SVCommandStatus& p_rStatus);
	HRESULT QueryMonitorListNames(CComVariant & listNames, SVCommandStatus& p_rStatus);
	HRESULT GetInspectionNames(CComVariant & listNames, SVCommandStatus& p_rStatus);
	HRESULT GetMonitorListProperties(const _bstr_t & listName, long&  RejectDepth, bool& IsActive, BSTR&  ppqName, SVCommandStatus& p_rStatus);
	HRESULT GetMaxRejectQeueDepth(unsigned long& p_rState, SVCommandStatus& p_rStatus);

	HRESULT GetProductFilter(const _bstr_t& rListName, unsigned long& rFilter, SVCommandStatus& p_rStatus);
	HRESULT SetProductFilter(const _bstr_t& rListName, unsigned long filter, SVCommandStatus& p_rStatus);
	bool isConnected() const { return m_Connected; }





protected:
	template<typename API>
	SVControlResponse SendCommand(SvSol::SVRCClientSocket<API> & p_rSocket, const Json::Value & p_cmd, unsigned long p_TimeoutInSeconds = 1800);
	template<typename API>
	SVControlResponse SendRequest(SvSol::SVClientSocket<API> & sock, const Json::Value & req, unsigned long timeout = 500);
	template<typename API>
	SVControlResponse SendCommandWithFile(SvSol::SVRCClientSocket<API> & p_rSocket, const Json::Value & p_cmd, _bstr_t filepath, unsigned long p_TimeoutInSeconds = 1800);

	HRESULT CheckSocketConnection();
	template<typename API>
	HRESULT WriteToSocket(const std::string& data, SvSol::SVRCClientSocket<API> & sock);

	void OnConnectionStatus(SvSol::ConnectionState state);
	void OnControlDataReceived(std::istream& is);

	HRESULT Notify(const Json::Value& p_JsonValues);
	HRESULT BuildDatagram(CComBSTR& p_BSTR, const Json::Value& p_JsonValue, SVNotificationTypesEnum& p_Type);

	template<typename API>
	HRESULT SendFile(SvSol::SVRCClientSocket<API>& rSocket, std::istream& is);

	_bstr_t m_ServerName;
	unsigned short m_CommandPort;
	bool m_Connected;
	bool m_RRSConnected;

	SVRCClientSocket m_ClientSocket; // TCP socket to SVObserver

	SVJsonCommandHelper m_Command;
	NotifyFunctor m_Notifier;
	bool m_bConnectToRRS;
	std::unique_ptr<SvRpc::RPCClient> m_pRpcClient;
	std::unique_ptr<SvWsl::ClientService> m_pClientService;
	boost::posix_time::seconds m_RequestTimeout;


};

template<typename API>
inline HRESULT SVControlCommands::WriteToSocket(const std::string& data, SvSol::SVRCClientSocket<API> & sock)
{
	HRESULT l_Status = CheckSocketConnection();

	if (S_OK == l_Status)
	{
		l_Status = SvSol::SVSocketError::HrFromSocketError(sock.Write(data));
	}
	return l_Status;
}
