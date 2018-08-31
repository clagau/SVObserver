//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemReader.cpp
/// All Rights Reserved 
//*****************************************************************************
///// Class encapsulate function to open  and read from the  shared Memory
//******************************************************************************
#include "StdAfx.h"
#include "SharedMemReader.h"
#include "ShareEvents.h"

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
	std::lock_guard<std::mutex> guard(m_ProtectReader);
	m_DataContainer.CloseConnection();
	m_MonitorListStore.CloseConnection();
	m_MLContainer.Clear();
}

void SharedMemReader::Reload(DWORD version)
{
	std::lock_guard<std::mutex> guard(m_ProtectReader);
	m_MonitorListStore.OpenMonitorStore("Monitor");
	DWORD size = m_MonitorListStore.GetSize();
	SvPml::MesMLCpyContainer MesMLCpyCont;
	MesMLCpyCont.ParseFromArray(m_MonitorListStore.GetPtr(), size);
	m_MLContainer.BuildFromProtoMessage(MesMLCpyCont);

	m_DataContainer.CloseConnection();
	m_DataContainer.OpenSlotManagment(m_MLContainer);
	m_DataContainer.OpenStores(m_MLContainer);
	m_DataContainer.CreateSharedMatroxBuffer(m_MLContainer);
	m_SlotManagerIndexMap.clear();

	for (const auto &it : m_MLContainer.m_MonitorListCpyMap)
	{
		if (it.second.get() && it.second->GetIsActive())
		{
			int Slotmanagerindex = GetSlotManagerIndexForPPQName(it.second->GetPPQname().c_str());
			if (Slotmanagerindex >= 0)
			{
				m_SlotManagerIndexMap[it.first] = Slotmanagerindex;
			}
		}
	}

}


RingBufferPointer SharedMemReader::GetSlotManager(int index)
{
	if (index >= 0 && index < m_MLContainer.m_PPQInfoMap.size())
	{
		return m_DataContainer.GetSlotManager(index);
	}
	else
	{
		return RingBufferPointer(nullptr);
	}
}
int SharedMemReader::GetSlotManagerIndexForPPQName(LPCTSTR PPQname)
{
	auto& it = m_MLContainer.m_PPQInfoMap.find(PPQname);
	if (it != m_MLContainer.m_PPQInfoMap.end() && it->second.get())
	{
		return it->second->SlotManagerIndex;
	}
	else
	{
		return -1;
	}
}

SharedMemReader::retvalues  SharedMemReader::_GetProduct(const GetProdPar& par, LPCTSTR Monitorlist, int trigger, MLProduct* pProduct, const MLProduct* pLastProduct)
{
	std::lock_guard<std::mutex> guard(m_ProtectReader);
	if (nullptr == pProduct)
	{
		return fail;
	}
	pProduct->ResetProduct();
	const MonitorListCpy*  pML = m_MLContainer.GetMonitorListCpyPointer(Monitorlist);
	if (nullptr == pML || FALSE == pML->GetIsActive())
	{
		return fail;
	}
	int SlotManagerIndex = GetSlotManagerIndexForMonitorList(Monitorlist);
	if (SlotManagerIndex < 0)
	{
		assert(false);
		return fail;
	}
	RingBufferPointer rBp = GetSlotManager(SlotManagerIndex);
	if (FALSE == rBp.get())
	{
		assert(false);
		return fail;
	}
	//get readerslot
	int slot(-1);
	if (-1 < trigger)
	{
		slot = rBp->GetReaderSlotByTrigger(trigger);
	}
	else if (par.reject)
	{
		slot = rBp->GetReaderSlotLastWritten(SMRingBuffer::Reject);
	}
	else
	{
		slot = rBp->GetReaderSlotLastWritten(SMRingBuffer::Last);
	}
	if (0 > slot)
	{
		return fail;
	}
	if (nullptr != pLastProduct)
	{
		if (rBp->GetTriggerNumber(slot) == pLastProduct->m_trigger && pLastProduct->m_status == S_OK)
		{
			/// the last Product holds the information we can return here;
			rBp->ReleaseReaderSlot(slot);
			return last;
		}
	}

	pProduct->m_slot = slot;
	pProduct->m_SlotManagerIndex = SlotManagerIndex;
	pProduct->m_trigger = rBp->GetTriggerNumber(slot);
	pProduct->m_status = S_OK;

	ListType::typ t = par.failstatus ? ListType::failStatus : ListType::productItemsData;
	const MonitorEntries& ProdEnties = pML->GetMonitorEntries(t);
	for (MonitorEntryPointer mep : ProdEnties)
	{
		std::string value;
		int Offset = mep->data.Store_Offset;
		int store = mep->data.InspectionStoreId;
		BYTE* ptr = m_DataContainer.GetDataBufferPtr(slot, store, Offset);
		if (mep->data.wholeArray)
		{
			//@Todo[MEC][7.50] [14.07.2017] Implement Whole Array;
			value = "???";
		}
		else
		{
			mep->GetValue(value, ptr);
		}
		pProduct->m_data.push_back(stringpointer(new  std::string(value)));
		pProduct->m_dataEntries.push_back(mep);
	}
	const MonitorEntries& ProdImageEntries = pML->GetMonitorEntries(ListType::productItemsImage);
	if (!par.failstatus)
	{
		for (MonitorEntryPointer mep : ProdImageEntries)
		{
			pProduct->m_ImageEntries.push_back(mep);
		}
	}
	if (par.releaseTrigger)
	{
		rBp->ReleaseReaderSlot(slot);
	}
	return success;

}



