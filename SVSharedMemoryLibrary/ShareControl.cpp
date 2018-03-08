//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareControl.cpp
/// All Rights Reserved 
//*****************************************************************************
/// ShareControl implements the connection to the shared memory 
//******************************************************************************
#include  "StdAfx.h"
#include "ShareControl.h"
#include "ShareEvents.h"
#include "LastResponseData.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "Definitions\StringTypeDef.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
namespace SvSml
{

ShareControl::ShareControl()
{
	m_pLastResponseData = std::make_unique<LastResponseData>();
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(boost::bind(&SvSml::ShareControl::EventHandler, boost::ref((*this)), _1));
	SvSml::ShareEvents::GetInstance().StartWatch();
}
ShareControl::~ShareControl()
{
	SvSml::ShareEvents::GetInstance().StopWatch();
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(0);
}

bool  ShareControl::QueryListName(const RRWS::QueryListNameRequest& req, RRWS::QueryListNameResponse& resp, SVRPC::Error& err)
{
	return m_MemReader.QueryListName(req, resp, err);
};
bool  ShareControl::QueryListItem(const RRWS::QueryListItemRequest& req, RRWS::QueryListItemResponse& resp, SVRPC::Error& err)
{
	return m_MemReader.QueryListItem(req, resp, err);
}

bool  ShareControl::GetProductOrReject(const RRWS::GetProductRequest& request, RRWS::GetProductResponse& rResp, SVRPC::Error& err)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		err.set_error_code(SVRPC::ErrorCode::Locked);
		err.set_message("Shared Memory is blocked");
		return false;
	}
	bool nameInresponse = request.nameinresponse();
	bool bProduct = (false == request.breject());

	bool bValid(false);
	if (request.name().length() > 0)
	{
		bValid = m_MemReader.IsActiveMonitorList(request.name());
	}
	if (!bValid)
	{
		err.set_error_code(SVRPC::ErrorCode::BadRequest);
		err.set_message("MonitorList with given name does not exist or is not active");
		return false;
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
			ret = m_MemReader.GetProductData(request.name().c_str(), trig, new_productPtr.get(), pLastProduct, FALSE);
		}
		else
		{
			ret = m_MemReader.GetRejectData(request.name().c_str(), trig, new_productPtr.get(), pLastProduct, FALSE);
		}
		switch (ret)
		{
			case SvSml::SharedMemReader::sucess:
			{
				DWORD index = m_MemReader.GetSlotManagerIndexForMonitorList(request.name().c_str());
				if (nullptr != pLastProduct)
				{
					///release Readerslot for last product
					if (index < 0)
					{
						//@Todo[MEC][8.00] [19.10.2017] log errors 
						err.set_error_code(SVRPC::ErrorCode::InternalError);
						err.set_message("Unable to get slot for requested MonitorList");
						return false;
					}
					m_MemReader.GetSlotManager(index)->ReleaseReaderSlot(pLastProduct->m_slot);
				}

				if (GetProduct(new_productPtr.get(), request.nameinresponse(), rResp.mutable_product(), err))
				{
					LastResponseMap[request.name()] = std::move(new_productPtr);

				}
				else
				{
					return false;
				}

				return true;
			}
			break;
			case SvSml::SharedMemReader::last:
			{
				if (prevTrigger == pLastProduct->m_trigger)
				{
					rResp.mutable_product()->set_status(RRWS::State::Unchanged);
				}
				else
				{
					if (false == GetProduct(pLastProduct, request.nameinresponse(), rResp.mutable_product(), err))
					{
						return false;
					}
				}
				return true;
			}
			break;
			case SvSml::SharedMemReader::fail:
			{
				err.set_error_code(SVRPC::ErrorCode::NotFound);
				return false;
			}
			break;
			default:
			{
				err.set_error_code(SVRPC::ErrorCode::InternalError);
				return false;
			}
			break;
		}
	}
}


bool  ShareControl::GetProduct(SvSml::MLProduct* pProduct, bool nameInResponse, RRWS::Product* pProductMsg, SVRPC::Error& err)
{

	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		err.set_error_code(SVRPC::ErrorCode::Locked);
		err.set_message("Shared Memory is blocked");
		return false;
	}

	return SetProductResponse(nameInResponse, pProduct, *pProductMsg, err);

}

bool  ShareControl::SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, RRWS::Product&  rProductMsg, SVRPC::Error& err)
{
	if (nullptr == pProduct)
	{
		err.set_error_code(SVRPC::ErrorCode::NotFound);
		return false;
	}

	rProductMsg.set_trigger((int)pProduct->m_trigger);
	rProductMsg.set_status(RRWS::State::IsValid);

	for (int i = 0; i < pProduct->m_data.size(); i++)
	{
		auto pval = rProductMsg.add_values();
		//@Todo[MEC][8.00] [19.10.2017] use variant 
		pval->set_string_value(pProduct->m_data[i]->c_str());
		pval->set_varianttype(VT_BSTR);
		if (nameInResponse)
		{
			*(rProductMsg.add_valuenames()) = (pProduct->m_dataEntries[i]->name);
		}
	}
	int slot = pProduct->m_slot;

	int i(0);
	for (auto& MeP : pProduct->m_ImageEntries)
	{

		if (!MeP.get())
			break;
		auto pval = rProductMsg.add_images();
		pval->set_slotindex(slot);
		pval->set_imageindex(MeP->data.ItemId);
		pval->set_imagestore(MeP->data.InspectionStoreId);

		if (nameInResponse)
		{
			*rProductMsg.add_imagenames() = pProduct->m_ImageEntries[i++]->name;
		}
	}
	return true;
}

