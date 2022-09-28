//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SharedMemReader.cpp
/// All Rights Reserved 
//*****************************************************************************
///// Class encapsulate function to open  and read from the  shared Memory
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SharedMemReader.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVLogLibrary/Logging.h"
#pragma endregion Includes

//#define TRACE_TRC

namespace
{
void fillDataFromTR(const SvSml::MonitorEntries& rProdEnties, SvSml::MLProduct& rProduct)
{
	for (SvSml::MonitorEntryPointer mep : rProdEnties)
	{
		auto pTr = rProduct.m_triggerRecordMap[mep->data.m_inspectionStoreId];
		variant_t valueV = (nullptr != pTr) ? pTr->getDataValue(mep->data.m_triggerRecordPos) : variant_t();
		bool isArray = (VT_ARRAY == (valueV.vt & VT_ARRAY));
		if (isArray && mep->data.arrayIndex >= 0)
		{
			std::unique_ptr<_variant_t> TempVariant(new _variant_t);
			SvUl::getSingleVariantFromArrayOneDim<_variant_t>(valueV, mep->data.arrayIndex, *TempVariant.get());
			rProduct.m_dataV.push_back(std::move(TempVariant));
			rProduct.m_dataEntries.push_back(mep);
		}
		else
		{
			rProduct.m_dataV.push_back(std::make_unique<_variant_t>(valueV));
			rProduct.m_dataEntries.push_back(mep);
		}
	}
}
}

