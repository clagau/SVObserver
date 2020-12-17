//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file RemoteControl.h
/// The exported functions of the remote control dll header file
//******************************************************************************

#pragma once
typedef std::function<void(const _variant_t&, long)> NotifyFunctor;

namespace SvRc
{
struct Value;
struct Product;
struct DataDefinition;
struct MonitorList;

class RemoteControl
{
public:
	RemoteControl() = default;
	~RemoteControl() = default;

	HRESULT Connect(LPCTSTR serverName, long timeout);
	void Disconnect();
	bool isConnected() const;

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
	HRESULT QueryProductList(LPCTSTR listName, variant_t& rItemNames) const;
	HRESULT QueryRejectConditionList(LPCTSTR listName, variant_t& rItemNames) const;
	HRESULT QueryFailStatusList(LPCTSTR listName, variant_t& rItemNames) const;
	HRESULT RegisterMonitorList(const SvRc::MonitorList& rMonitorList, std::vector<SvRc::Value>& rErrorVector) const;
	HRESULT GetProduct(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const;
	HRESULT GetReject(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const;
	HRESULT GetFailStatus(LPCTSTR listName, std::vector<SvRc::Product>& rProductVector) const;
	HRESULT GetProductFilter(LPCTSTR listName, unsigned long& rFilter) const;
	HRESULT SetProductFilter(LPCTSTR listName, unsigned long filter) const;
	HRESULT SetTriggerConfig(LPCTSTR plcSimulatedFile) const;

	void SetNotifyCallback(NotifyFunctor pNotifier) const;
	HRESULT ShutDown(long option) const;
};
} //namespace SvRc