bool ShareControl::GetFailstatus(SvSml::vecpProd* pFailstatus, const RRWS::GetFailStatusRequest& request, RRWS::GetFailStatusResponse& resp, SVRPC::Error& err)
{
	bool nameInresponse = request.nameinresponse();

	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		err.set_error_code(SVRPC::ErrorCode::Locked);
		err.set_message("Shared Memory is blocked");
		return false;
	}
	if (nullptr == pFailstatus)
	{
		err.set_error_code(SVRPC::ErrorCode::InternalError);
		return false;
	}

	for (SvSml::pProd& pProduct : *pFailstatus)
	{
		auto pProductMsg = resp.add_products();
		if (false == SetProductResponse(nameInresponse, pProduct.get(), *pProductMsg, err))
		{
			return false;
		}
	}
	return true;
}


bool  ShareControl::GetFailstatus(const RRWS::GetFailStatusRequest& request, RRWS::GetFailStatusResponse& resp, SVRPC::Error& err)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		err.set_error_code(SVRPC::ErrorCode::Locked);
		err.set_message("Shared Memory is blocked");
		return false;
	}
	bool nameInresponse = request.nameinresponse();

	bool bValid(false);
	if (!request.name().empty())
	{
		bValid = m_MemReader.IsActiveMonitorList(request.name());
	}
	if (!bValid)
	{
		SVRPC::Error err;
		err.set_error_code(SVRPC::ErrorCode::BadRequest);
		err.set_message("MonitorList with given name does not exist or is not active");
		return false;
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
			m_MemReader.GetFailstatus(request.name().c_str(), new_FailstatusPtr.get(), pLastFailstatus);
		switch (ret)
		{
			case SvSml::SharedMemReader::sucess:
			{
				if (false == GetFailstatus(new_FailstatusPtr.get(), request, resp, err))
				{
					return false;
				}
				m_pLastResponseData->m_LastFailstatus[request.name().c_str()] = std::move(new_FailstatusPtr);
			}
			break;
			case SvSml::SharedMemReader::last:
			{
				if (false == GetFailstatus(pLastFailstatus, request, resp, err))
				{
					return false;
				}


			}
			break;
			case SvSml::SharedMemReader::fail:
			{
				SVRPC::Error err;
				err.set_error_code(SVRPC::ErrorCode::InternalError);
				return false;
			}
			break;
		}
	}
	return true;
}

bool ShareControl::GetImageFromCurId(const  RRWS::GetImageFromCurIdRequest& req, RRWS::GetImageFromCurIdResponse& resp, SVRPC::Error& err)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		err.set_error_code(SVRPC::ErrorCode::Locked);
		err.set_message("Shared Memory is blocked");
		return false;
	}
	int storeindex = req.id().imagestore();
	int slotindex = req.id().slotindex();
	int Imageindex = req.id().imageindex();

	SVBitmapInfo bitmapInfo;
	auto pData = resp.mutable_imagedata();
	const SVMatroxBuffer& rFromId =
		m_MemReader.m_DataContainer.GetImageBuffer(slotindex, storeindex, Imageindex);
	//@Todo[MEC][8.00] [22.11.2017] true is for testscript for binary compare  false is faster 
	SVMatroxBufferInterface::CopyBufferToFileDIB(*(pData->mutable_rgb()), bitmapInfo, rFromId, 0, true);
	pData->set_w(std::abs(bitmapInfo.GetWidth()));
	pData->set_h(std::abs(bitmapInfo.GetHeight()));
	return true;
}

bool ShareControl::EventHandler(DWORD event)
{
	bool res(false);
	switch (event)
	{
		case SvSml::ShareEvents::Change:
			try
			{
				Sleep(SvSml::ShareEvents::Delay_Before_ClearShare);
				m_MemReader.Clear();
				res = true;
			}
			catch (std::exception& rExp)
			{
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				SvDef::StringVector msgList;
				msgList.push_back(rExp.what());
				Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
			break;
		case SvSml::ShareEvents::Ready:
		{
			try
			{
				DWORD version = SvSml::ShareEvents::GetInstance().GetReadyCounter();
				m_MemReader.Reload(version);
				m_pLastResponseData->ClearHeld();
				res = true;
			}
			catch (std::exception& rExp)
			{
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				SvDef::StringVector msgList;
				msgList.push_back(rExp.what());
				Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
			break;
		}
		default:
			break;
	}
	return res;
}
}