namespace SvSml
{
SharedMemReader::SharedMemReader(void)
{
}

SharedMemReader::~SharedMemReader(void)
{
}

void SharedMemReader::Clear(void)
{
	m_MonitorListStore.CloseConnection();
	m_MLContainer.Clear();
}

void SharedMemReader::Reload(DWORD )
{
	
	m_MonitorListStore.OpenMonitorStore("Monitor");
	DWORD size = m_MonitorListStore.GetSize();
	SvPml::MesMLCpyContainer MesMLCpyCont;
	MesMLCpyCont.ParseFromArray(m_MonitorListStore.GetPtr(), size);
	m_MLContainer.BuildFromProtoMessage(MesMLCpyCont);
}

SharedMemReader::retvalues SharedMemReader::fillProductWithTR(int trigger, SvOi::ITriggerRecordControllerR& rTRC, const MonitorListCpy& rML, MLProduct& rProduct, bool firstAlreadySet)
{
	bool firstIp = true;
	for (const auto& ipPair : rML.m_InspectionIdsVector)
	{
		if (false == firstAlreadySet || false == firstIp)
		{
			rProduct.m_triggerRecordMap[ipPair.first] = rTRC.createTriggerRecordObjectPerTriggerCount(ipPair.second, trigger);
		}
		firstIp = false;

		if (nullptr == rProduct.m_triggerRecordMap[ipPair.first])
		{
			SV_LOG_GLOBAL(trace) << SvUl::Format(_T("fillProductWithTR: TRC is null; %d\n"), ipPair.first);
			return fail;
		}
	}
	rProduct.m_trigger = trigger;
	return success;
}

SharedMemReader::retvalues SharedMemReader::fillProductWithLastTR(SvOi::ITriggerRecordControllerR& rTRC, const MonitorListCpy& rML, MLProduct& rProduct)
{
	if (0 < rML.m_InspectionIdsVector.size())
	{
		for (const auto& ipPair : rML.m_InspectionIdsVector)
		{
			rProduct.m_triggerRecordMap[ipPair.first] = rTRC.createTriggerRecordObject(ipPair.second, rTRC.getLastTrId(ipPair.second));
			if (nullptr == rProduct.m_triggerRecordMap[ipPair.first])
			{
				SV_LOG_GLOBAL(trace) << SvUl::Format(_T("fillProductWithLastTR: TRC is null; %d\n"), ipPair.first);
				return fail;
			}
		}
		auto ipIter = rProduct.m_triggerRecordMap.begin();
		auto lowestTriggerCount = ipIter->second->getTriggerData().m_TriggerCount;
		bool allCountsEqual = true;
		for (++ipIter; rProduct.m_triggerRecordMap.end() != ipIter; ++ipIter)
		{
			if (ipIter->second->getTriggerData().m_TriggerCount != lowestTriggerCount)
			{
				allCountsEqual = false;
				lowestTriggerCount = std::min(lowestTriggerCount, ipIter->second->getTriggerData().m_TriggerCount);
			}
		}
		if (false == allCountsEqual)
		{
			for (auto& trPair : rProduct.m_triggerRecordMap)
			{
				if (trPair.second->getTriggerData().m_TriggerCount != lowestTriggerCount)
				{
					auto iter = std::find_if(rML.m_InspectionIdsVector.begin(), rML.m_InspectionIdsVector.end(), [trPair](const auto& rEntry) { return trPair.first == rEntry.first; });
					if (rML.m_InspectionIdsVector.end() != iter)
					{
						trPair.second = rTRC.createTriggerRecordObjectPerTriggerCount(iter->second, lowestTriggerCount);
						if (nullptr == trPair.second)
						{
							SV_LOG_GLOBAL(trace) << SvUl::Format(_T("fillProductWithLastTR: TRC is null; %d\n"), trPair.first);
							return fail;
						}
					}
					else
					{
						return fail;
					}
				}
			}
		}
		rProduct.m_trigger = lowestTriggerCount;
		return success;
	}
	else
	{
		return fail;
	}
}

SharedMemReader::retvalues  SharedMemReader::_GetProduct(const GetProdPar& par, LPCTSTR Monitorlist, int trigger, MLProduct* pProduct)
{
	if (nullptr == pProduct)
	{
		SV_LOG_GLOBAL(trace) << "no Productptr";
		return fail;
	}
	pProduct->ResetProduct();
	const MonitorListCpy*  pML = m_MLContainer.GetMonitorListCpyPointer(Monitorlist);
	if (nullptr == pML || FALSE == pML->GetIsActive())
	{
		SV_LOG_GLOBAL(trace) << "no MonitorlistMetadat or no active Monitorlist";
		return fail;
	}

	auto* pTRC = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTRC)
	{
		if (-1 < trigger)
		{
			auto ret = fillProductWithTR(trigger, *pTRC, *pML, *pProduct);
			if (success != ret)
			{
				return ret;
			}
		}
		else if (par.reject)
		{
			if (0 < pML->m_InspectionIdsVector.size())
			{
				auto ipPair = pML->m_InspectionIdsVector.begin();
				const auto& trList = pTRC->getTrsOfInterest(ipPair->second, 1);
				if (trList.size() && nullptr != trList[0])
				{
					pProduct->m_triggerRecordMap[ipPair->first] = trList[0];
					auto ret = fillProductWithTR(trList[0]->getTriggerData().m_TriggerCount, *pTRC, *pML, *pProduct, true);
					if (success != ret)
					{
						return ret;
					}
				}
				else
				{
					SV_LOG_GLOBAL(trace) << SvUl::Format(_T("_GetProduct: TRC is null; Reject\n"));
					return fail;
				}				
			}
		}
		else
		{
			auto ret = fillProductWithLastTR(*pTRC, *pML, *pProduct);
			if (success != ret)
			{
				return ret;
			}
		}
	}
	pProduct->m_status = S_OK;

	const MonitorEntries& ProdEnties = pML->GetMonitorEntries(ListType::productItemsData);
	fillDataFromTR(ProdEnties, *pProduct);

	const MonitorEntries& ProdImageEntries = pML->GetMonitorEntries(ListType::productItemsImage);
	for (MonitorEntryPointer mep : ProdImageEntries)
	{
		pProduct->m_ImageEntries.push_back(mep);
	}

	if (par.releaseTrigger)
	{
		pProduct->m_triggerRecordMap.clear();
	}
	return success;
}

