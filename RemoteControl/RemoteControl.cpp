//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file RemoteControl.cpp
/// (see header file of the same name)
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RemoteControl.h"
#include "RemoteControlImpl.h"
#include "RemoteStructs.h"
#pragma endregion Includes

namespace SvRc
{
HRESULT RemoteControl::Connect(LPCTSTR serverName, long timeout)
{
	return RemoteControlImpl::Instance().Connect(serverName, timeout);
}

void RemoteControl::Disconnect()
{
	RemoteControlImpl::Instance().Disconnect();
}

bool RemoteControl::isConnected() const
{
	return RemoteControlImpl::Instance().isConnected();
}

HRESULT RemoteControl::LoadConfig(LPCTSTR sourceFile) const
{
	return RemoteControlImpl::Instance().LoadConfig(sourceFile);
}

HRESULT RemoteControl::GetConfig(LPCTSTR destinationFile) const
{
	return RemoteControlImpl::Instance().GetConfig(destinationFile);
}

HRESULT RemoteControl::PutConfig(LPCTSTR sourceFile) const
{
	return RemoteControlImpl::Instance().PutConfig(sourceFile);
}

HRESULT RemoteControl::GetState(unsigned long& rState) const
{
	return RemoteControlImpl::Instance().GetState(rState);
}

HRESULT RemoteControl::RunOnce(LPCTSTR inspectionName) const
{
	return RemoteControlImpl::Instance().RunOnce(inspectionName);
}

HRESULT RemoteControl::GetDeviceMode(long& rMode) const
{
	return RemoteControlImpl::Instance().GetDeviceMode(rMode);
}

HRESULT RemoteControl::SetDeviceMode(long mode) const
{
	return RemoteControlImpl::Instance().SetDeviceMode(mode);
}

HRESULT RemoteControl::GetVersion(std::string& rSVObserver, std::string& rSVOGateway) const
{
	return RemoteControlImpl::Instance().GetVersion(rSVObserver, rSVOGateway);
}

HRESULT RemoteControl::GetDeviceReport(std::string& rReport) const
{
	return RemoteControlImpl::Instance().GetDeviceReport(rReport);
}

HRESULT RemoteControl::GetItems(const std::vector<std::string>& rItemNameVector, SvRc::Product& rProduct) const
{
	return RemoteControlImpl::Instance().GetItems(rItemNameVector, rProduct);
}

HRESULT RemoteControl::SetItems(SvRc::Product& rProduct, bool runOnce) const
{
	return RemoteControlImpl::Instance().SetItems(rProduct, runOnce);
}

HRESULT RemoteControl::GetFile(LPCTSTR sourceFile, LPCTSTR destinationFile) const
{
	return RemoteControlImpl::Instance().GetFile(sourceFile, destinationFile);
}

HRESULT RemoteControl::PutFile(LPCTSTR sourceFile, LPCTSTR destinationFile, bool saveInConfig) const
{
	return RemoteControlImpl::Instance().PutFile(sourceFile, destinationFile, saveInConfig);
}

HRESULT RemoteControl::GetInspectionNames(variant_t& rInspectionNames) const
{
	return RemoteControlImpl::Instance().GetInspectionNames(rInspectionNames);
}

HRESULT RemoteControl::GetDataDefinitionList(LPCTSTR inspectionName, long listType, std::vector<DataDefinition>& rDataDefVector) const
{
	return RemoteControlImpl::Instance().GetDataDefinitionList(inspectionName, listType, rDataDefVector);
}

HRESULT RemoteControl::GetDeviceOfflineCount(long& rOfflineCount) const
{
	return RemoteControlImpl::Instance().GetDeviceOfflineCount(rOfflineCount);
}

HRESULT RemoteControl::ActivateMonitorList(LPCTSTR listName, bool activate) const
{
	return RemoteControlImpl::Instance().ActivateMonitorList(listName, activate);
}

HRESULT RemoteControl::QueryMonitorListNames(variant_t& rNames) const
{
	return RemoteControlImpl::Instance().QueryMonitorListNames(rNames);
}

HRESULT RemoteControl::QueryProductList(LPCTSTR listName, variant_t& rItemNames) const
{
	return RemoteControlImpl::Instance().QueryMonitorList(listName, SvPb::ListType::productItem, rItemNames);
}

// cppcheck-suppress unusedFunction
HRESULT RemoteControl::QueryRejectConditionList(LPCTSTR listName, variant_t& rItemNames) const
{
	return RemoteControlImpl::Instance().QueryMonitorList(listName, SvPb::ListType::rejectCondition, rItemNames);
}

HRESULT RemoteControl::QueryFailStatusList(LPCTSTR listName, variant_t& rItemNames) const
{
	return RemoteControlImpl::Instance().QueryMonitorList(listName, SvPb::ListType::failStatus, rItemNames);
}

HRESULT RemoteControl::RegisterMonitorList(const SvRc::MonitorList& rMonitorList, std::vector<SvRc::Value>& rErrorVector) const
{
	return RemoteControlImpl::Instance().RegisterMonitorList(rMonitorList, rErrorVector);
}

HRESULT RemoteControl::GetProduct(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const
{
	return RemoteControlImpl::Instance().GetProduct(listName, triggerCount, rProduct);
}

HRESULT RemoteControl::GetReject(LPCTSTR listName, long triggerCount, SvRc::Product& rProduct) const
{
	return RemoteControlImpl::Instance().GetReject(listName, triggerCount, rProduct);
}

HRESULT RemoteControl::GetFailStatus(LPCTSTR listName, std::vector<SvRc::Product>& rProductVector) const
{
	return RemoteControlImpl::Instance().GetFailStatus(listName, rProductVector);
}

HRESULT RemoteControl::GetProductFilter(LPCTSTR listName, unsigned long& rFilter) const
{
	return RemoteControlImpl::Instance().GetProductFilter(listName, rFilter);
}

HRESULT RemoteControl::SetProductFilter(LPCTSTR listName, unsigned long filter) const
{
	return RemoteControlImpl::Instance().SetProductFilter(listName, filter);
}

// cppcheck-suppress unusedFunction
void RemoteControl::SetNotifyCallback(NotifyFunctor pNotifier) const
{
	RemoteControlImpl::Instance().SetNotifyFunctor(pNotifier);
}

HRESULT RemoteControl::ShutDown(long option) const
{
	return RemoteControlImpl::Instance().ShutDown(option);
}
} //namespace SvRc