//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareControl.cpp
/// All Rights Reserved 
//*****************************************************************************
/// ShareControl implements the connection to the shared memory 
//******************************************************************************
#include  "StdAfx.h"
#include "LastResponseData.h"
#include "ShareControl.h"
#include "ShareEvents.h"
#include "ShareControlSetting.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "Definitions\StringTypeDef.h"
#include "SVProtoBuf/Envelope.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVLogLibrary\Logging.h"
#include "SVStatusLibrary\MessageTextGenerator.h"

namespace SvSml
{

ShareControl::ShareControl(const ShareControlSettings& ControlParameter )
{
	
	if (ControlParameter.DelayBeforeClearShare > 0)
	{
		m_DelayBeforeClearShare = ControlParameter.DelayBeforeClearShare;
	}
	m_pLastResponseData = std::make_unique<LastResponseData>();
	std::function<bool(DWORD)> eventHandler = [this](DWORD event) { return EventHandler(event); };
	SvSml::ShareEvents::GetInstance().SetParameter(ControlParameter);
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(eventHandler);
	SvSml::ShareEvents::GetInstance().StartWatch();
}
ShareControl::~ShareControl()
{
	SvSml::ShareEvents::GetInstance().StopWatch();
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(nullptr);
}

bool  ShareControl::QueryListName(const SvPb::QueryListNameRequest& rRequest, SvPb::QueryListNameResponse& rResponse, SvPenv::Error& rError)
{
	return m_MemReader.QueryListName(rRequest, rResponse, rError);
};
bool  ShareControl::QueryListItem(const SvPb::QueryListItemRequest& rRequest, SvPb::QueryListItemResponse& rResponse, SvPenv::Error& rError)
{
	return m_MemReader.QueryListItem(rRequest, rResponse, rError);
}

bool ShareControl::GetProduct(const SvPb::GetProductRequest& rRequest, SvPb::GetProductResponse& rResponse, SvPenv::Error& rError)
{
	return GetProductItem(false, rRequest.triggercount(), rRequest.pevioustrigger(), SvUl::to_ansi(rRequest.listname()), rRequest.nameinresponse(), rResponse.mutable_productitem(), rError);
}

bool ShareControl::GetReject(const SvPb::GetRejectRequest& rRequest, SvPb::GetRejectResponse& rResponse, SvPenv::Error& rError)
{
	return GetProductItem(true, rRequest.triggercount(), rRequest.pevioustrigger(), SvUl::to_ansi(rRequest.listname()), rRequest.nameinresponse(), rResponse.mutable_productitem(), rError);
}

bool  ShareControl::GetFailstatus(const SvPb::GetFailStatusRequest& rRequest, SvPb::GetFailStatusResponse& rResponse, SvPenv::Error& rError)
{
	if (!isReady(rError))
	{
		return false;
	}

	std::string listName = SvUl::to_ansi(rRequest.listname());

	bool bValid(false);
	if (!listName.empty())
	{
		bValid = m_MemReader.IsActiveMonitorList(listName);
	}
	if (!bValid)
	{
		SvPenv::Error err;
		err.set_errorcode(SvPenv::ErrorCode::badRequest);
		err.set_message("MonitorList with given name does not exist or is not active");
		return false;
	}
	SvSml::vecpProd* pLastFailstatus(nullptr);
	std::unique_ptr<SvSml::vecpProd>  new_FailstatusPtr(new SvSml::vecpProd);
	{
		std::lock_guard<std::mutex> guard(m_pLastResponseData->m_ProtectLastFailstatus);
		auto it = m_pLastResponseData->m_LastFailstatus.find(listName);
		if (it != m_pLastResponseData->m_LastFailstatus.end())
		{
			pLastFailstatus = it->second.get();
		}

		SvSml::SharedMemReader::retvalues ret =
			m_MemReader.GetFailstatus(listName.c_str(), new_FailstatusPtr.get(), pLastFailstatus);
		switch (ret)
		{
			case SvSml::SharedMemReader::success:
			{
				if (false == GetFailstatus(new_FailstatusPtr.get(), rRequest, rResponse, rError))
				{
					return false;
				}
				m_pLastResponseData->m_LastFailstatus[listName.c_str()] = std::move(new_FailstatusPtr);
			}
			break;
			case SvSml::SharedMemReader::last:
			{
				if (false == GetFailstatus(pLastFailstatus, rRequest, rResponse, rError))
				{
					return false;
				}
			}
			break;
			case SvSml::SharedMemReader::fail:
			{
				SvPenv::Error err;
				err.set_errorcode(SvPenv::ErrorCode::internalError);
				return false;
			}
			break;
		}
	}
	return true;
}

bool ShareControl::GetImageFromId(const  SvPb::GetImageFromIdRequest& req, SvPb::GetImageFromIdResponse& resp, SvPenv::Error& err)
{
	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
	if (!isready)
	{
		err.set_errorcode(SvPenv::ErrorCode::locked);
		err.set_message("Shared Memory is blocked");
		return false;
	}
	int inspectionId = req.id().inspectionid();
	int triggerRecordId = req.id().trid();
	int imageindex = req.id().imageindex();

	SVBitmapInfo bitmapInfo;
	auto pData = resp.mutable_imagedata();
	SVMatroxBuffer imageBuffer = m_MemReader.GetImageBuffer(triggerRecordId, inspectionId, imageindex);
	std::string ImageBuffer;
	DWORD res = SVMatroxBufferInterface::CopyBufferToFileDIB(ImageBuffer, bitmapInfo, imageBuffer);
	if (S_OK != res)
	{
		SvDef::StringVector msgList;
		err.set_errorcode(SvPenv::ErrorCode::internalError);
		err.set_message(SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_ShareControl_NoImageForId, msgList).c_str());
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVGateway_0_GENERAL_ERROR, SvStl::Tid_ShareControl_NoImageForId, msgList, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}
	pData->mutable_rgbdata()->swap(ImageBuffer);
	pData->set_width(std::abs(bitmapInfo.GetWidth()));
	pData->set_height(std::abs(bitmapInfo.GetHeight()));
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
				Sleep(m_DelayBeforeClearShare);
				m_MemReader.Clear();
				res = true;
			}
			catch (std::exception& rExp)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				SvDef::StringVector msgList;
				msgList.push_back(rExp.what());
				Exception.setMessage(SVMSG_SVGateway_0_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
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
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				SvDef::StringVector msgList;
				msgList.push_back(rExp.what());
				Exception.setMessage(SVMSG_SVGateway_0_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
			break;
		}
		default:
			break;
	}
	return res;
}