SharedMemReader::retvalues SharedMemReader::fillTrcInFailstatusList(LPCTSTR Monitorlist, vecpProd& rFailstatus)
{
	const MonitorListCpy* pML = m_MLContainer.GetMonitorListCpyPointer(Monitorlist);
	if (nullptr == pML || FALSE == pML->GetIsActive())
	{
		SV_LOG_GLOBAL(trace) << "no MonitorlistMetadat or no active Monitorlist";
		return fail;
	}

	auto* pTRC = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr == pTRC)
	{
		SV_LOG_GLOBAL(trace) << "no TriggerRecordController";
		return fail;
	}

	if (0 < pML->m_InspectionIdsVector.size())
	{
		int rejectDepth = pTRC->getInspections().list(pML->m_InspectionIdsVector[0].second).numberrecordsofinterest();
		for (const auto& ipPair : pML->m_InspectionIdsVector)
		{
			auto tmpTrcs = pTRC->getTrsOfInterest(ipPair.second, rejectDepth);
			for (int i = 0; i < tmpTrcs.size(); ++i)
			{
				auto& pTr = tmpTrcs[tmpTrcs.size() - 1 - i];
				if (rFailstatus.size() <= i)
				{
					const auto& tmp = rFailstatus.emplace_back(std::make_unique<MLProduct>());
					if (nullptr != pTr)
					{
						tmp->m_trigger = pTr->getTriggerData().m_TriggerCount;
					}
					else
					{
						SV_LOG_GLOBAL(trace) << SvUl::Format(_T("fillTrcInFailstatusList: TRC is null; %d\n"), ipPair.first);
						return fail;
					}
				}
				rFailstatus[i]->m_triggerRecordMap[ipPair.first] = pTr;
			}
		}
	}

	return success;	
}

SVMatroxBuffer SharedMemReader::GetImageBuffer(int triggerRecordId, int inspectionId, int imageIndex)
{
	auto* pTRC = SvOi::getTriggerRecordControllerRInstance();
	auto pTr = (nullptr != pTRC) ? pTRC->createTriggerRecordObject(inspectionId, triggerRecordId) : nullptr;
	if (nullptr != pTr)
	{
		SvOi::ITRCImagePtr pImage = nullptr;
		if (imageIndex < MonitorEntryData::c_childFlagForTrPos)
		{
			pImage = pTr->getImage(imageIndex);
		}
		else
		{
			pImage = pTr->getChildImage(imageIndex&(~MonitorEntryData::c_childFlagForTrPos));
		}
		if (nullptr != pImage && nullptr != pImage->getHandle())
		{
			return pImage->getHandle()->GetBuffer();
		}
	}
	return {};
}

SharedMemReader::retvalues SharedMemReader::GetFailstatus(LPCTSTR Monitorlist, vecpProd* pFailstatus, vecpProd*  pLastFailstatus)
{
	if (nullptr == pFailstatus)
	{
		return fail;
	}

	pFailstatus->clear();
	fillTrcInFailstatusList(Monitorlist, *pFailstatus);
	if (nullptr != pLastFailstatus && pFailstatus->size() == pLastFailstatus->size())
	{
		bool equal(true);
		for (int i = 0; i < pFailstatus->size(); i++)
		{
			if ((*pLastFailstatus)[i]->m_trigger != (*pFailstatus)[i]->m_trigger)
			{
				equal = false;
				break;
			}
		}

		if (equal)
		{
			pFailstatus->clear();
			return last;
		}
	}

	const MonitorListCpy* pML = m_MLContainer.GetMonitorListCpyPointer(Monitorlist);
	if (nullptr == pML || FALSE == pML->GetIsActive())
	{
		SV_LOG_GLOBAL(trace) << "no MonitorlistMetadat or no active Monitorlist";
		return fail;
	}

	const MonitorEntries& ProdEnties = pML->GetMonitorEntries(ListType::failStatus);
	for(auto& pProduct : *pFailstatus)
	{
		if (nullptr != pProduct)
		{
			fillDataFromTR(ProdEnties, *pProduct);
			pProduct->m_triggerRecordMap.clear();
		}
	}

	return success;
}

SharedMemReader::retvalues SharedMemReader::GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, bool releaseSlot)
{
	GetProdPar par;
	par.reject = true;
	par.releaseTrigger = releaseSlot;
	return _GetProduct(par, Monitorlist, TriggerNumber, pProduct);
}

SharedMemReader::retvalues SharedMemReader::GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, bool  releaseSlot)
{
	GetProdPar par;
	par.reject = false;
	par.releaseTrigger = releaseSlot;
	return _GetProduct(par, Monitorlist, TriggerNumber, pProduct);
}

} //namespace SvSml
