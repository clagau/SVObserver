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
#include "SVCommandStatus.h"
#include "SVControlResponse.h"
#include "SVRemoteCtrl.h"

#include <Thread>
#include <boost/chrono/duration.hpp>
#include "WebsocketLibrary/SVRCClientService.h"
#include "SVRPCLibrary/RPCClient.h"
#include "MessageNotificationHandler.h"
#include "NotificationHandler.h"
#include "RCSettings.h"
#pragma endregion Includes



typedef std::function< void(_variant_t&, SVNotificationTypesEnum) > NotifyFunctor;

class SVControlCommands
{
public:
	virtual ~SVControlCommands();
	SVControlCommands(NotifyFunctor p_Func);
	SVControlCommands() = delete;

	HRESULT SetConnectionData(const _bstr_t& p_rServerName, boost::posix_time::time_duration timeout);
	void ResetConnection();
	void  StartNotificationStreaming();
	void  StopNotificationStreaming();


	HRESULT GetVersion(_bstr_t& rSVObserverVersion, _bstr_t& rRunRejectServerVersion, SVCommandStatus& rStatus);
	HRESULT GetState(unsigned long& rState, SVCommandStatus& rStatus);
	HRESULT GetOfflineCount(unsigned long& rCount, SVCommandStatus& rStatus);
	HRESULT GetConfigReport(BSTR& rReport, SVCommandStatus& rStatus);
	HRESULT GetMode(unsigned long& rMode, SVCommandStatus& rStatus);
	HRESULT SetMode(unsigned long Mode, SVCommandStatus& rStatus);
	HRESULT GetItems(CComVariant ItemNames, ISVProductItems** ppItems, SVCommandStatus& rStatus);
	HRESULT SetItems(ISVProductItems* pItems, bool RunOnce, ISVProductItems** ppErrors, SVCommandStatus& rStatus);
	HRESULT RunOnce(const _bstr_t& rInspectionName, SVCommandStatus& rStatus);
	HRESULT GetConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus);
	HRESULT PutConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus);
	HRESULT LoadConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus);
	HRESULT PutFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, bool saveInConfig, SVCommandStatus& rStatus);
	HRESULT GetFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, SVCommandStatus& rStatus);
	HRESULT GetDataDefinitionList(const _bstr_t& p_rInspectionName, long p_ListType, ISVDataDefObjectList** p_ppEntries, SVCommandStatus& p_rStatus);
	HRESULT RegisterMonitorList(const _bstr_t & rListName, const _bstr_t& rPpqName, int RejectDepth, const CComVariant& rProductItemList,
		const CComVariant& rRejectCondList, const CComVariant& rFailStatusList, ISVErrorObjectList** ppErrors, SVCommandStatus& rStatus);
	HRESULT QueryMonitorList(const _bstr_t& rListName, SvPb::ListType Type, CComVariant& rItemNames, SVCommandStatus& rStatus);

	HRESULT GetProduct(const _bstr_t& rListName, long TriggerCount, long ImageScale, ISVProductItems** ppViewItems, SVCommandStatus& rStatus);
	HRESULT GetRejects(const _bstr_t& rListName, long TriggerCount, long ImageScale, ISVProductItems** ppViewItems, SVCommandStatus& rStatus);
	HRESULT ActivateMonitorList(const _bstr_t& rListName, bool Active, SVCommandStatus& rStatus);
	HRESULT GetFailStatus(const _bstr_t& rListName, CComVariant & values, SVCommandStatus& rStatus);
	HRESULT ShutDown(SVShutdownOptionsEnum Options, SVCommandStatus& p_rStatus);
	HRESULT QueryMonitorListNames(CComVariant& rListName, SVCommandStatus& rStatus);
	HRESULT GetInspectionNames(CComVariant& rListName, SVCommandStatus& rStatus);
	HRESULT GetMonitorListProperties(const _bstr_t& rListName, long&  rRejectDepth, bool& rIsActive, BSTR&  rPpqName, SVCommandStatus& rStatus);
	HRESULT GetMaxRejectQeueDepth(unsigned long& rDepth, SVCommandStatus& rStatus);

	HRESULT GetProductFilter(const _bstr_t& rListName, unsigned long& rFilter, SVCommandStatus& p_rStatus);
	HRESULT SetProductFilter(const _bstr_t& rListName, unsigned long filter, SVCommandStatus& p_rStatus);
	bool isConnected() const { return (m_pRpcClient && m_pRpcClient->isConnected()); }
	const NotifyFunctor& getNotifier() const { return m_pNotifier; }

private:
	void OnConnectionStatus(SvRpc::ClientStatus Status);

	std::wstring ConvertResult(HRESULT hResult);

	NotifyFunctor m_pNotifier{ nullptr };

	SvRpc::ClientStreamContext m_csxNotification{ nullptr };
	SvRpc::ClientStreamContext m_csxMessageNotification{ nullptr };
	MessageNotificationHandler m_messageNotificationHandler;
	NotificationHandler m_notificationHandler;
	RCSettings m_settings;
	std::unique_ptr<SvRpc::RPCClient> m_pRpcClient;
	std::unique_ptr<SvWsl::SVRCClientService> m_pSvrcClientService;
};