bool ShareControl::isReady(SvPenv::Error& rError)
{
	if (!SvSml::ShareEvents::GetInstance().GetIsReady())
	{
		rError.set_errorcode(SvPenv::ErrorCode::locked);
		rError.set_message("Shared Memory is blocked");
		return false;
	}
	return true;
}

bool ShareControl::isMonitorListActive(const std::string& rListName, SvPenv::Error& rError)
{
	bool Result{false};

	if (rListName.length() > 0)
	{
		Result = m_MemReader.IsActiveMonitorList(rListName);
	}

	if (!Result)
	{
		rError.set_errorcode(SvPenv::ErrorCode::badRequest);
		rError.set_message("MonitorList with given name does not exist or is not active");
	}
	return Result;
}

bool ShareControl::GetProductItem(bool isReject, int triggerCount, int peviousTrigger, const std::string& rListName, bool nameInResponse, SvPb::Product* pProductMsg, SvPenv::Error& rError)
{
	if (!isReady(rError))
	{
		SV_LOG_GLOBAL(trace) << "Shared Memory not ready";
		return false;
	}

	if (!isMonitorListActive(rListName, rError))
	{
		SV_LOG_GLOBAL(trace) << "MonitorList not active: " << rListName.c_str();
		return false;
	}

	const SvSml::MLProduct* pLastProduct {nullptr};
	SvSml::pProd new_productPtr(new SvSml::MLProduct());
	{
		std::mutex& rProtectLast = isReject ? m_pLastResponseData->m_ProtectLastReject : m_pLastResponseData->m_ProtectLastProduct;
		std::map<std::string, SvSml::pProd>& rLastMap = isReject ? m_pLastResponseData->m_LastReject : m_pLastResponseData->m_LastProduct;

		std::lock_guard<std::mutex> guard(rProtectLast);

		auto it = rLastMap.find(rListName);
		if (rLastMap.end() != it)
		{
			pLastProduct = it->second.get();
		}

		SvSml::SharedMemReader::retvalues ret(SvSml::SharedMemReader::fail);
		if(isReject)
		{
			ret = m_MemReader.GetRejectData(rListName.c_str(), triggerCount, new_productPtr.get(), pLastProduct, false);
		}
		else
		{
			ret = m_MemReader.GetProductData(rListName.c_str(), triggerCount, new_productPtr.get(), pLastProduct, false);
		}

		switch (ret)
		{
			case SvSml::SharedMemReader::success:
			{
				int index = m_MemReader.GetSlotManagerIndexForMonitorList(rListName.c_str());
				if (nullptr != pLastProduct)
				{
					///release Readerslot for last product
					if (index < 0)
					{
						SvStl::MessageManager exception(SvStl::MsgType::Log);
						SvDef::StringVector msgList;
						msgList.emplace_back(rListName);
						exception.setMessage(SVMSG_SVGateway_0_GENERAL_ERROR, SvStl::Tid_SM_SlotNotFound, msgList, SvStl::SourceFileParams(StdMessageParams));
				
						rError.set_errorcode(SvPenv::ErrorCode::internalError);
						rError.set_message(exception.getMessageContainer().what());
						return false;
					}
					m_MemReader.GetSlotManager(index)->ReleaseReaderSlot(pLastProduct->m_slot);
				}

				if (SetProductResponse(nameInResponse, new_productPtr.get(), pProductMsg, rError))
				{
					rLastMap[rListName].reset();
					rLastMap[rListName] = std::move(new_productPtr);
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
				if (peviousTrigger == pLastProduct->m_trigger)
				{
					pProductMsg->set_status(SvPb::State::unchanged);
				}
				else
				{
					if (!SetProductResponse(nameInResponse, pLastProduct, pProductMsg, rError))
					{
						return false;
					}
				}
				return true;
			}
			break;
			case SvSml::SharedMemReader::fail:
			{
				SV_LOG_GLOBAL(trace) << "GetProductItem fails";
				rError.set_errorcode(SvPenv::ErrorCode::notFound);
				return false;
			}
			break;
			default:
			{
				rError.set_errorcode(SvPenv::ErrorCode::internalError);
				return false;
			}
			break;
		}
	}
}

bool  ShareControl::SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, SvPb::Product* pProductMsg, SvPenv::Error& rError)
{
	if (nullptr == pProduct || nullptr == pProductMsg)
	{
		SV_LOG_GLOBAL(trace) << "Missing Pointer in SetProductResponse";
		rError.set_errorcode(SvPenv::ErrorCode::notFound);
		return false;
	}

	pProductMsg->set_trigger((int)pProduct->m_trigger);
	pProductMsg->set_status(SvPb::State::isValid);

	for (int i = 0; i < pProduct->m_dataV.size(); i++)
	{
		auto pValue = pProductMsg->add_values();
	
		SvPb::ConvertVariantToProtobuf(pProduct->m_dataV[i].get(), pValue);
	
		if (nameInResponse)
		{
			*pProductMsg->add_valuenames() = SvUl::to_utf8(pProduct->m_dataEntries[i]->name);
		}
	}

	int index(0);
	for (auto& MeP : pProduct->m_ImageEntries)
	{
		if (!MeP.get())
		{
			break;
		}
		auto pImage = pProductMsg->add_images();
		auto mapIter = pProduct->m_triggerRecordMap.find(MeP->data.m_inspectionStoreId);
		if (pProduct->m_triggerRecordMap.end() != mapIter && nullptr != mapIter->second)
		{
			pImage->set_trid(mapIter->second->getId());
		}
		pImage->set_imageindex(MeP->data.m_triggerRecordPos);
		pImage->set_inspectionid(MeP->data.m_inspectionTRCPos);

		if (nameInResponse)
		{
			*pProductMsg->add_imagenames() = SvUl::to_utf8(pProduct->m_ImageEntries[index++]->name);
		}
	}
	return true;
}

bool ShareControl::GetFailstatus(SvSml::vecpProd* pFailstatus, const SvPb::GetFailStatusRequest& rRequest, SvPb::GetFailStatusResponse& rResponse, SvPenv::Error& rError)
{
	if (nullptr == pFailstatus)
	{
		rError.set_errorcode(SvPenv::ErrorCode::internalError);
		return false;
	}

	for (SvSml::pProd& pProduct : *pFailstatus)
	{
		auto pProductMsg = rResponse.add_products();
		if (false == SetProductResponse(rRequest.nameinresponse(), pProduct.get(), pProductMsg, rError))
		{
			return false;
		}
	}
	return true;
}

} //namespace SvSml