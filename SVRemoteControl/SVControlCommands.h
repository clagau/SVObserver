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
#include <boost/asio/io_service.hpp>
#include "WebsocketLibrary/SVRCClientService.h"
#include "SVRPCLibrary/RPCClient.h"
#pragma endregion Includes

typedef boost::function< void(_variant_t&, SVNotificationTypesEnum) > NotifyFunctor;

class SVControlCommands
{
public:
	virtual ~SVControlCommands();
	SVControlCommands(NotifyFunctor p_Func);
	SVControlCommands() = delete;

	HRESULT SetConnectionData(const _bstr_t& p_rServerName, unsigned short p_CommandPort, long timeout);

	HRESULT GetVersion(_bstr_t& rSVObserverVersion, _bstr_t& rRunRejectServerVersion, SVCommandStatus& rStatus);
	HRESULT GetState(unsigned long& rState, SVCommandStatus& rStatus);
	HRESULT GetOfflineCount(unsigned long& rCount, SVCommandStatus& rStatus);
	HRESULT GetConfigReport(BSTR& rReport, SVCommandStatus& rStatus);
	HRESULT GetMode(unsigned long& rMode, SVCommandStatus& rStatus);
	HRESULT SetMode(unsigned long Mode, SVCommandStatus& rStatus);
	HRESULT GetItems(CComVariant ItemNames, ISVProductItems** ppItems, SVCommandStatus& rStatus);
	HRESULT SetItems(ISVProductItems* pItems, ISVProductItems** ppErrors, SVCommandStatus& rStatus);
	HRESULT GetConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus);
	HRESULT PutConfig(const _bstr_t& rFilePath, SVCommandStatus& rStatus);
	HRESULT PutFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, SVCommandStatus& rStatus);
	HRESULT GetFile(const _bstr_t& rSourcePath, const _bstr_t& rDestinationPath, SVCommandStatus& rStatus);
	HRESULT GetDataDefinitionList(const _bstr_t& p_rInspectionName, long p_ListType, ISVDataDefObjectList** p_ppEntries, SVCommandStatus& p_rStatus);
	HRESULT RegisterMonitorList(const _bstr_t & rListName, const _bstr_t& rPpqName, int RejectDepth, const CComVariant& rProductItemList,
		const CComVariant& rRejectCondList, const CComVariant& rFailStatusList, ISVErrorObjectList** ppErrors, SVCommandStatus& rStatus);
	HRESULT QueryMonitorList(const _bstr_t& rListName, SvPb::ListType Type, CComVariant& rItemNames, SVCommandStatus& rStatus);

	HRESULT GetProduct(bool bGetReject, const _bstr_t& rListName, long TriggerCount, long ImageScale, ISVProductItems** ppViewItems, SVCommandStatus& rStatus);
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
	bool isConnected() const { return m_Connected; }

private:
	void OnConnectionStatus(SvRpc::ClientStatus Status);

	_bstr_t m_ServerName;
	unsigned short m_CommandPort;
	bool m_Connected;

	NotifyFunctor m_Notifier;

	std::unique_ptr<SvRpc::RPCClient> m_pRpcClient;
	std::unique_ptr<SvWsl::SVRCClientService> m_pSvrcClientService;
};

