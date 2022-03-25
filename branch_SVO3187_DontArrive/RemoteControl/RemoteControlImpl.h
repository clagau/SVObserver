//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RemoteControlImpl.h
/// The remote control implimentation header file
//******************************************************************************

#pragma once
#pragma region Includes
#include "NotificationHandler.h"
#include "MessageNotificationHandler.h"
#include "RCSettings.h"
#include "SVRPCLibrary/RPCClient.h"
#include "WebsocketLibrary/SVRCClientService.h"
#pragma endregion Includes


typedef std::function<void(const _variant_t&, long)> NotifyFunctor;

namespace SvRc
{
struct Value;
struct Product;
struct DataDefinition;
struct MonitorList;

class RemoteControlImpl
{
public:
	RemoteControlImpl();
	virtual ~RemoteControlImpl();

	static RemoteControlImpl& Instance();

	HRESULT Connect(LPCTSTR serverName, long timeout);
	void Disconnect();
	bool isConnected() const { return (nullptr != m_pRpcClient && m_pRpcClient->isConnected()); }

	HRESULT LoadConfig(LPCTSTR sourceFile) const;
	HRESULT GetConfig(LPCTSTR destinationFile) const;
	HRESULT PutConfig(LPCTSTR sourceFile) const;
	HRESULT GetState(unsigned long& rState) const;
	HRESULT RunOnce(LPCTSTR inspectionName) const;
	HRESULT GetDeviceMode(long& rMode) const;
	HRESULT SetDeviceMode(long mode) const;
	HRESULT GetVersion(std::string& rSVObserver, std::string& rSVOGateway) const;
	HRESULT GetDeviceReport(std::string& rReport) const;
	HRESULT GetItems(const std::vector<std::string>& rItemNameVector, SvRc::Product& rProduct) const;
	HRESULT SetItems(SvRc::Product& rProduct, bool runOnce) const;
	HRESULT GetFile(LPCTSTR sourceFile, LPCTSTR destinationFile) const;
	HRESULT PutFile(LPCTSTR sourceFile, LPCTSTR destinationFile, bool saveInConfig) const;
	HRESULT GetInspectionNames(variant_t& rInspectionNames) const;
	HRESULT GetDataDefinitionList(LPCTSTR inspectionName, long listType, std::vector<DataDefinition>& rDataDefVector) const;
	HRESULT GetDeviceOfflineCount(long& rOfflineCount) const;

	HRESULT ActivateMonitorList(LPCTSTR listName, bool activate) const;
	HRESULT QueryMonitorListNames(variant_t& rNames) const;
	HRESULT QueryMonitorList(LPCTSTR listName, SvPb::ListType listType, variant_t& rItemNames) const;
	HRESULT RegisterMonitorList(const SvRc::MonitorList& rMonitorList, std::vector<SvRc::Value>& rErrorVector) const;
	HRESULT GetProduct(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const;
	HRESULT GetReject(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const;
	HRESULT GetFailStatus(LPCTSTR listName, std::vector<SvRc::Product>& rProductVector) const;
	HRESULT GetProductFilter(LPCTSTR listName, unsigned long& rFilter) const;
	HRESULT SetProductFilter(LPCTSTR listName, unsigned long filter) const;
	HRESULT SetTriggerConfig(LPCTSTR plcSimulatedFile) const;
	HRESULT SoftwareTrigger(const SvRc::TriggerItem& rTrigger) const;

	NotifyFunctor GetNotifyFunctor() const { return m_pNotifier; }
	void SetNotifyFunctor(NotifyFunctor pNotifier) { m_pNotifier = pNotifier; }

	HRESULT ShutDown(long option) const;

private:
	void  StartNotificationStreaming();
	void  StopNotificationStreaming();
	void OnConnectionStatus(SvRpc::ClientStatus Status);
	void ProtobufToProduct(SvPb::GetItemsResponse& rResponse, SvRc::Product& rProduct) const;
	void ProtobufToMonitorListProduct(const SvPb::Product& rProductItem, SvRc::Product& rProduct) const;
	SvRc::Value ProtobufToValue(const SvPb::Value& rValue) const;
	void StringVectorToProtobuf(const std::vector<std::string>& rStringVector, ::google::protobuf::RepeatedPtrField<::std::string>* pStringList) const;

	NotifyFunctor m_pNotifier;
	SvRpc::ClientStreamContext m_csxNotification{ nullptr };
	SvRpc::ClientStreamContext m_csxMessageNotification{ nullptr };
	MessageNotificationHandler m_messageNotificationHandler;
	NotificationHandler m_notificationHandler;
	RCSettings m_settings;
	std::unique_ptr<SvRpc::RPCClient> m_pRpcClient;
	std::unique_ptr<SvWsl::SVRCClientService> m_pSvrcClientService;

};
} //namespace SvRc