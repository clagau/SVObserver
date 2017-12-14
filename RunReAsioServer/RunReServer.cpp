//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file RunReServer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "StdAfx.h"
#include "RunReApi\format.pb.h"
#include "ShareControl.h"
#include "LastResponseData.h"
#include "RunReServer.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"


RunReServer::RunReServer()
{
	m_pLastResponseData = std::make_unique<CLastResponseData>();
	m_pShareControl = std::make_unique<CShareControl>(m_pLastResponseData.get());
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(
		boost::bind(&CShareControl::EventHandler,boost::ref((*m_pShareControl.get())), _1 ));
	SvSml::ShareEvents::GetInstance().StartWatch();
	
}
RunReServer::~RunReServer()
{
	SvSml::ShareEvents::GetInstance().StopWatch();
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(0);
}

void RunReServer::GetVersion(const GetVersionRequest& req, GetVersionResponse& resp)
{
	std::string version = SvSyl::SVVersionInfo::GetVersion();
	resp.set_version(version);
}


void RunReServer::GetProduct(const GetProductRequest& request, GetProductResponse& resp)
{
	return GetProductOrReject(true, request, resp);
}

void RunReServer::GetProductOrReject(bool bProduct, const GetProductRequest& request, GetProductResponse& resp)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		resp.set_status(State::SharedMemoryBlocked);
		return;
	}
	bool nameInresponse(false);
	if (request.has_nameinresponse())
	{
		nameInresponse = request.nameinresponse();
	}
	resp.set_status(State::InternalError);
	bool bValid(false);
	if (request.has_name())
	{
		bValid = m_pShareControl->m_MemReader.IsActiveMonitorList(request.name());
	}
	if (!bValid)
	{
		resp.set_status(State::InvalidMonitorName);
		return;
	}
	SvSml::MLProduct* pLastProduct(nullptr);
	SvSml::productPointer new_productPtr(new SvSml::MLProduct());
	auto& LastResponseMap(bProduct ? m_pLastResponseData->m_LastProduct : m_pLastResponseData->m_LastReject);
	auto& it = LastResponseMap.find(request.name());
	if (it != LastResponseMap.end())
	{
		pLastProduct = it->second.get();
	}
	int trig(-1);
	if (request.has_triggercount())
	{
		trig = request.triggercount();
	}
	SvSml::SharedMemReader::retvalues ret(SvSml::SharedMemReader::fail);
	if (bProduct)
	{
		ret = m_pShareControl->m_MemReader.GetProductData(request.name().c_str(), trig, new_productPtr.get(), pLastProduct, FALSE);
	}
	else
	{
		ret = m_pShareControl->m_MemReader.GetRejectData(request.name().c_str(), trig, new_productPtr.get(), pLastProduct, FALSE);
	}
	switch (ret)
	{
	case SvSml::SharedMemReader::sucess:
	{
		DWORD index = m_pShareControl->m_MemReader.GetSlotManagerIndexForMonitorList(request.name().c_str());
		if (nullptr != pLastProduct)
		{
			///release Readerslot for last product
			if (index < 0)
			{
				//@Todo[MEC][8.00] [19.10.2017] log errors 
				resp.set_status(State::InternalError);
				return;
			}
			m_pShareControl->m_MemReader.GetSlotManager(index)->ReleaseReaderSlot(pLastProduct->m_slot);
		}
		///the last become the new one 
		//rslt = WriteProduct<SvSol::UdpApi>(new_productPtr.get(), pMemReader);
		GetProduct(new_productPtr.get(), request, resp);

		LastResponseMap[request.name()] = std::move(new_productPtr);
	}
	break;
	case SvSml::SharedMemReader::last:
	{
		//rslt = WriteProduct<SvSol::UdpApi>(pLastProduct, pMemReader);
		GetProduct(pLastProduct, request, resp);
	}
	break;
	case SvSml::SharedMemReader::fail:
	{
		{
			resp.set_status(State::NotFound);
			return;
		}
	}
	break;
	}
}
void RunReServer::GetFailstatus(SvSml::Failstatus* pFailstatus, const GetFailStatusRequest& request, GetFailStatusResponse& resp)
{
	bool nameInresponse(false);
	if (request.has_nameinresponse())
	{
		nameInresponse = request.nameinresponse();
	}

	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		resp.set_status(State::SharedMemoryBlocked);
		return;
	}
	if (nullptr == pFailstatus)
	{
		resp.set_status(State::NotFound);
		return;
	}
	else
	{
		resp.set_status(IsValid);
	}
	for (SvSml::productPointer& pProduct : *pFailstatus)
	{
		auto pProductResponse = resp.add_productresponse();
		SetProductResponse(nameInresponse, pProduct.get(), pProductResponse);
	}

}

