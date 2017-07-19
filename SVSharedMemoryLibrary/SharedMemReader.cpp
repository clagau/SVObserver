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
		m_DataContainer.CloseConnection();
		m_MLContainer.Clear();
	}

	void SharedMemReader::Reload( DWORD version)
	{
		m_MLContainer.ReloadMonitorMap(m_monitorListReader,version);
		m_DataContainer.CloseConnection();
		m_DataContainer.OpenSlotManagment(m_MLContainer);
		m_DataContainer.OpenStores(m_MLContainer);
		m_DataContainer.CreateSharedMatroxBuffer(m_MLContainer);
		m_SlotManagerIndexMap.clear();

		for (const auto &it : m_MLContainer.m_MonitorListCpyMap)
		{
			if (it.second.get() && it.second->GetIsActive())
			{
				int Slotmanagerindex = GetSlotManagerIndex(it.second->GetPPQname().c_str());
				if (Slotmanagerindex >= 0)
				{
					m_SlotManagerIndexMap[it.first] = Slotmanagerindex;
				}
			}
		}

	}
	DWORD SharedMemReader::GetVersion() const
	{
		return m_MLContainer.GetVersion();
	}

	bool SharedMemReader::IsActiveMonitorList(const SVString& Monitorlistname ) const
	{
		return m_MLContainer.IsActiveMonitorList(Monitorlistname);
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
	int SharedMemReader::GetSlotManagerIndex(LPCTSTR PPQname)
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
	
	SharedMemReader::retvalues  SharedMemReader::_GetProduct(LPCTSTR Monitorlist, int trigger,bool failstatus, bool reject,   MLProduct* pProduct, const MLProduct* pLastProduct, BOOL releaseSlot)
	{
		if (nullptr == pProduct)
			return fail;
		pProduct->ResetProduct();
		const MonitorListCpy*  pML = m_MLContainer.GetMonitorListCpyPointer(Monitorlist);
		if (nullptr == pML || FALSE == pML->GetIsActive())
		{
			return fail;
		}
		int SlotManagerIndex = GetSlotManagerIndexForMonitorList(Monitorlist);
		if (SlotManagerIndex < 0)
		{
			assert(FALSE);
			return fail;
		}
		RingBufferPointer rBp = GetSlotManager(SlotManagerIndex);
		if (FALSE == rBp.get())
		{
			assert(FALSE);
			return fail;
		}
		//get readerslot
		int slot(-1);
		if (trigger > -1)
		{
			slot = rBp->GetReaderSlotByTrigger(trigger);
		}
		else if (reject)
		{
			slot = rBp->GetReaderSlotLastWritten(SMRingBuffer::Reject);
		}
		else
		{
			slot = rBp->GetReaderSlotLastWritten(SMRingBuffer::Last);
		}
		
		;
		if(slot < 0)
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
		 
		ListType::typ t = failstatus ? ListType::failStatus : ListType::productItemsData;
		const MonitorEntries& ProdEnties = pML->GetMonitorEntries(t);
		for (MonitorEntryPointer mep : ProdEnties)
		{
			SVString value;
			int Offset = mep->data.Store_Offset;
			int store = mep->data.InspectionStoreId;
			BYTE* ptr = 	m_DataContainer.GetDataBufferPtr(slot, store, Offset);
			if (mep->data.wholeArray)
			{
				//@Todo[MEC][7.50] [14.07.2017] Implement Whole Array;
				value = "???";
			}
			else
			{
				mep->GetValue(value, ptr);
			}
			pProduct->m_data.push_back(stringpointer(new  SVString(value)));
			pProduct->m_dataEntries.push_back(mep);
		}
		const MonitorEntries& ProdImageEntries = pML->GetMonitorEntries(ListType::productItemsImage);
		if (!failstatus)
		{
			for (MonitorEntryPointer mep : ProdImageEntries)
			{
				pProduct->m_ImageEntries.push_back(mep);
			}
		}
		if (releaseSlot)
		{
			rBp->ReleaseReaderSlot(slot);
		}
		return sucess;

	}
	BOOL SharedMemReader::GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, BOOL releaseSlot)
	{
		return (sucess == _GetProduct(Monitorlist, TriggerNumber, false, true, pProduct,nullptr, releaseSlot));
	}
	
	BOOL  SharedMemReader::GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, BOOL releaseSlot)
	{
		return (sucess == _GetProduct(Monitorlist, TriggerNumber, false, false, pProduct, nullptr, releaseSlot));
	}


	BOOL SharedMemReader::GetFailStatusData(LPCTSTR Monitorlist, int  TriggerNumber, MLProduct* pProduct)
	{
		return (sucess == _GetProduct(Monitorlist, TriggerNumber, true, false, pProduct, nullptr, true));
	}


	SVMatroxBuffer& SharedMemReader::GetImageBuffer(DWORD  SlotIndex, DWORD storeIndex, DWORD ImageIndex)
	{
		
		return m_DataContainer.GetImageBuffer(SlotIndex, storeIndex,ImageIndex);
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
	
	BOOL  SharedMemReader::GetFailstatus(LPCTSTR Monitorlist, productPointerVector* pFailstatus)
	{
		return(sucess == GetFailstatus(Monitorlist, pFailstatus, nullptr));
		
	}
	SharedMemReader::retvalues SharedMemReader::GetFailstatus(LPCTSTR Monitorlist, productPointerVector* pFailstatus, productPointerVector*  pLastFailstatus)
	{

		int SlotManagerIndex = GetSlotManagerIndexForMonitorList(Monitorlist);
		if (nullptr == pFailstatus || SlotManagerIndex < 0)
			return fail;

		std::vector<DWORD> rejectsTrigger;
		m_DataContainer.GetSlotManager(SlotManagerIndex)->GetRejects(rejectsTrigger);
		int max = m_DataContainer.GetSlotManager(SlotManagerIndex)->GetRejectSlotCount();
		max -= MLPPQInfo::NumRejectSizeDelta;
		max = std::min(max, (int)rejectsTrigger.size());
		pFailstatus->clear();
		if (nullptr != pLastFailstatus)
		{
			if (pLastFailstatus->size() == max)
			{
				bool equal(true);
				for (int i = 0; i < max; i++)
				{	
					if ( (*pLastFailstatus)[i]->m_trigger != rejectsTrigger[i])
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
		for (int i = 0; i < max; i++)
		{
			productPointer productPtr = productPointer(new MLProduct);
			if (GetFailStatusData(Monitorlist, rejectsTrigger[i], productPtr.get()))
				pFailstatus->push_back(std::move(productPtr));
			else
			{
				productPtr.release();
			}
		}
		return sucess;
	}

	SharedMemReader::retvalues SharedMemReader::GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, BOOL releaseSlot)
	{
		return _GetProduct(Monitorlist, TriggerNumber, false, true, pProduct, pLastProduct, releaseSlot);
	}
	SharedMemReader::retvalues SharedMemReader::GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, BOOL  releaseSlot)
	{
		return _GetProduct(Monitorlist, TriggerNumber, false, false, pProduct, pLastProduct, releaseSlot);
	}

	

} //namespace SvSml
