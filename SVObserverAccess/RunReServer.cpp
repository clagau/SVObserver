//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file RunReServer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// the runrejectServer class implements the runreject function. 
/// This function access the data in the shared memory 
//******************************************************************************
#include "StdAfx.h"
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "RunReApi\RunReApi.pb.h"
#pragma warning( pop )

#include "ShareControl.h"
#include "LastResponseData.h"
#include "RunReServer.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"




RunReServer::RunReServer()
{
	m_pLastResponseData = std::make_unique<LastResponseData>();
	m_pShareControl = std::make_unique<ShareControl>(m_pLastResponseData.get());
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(
		boost::bind(&ShareControl::EventHandler, boost::ref((*m_pShareControl.get())), _1));
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
	bool nameInresponse = request.nameinresponse();

	resp.set_status(State::IsValid);
	bool bValid(false);
	if (request.name().length() > 0)
	{
		bValid = m_pShareControl->m_MemReader.IsActiveMonitorList(request.name());
	}
	if (!bValid)
	{
		resp.set_status(State::InvalidMonitorName);
		return;
	}
	SvSml::MLProduct* pLastProduct(nullptr);
	SvSml::pProd new_productPtr(new SvSml::MLProduct());
	{
		std::lock_guard<std::mutex> guard(bProduct ? m_pLastResponseData->m_ProtectLastProduct : m_pLastResponseData->m_ProtectLastReject);
		auto& LastResponseMap(bProduct ? m_pLastResponseData->m_LastProduct : m_pLastResponseData->m_LastReject);

		auto& it = LastResponseMap.find(request.name());
		if (it != LastResponseMap.end())
		{
			pLastProduct = it->second.get();
		}
		int trig = request.trigger();
		int prevTrigger = request.pevioustrigger();

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

				GetProduct(new_productPtr.get(), request, resp);

				LastResponseMap[request.name()] = std::move(new_productPtr);
			}
			break;
		case SvSml::SharedMemReader::last:
			{
				if (prevTrigger == pLastProduct->m_trigger)
				{
					resp.set_status(State::Unchanged);
				}
				else
				{
					GetProduct(pLastProduct, request, resp);
				}
			}
			break;
		case SvSml::SharedMemReader::fail:
			{
				{
					resp.set_status(State::NotFound);

				}
			}
			break;
		}
	}
}
void RunReServer::GetFailstatus(SvSml::vecpProd* pFailstatus, const GetFailStatusRequest& request, GetFailStatusResponse& resp)
{
	bool nameInresponse = request.nameinresponse();

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
	for (SvSml::pProd& pProduct : *pFailstatus)
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

	pResp->set_trigger((int)pProduct->m_trigger);
	//??resp.set_status((int)pProduct->m_status);
	pResp->set_status(State::IsValid);
	for (int i = 0; i < pProduct->m_data.size(); i++)
	{
		auto pval = pResp->add_values();
		//@Todo[MEC][8.00] [19.10.2017] use variant 
		pval->set_string_value(pProduct->m_data[i]->c_str());
		pval->set_varianttype(VT_BSTR);
		if (nameInResponse)
		{
			*(pResp->add_valuenames()) = (pProduct->m_dataEntries[i]->name);
		}
	}
	int slot = pProduct->m_slot;
	//pResp->set_slotindex(slot);
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
	bool nameInresponse = request.nameinresponse();
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
	bool nameInresponse = request.nameinresponse();

	resp.set_status(State::InternalError);
	bool bValid(false);
	if (request.name().size() > 0)
	{
		bValid = m_pShareControl->m_MemReader.IsActiveMonitorList(request.name());
	}
	if (!bValid)
	{
		resp.set_status(State::InvalidMonitorName);
		return;
	}
	SvSml::vecpProd* pLastFailstatus(nullptr);
	std::unique_ptr<SvSml::vecpProd>  new_FailstatusPtr(new SvSml::vecpProd);
	{
		std::lock_guard<std::mutex> guard(m_pLastResponseData->m_ProtectLastFailstatus);
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
			}
			break;
		}
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
	//@Todo[MEC][8.00] [22.11.2017] true is for testscript for binary compare  false is faster 
	SVMatroxBufferInterface::CopyBufferToFileDIB(*(pData->mutable_rgb()), bitmapInfo, rFromId, 0, true);
	pData->set_w(std::abs(bitmapInfo.GetWidth()));
	pData->set_h(std::abs(bitmapInfo.GetHeight()));
	resp.set_status(State::IsValid);
}
void RunReServer::GetItems(const GetItemsRequest&, GetItemsResponse& resp)
{
	resp.set_status(State::NotImplemented);
}
void RunReServer::QueryListName(const QueryListNameRequest& req, QueryListNameResponse& resp)
{
	m_pShareControl->QueryListName(req, resp);
}

void RunReServer::QueryListItem(const QueryListItemRequest& req, QueryListItemResponse& resp)
{
	m_pShareControl->QueryListItem(req, resp);
}