void RunReServer::SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, GetProductResponse* pResp)
{
	if (nullptr == pProduct)
	{
		pResp->set_status(State::NotFound);
		return;
	}

	pResp->set_tiggercount((int)pProduct->m_trigger);
	//??resp.set_status((int)pProduct->m_status);
	pResp->set_status(State::IsValid);
	for (int i = 0; i < pProduct->m_data.size(); i++)
	{
		auto pval = pResp->add_values();
		//@Todo[MEC][8.00] [19.10.2017] use variant
		pval->set_string_value(pProduct->m_data[i]->c_str());
		if (nameInResponse)
		{
			*(pResp->add_valuenames()) = (pProduct->m_dataEntries[i]->name);
		}
	}
	int slot = pProduct->m_slot;
	int i(0);
	for (auto& MeP : pProduct->m_ImageEntries)
	{

		if (!MeP.get())
			break;
		auto pval = pResp->add_images();
		pval->set_slotindex(slot);
		pval->set_imageindex(MeP->data.ItemId);
		pval->set_imagestore(MeP->data.InspectionStoreId);

		if (nameInResponse)
		{
			*(pResp->add_imagenames()) = (pProduct->m_ImageEntries[i++]->name);
		}
	}
}


void RunReServer::GetProduct(SvSml::MLProduct* pProduct, const GetProductRequest& request, GetProductResponse& resp)
{
	bool nameInresponse(false);
	if (request.has_nameinresponse())
	{
		nameInresponse = request.nameinresponse();
	}
	
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		resp.set_status(State::SharedMemoryBlocked);
		return;
	}

	SetProductResponse(nameInresponse, pProduct, &resp);
	
}
	
void RunReServer::GetReject(const GetProductRequest& request, GetProductResponse& resp)
{
	return GetProductOrReject(false, request, resp);
}
void RunReServer::GetFailstatus(const GetFailStatusRequest& request, GetFailStatusResponse& resp)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		resp.set_status(State::SharedMemoryBlocked);
		return;
	}
	bool nameInresponse(false);
	if (request.has_nameinresponse())
	{
		nameInresponse = request.nameinresponse();
	}
	resp.set_status(State::InternalError);
	bool bValid(false);
	if (request.has_name())
	{
		bValid = m_pShareControl->m_MemReader.IsActiveMonitorList(request.name());
	}
	if (!bValid)
	{
		resp.set_status(State::InvalidMonitorName);
		return;
	}

	SvSml::Failstatus* pLastFailstatus(nullptr);
	SvSml::FailstatusPointer  new_FailstatusPtr(new SvSml::Failstatus);
	auto& it = m_pLastResponseData->m_LastFailstatus.find(request.name());
	if (it != m_pLastResponseData->m_LastFailstatus.end())
	{
		pLastFailstatus = it->second.get();
	}

	SvSml::SharedMemReader::retvalues ret =
		m_pShareControl->m_MemReader.GetFailstatus(request.name().c_str(), new_FailstatusPtr.get(), pLastFailstatus);
	switch (ret)
	{
	case SvSml::SharedMemReader::sucess:
	{
		GetFailstatus(new_FailstatusPtr.get(), request, resp);
		m_pLastResponseData->m_LastFailstatus[request.name().c_str()] = std::move(new_FailstatusPtr);
	}
	break;
	case SvSml::SharedMemReader::last:
	{
		GetFailstatus(pLastFailstatus, request, resp);
		

	}
	break;
	case SvSml::SharedMemReader::fail:
	{

		resp.set_status(State::NotFound);
		return;
	}
	break;
	}

}
void RunReServer::GetImageFromCurId(const GetImageFromCurIdRequest& req, GetImageFromCurIdResponse& resp)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		resp.set_status(State::SharedMemoryBlocked);
		return;
	}
	int storeindex = req.id().imagestore();
	int slotindex = req.id().slotindex();
	int Imageindex = req.id().imageindex();

	SVBitmapInfo bitmapInfo;
	auto pData = resp.mutable_imagedata();
	const SVMatroxBuffer& rFromId = 
		m_pShareControl->m_MemReader.m_DataContainer.GetImageBuffer(slotindex, storeindex, Imageindex);
	//@Todo[MEC][8.00] [22.11.2017] check with false 
	SVMatroxBufferInterface::CopyBufferToFileDIB(*(pData->mutable_rgb()), bitmapInfo, rFromId, 0, false);
	pData->set_w(std::abs(bitmapInfo.GetWidth()));
	pData->set_h(std::abs(bitmapInfo.GetHeight()));
	
	resp.set_status(State::IsValid);
}
void RunReServer::GetItems(const GetItemsRequest&, GetItemsResponse& resp)
{
	resp.set_status(State::NotImplemented);
}
void RunReServer::QueryListName(const QueryListNameRequest&, QueryListNameResponse& resp)
{
	SvDef::StringVector stringVector;
	m_pShareControl->m_MemReader.GetAllActiveMonitorlistName(stringVector);
	for (auto& itN : stringVector)
	{
		*(resp.add_listname()) = itN;
	}
	;
}
void RunReServer::QueryListItem(const QueryListItemRequest&, QueryListItemResponse&)
{
	;
}