bool SharedMemReader::GetFailStatusData(LPCTSTR Monitorlist, int  TriggerNumber, MLProduct* pProduct)
{
	GetProdPar par;
	par.reject = false;
	par.releaseTrigger = true;
	par.failstatus = true;
	return (success == _GetProduct(par, Monitorlist, TriggerNumber, pProduct, nullptr));
}

SVMatroxBuffer& SharedMemReader::GetImageBuffer(DWORD  SlotIndex, DWORD storeIndex, DWORD ImageIndex)
{

	return m_DataContainer.GetImageBuffer(SlotIndex, storeIndex, ImageIndex);
}

int SharedMemReader::GetSlotManagerIndexForMonitorList(LPCTSTR Monitorlist)
{
	auto it = m_SlotManagerIndexMap.find(Monitorlist);
	if (it == m_SlotManagerIndexMap.end())
	{
		return -1;
	}
	else
	{
		return it->second;
	}
}

SharedMemReader::retvalues SharedMemReader::GetFailstatus(LPCTSTR Monitorlist, vecpProd* pFailstatus, vecpProd*  pLastFailstatus)
{
	int SlotManagerIndex = GetSlotManagerIndexForMonitorList(Monitorlist);
	if (nullptr == pFailstatus || SlotManagerIndex < 0)
	{
		return fail;
	}
	std::vector<DWORD> rejectsTrigger;
	m_DataContainer.GetSlotManager(SlotManagerIndex)->GetRejects(rejectsTrigger);
	int max = m_DataContainer.GetSlotManager(SlotManagerIndex)->GetRejectSlotCount();
	max -= (MLPPQInfo::NumRejectSizeDelta);
	max = std::min(max, static_cast<int>(rejectsTrigger.size()));
	pFailstatus->clear();
	if (nullptr != pLastFailstatus)
	{
		if (pLastFailstatus->size() == max)
		{
			bool equal(true);
			for (int i = 0; i < max; i++)
			{
				if ((*pLastFailstatus)[i]->m_trigger != rejectsTrigger[i])
				{
					equal = false;
					break;
				}
			}

			if (equal)
			{
				return last;
			}
		}


	}
	for (int i = max - 1; i >= 0; i--)
	{
		pProd productPtr = pProd(new MLProduct);
		if (GetFailStatusData(Monitorlist, rejectsTrigger[i], productPtr.get()))
		{
			pFailstatus->push_back(std::move(productPtr));
		}
		else
		{
			productPtr.reset();
		}
	}
	return success;
}

SharedMemReader::retvalues SharedMemReader::GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, bool releaseSlot)
{
	GetProdPar par;
	par.failstatus = false;
	par.reject = true;
	par.releaseTrigger = releaseSlot;
	return _GetProduct(par, Monitorlist, TriggerNumber, pProduct, pLastProduct);
}

SharedMemReader::retvalues SharedMemReader::GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, bool  releaseSlot)
{
	GetProdPar par;
	par.failstatus = false;
	par.reject = false;
	par.releaseTrigger = releaseSlot;
	return _GetProduct(par, Monitorlist, TriggerNumber, pProduct, pLastProduct);
}

} //namespace SvSml
