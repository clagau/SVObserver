//*****************************************************************************
/// \copyright (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ShareControl.h
/// All Rights Reserved 
//*****************************************************************************
/// ShareControl implements the connection to the shared memory 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SharedMemReader.h"
#include "LastResponseData.h"
#pragma endregion Includes

namespace SvSml
{
//! ShareControl implements the connection to the shared memory 

struct ShareControlSettings;

class ShareControl
{
public:
	explicit ShareControl(const ShareControlSettings& ControlParameter);
	~ShareControl();
	bool EventHandler(DWORD event);
	bool QueryListName(const SvPb::QueryListNameRequest& rRequest, SvPb::QueryListNameResponse& rResponse, SvPenv::Error& rError);
	bool QueryListItem(const SvPb::QueryListItemRequest& rRequest, SvPb::QueryListItemResponse& rResponse, SvPenv::Error& rError);
	bool GetProduct(const SvPb::GetProductRequest& rRequest, SvPb::GetProductResponse& rResponse, SvPenv::Error& rError);
	bool GetReject(const SvPb::GetRejectRequest& rRequest, SvPb::GetRejectResponse& rResponse, SvPenv::Error& rError);
	bool GetFailstatus(const SvPb::GetFailStatusRequest& rRequest, SvPb::GetFailStatusResponse& rResponse, SvPenv::Error& rError);
	bool GetImageFromId(const  SvPb::GetImageFromIdRequest& rRequest, SvPb::GetImageFromIdResponse& rResponse, SvPenv::Error& rError);
	

private:
	bool isReady(SvPenv::Error& rError);
	bool isMonitorListActive(const std::string& rListName, SvPenv::Error& rError);
	bool GetProductItem(bool isReject, int triggerCount, int peviousTrigger, const std::string& rListName, bool nameInResponse, SvPb::Product* pProductItem, SvPenv::Error& rError);
	bool SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, SvPb::Product* pProductMsg, SvPenv::Error& rError);
	bool GetFailstatus(SvSml::vecpProd* pFailstatus, const SvPb::GetFailStatusRequest& rRequest, SvPb::GetFailStatusResponse& rResponse, SvPenv::Error& rError);
private:
	SvSml::SharedMemReader   m_MemReader;
	std::unique_ptr<LastResponseData> m_pLastResponseData;
	DWORD m_DelayBeforeClearShare {30};
};
}

