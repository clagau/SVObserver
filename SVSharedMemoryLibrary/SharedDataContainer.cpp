//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedDataContainer.cpp
/// All Rights Reserved 
//*****************************************************************************
///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory
///Data store and slot management  
//******************************************************************************
#include "Stdafx.h"
#include "SVUtilityLibrary\SVString.h"
#include "SharedDataContainer.h"
#include "SMRingbuffer.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"

namespace SvSml
{
	const LPCTSTR SharedDataContainer::ShareNames[] = { ".SlotManager",".Images", ".Data" };
	SharedDataContainer::SharedDataContainer(void)
	{}
	SharedDataContainer::~SharedDataContainer(void)
	{
		CloseConnection();
	}
	int SharedDataContainer::CreateSlotManagment( const MLCpyContainer&  rmlCont, const SMParameterStruct& rParam)
	{
		m_ManagmentStore.resize(rmlCont.m_PPQInfoMap.size());
		for (auto& PPQinfo : rmlCont.m_PPQInfoMap)
		{
			DWORD Reject = PPQinfo.second->NumRejectSlot;
			DWORD Total = PPQinfo.second->NumRejectSlot + PPQinfo.second->NumLastSlot;
			DWORD index = PPQinfo.second->SlotManagerIndex;
			m_ManagmentStore[index] = RingBufferPointer(new SMRingBuffer);
			m_ManagmentStore[index]->CreateConnection(PPQinfo.first.c_str(), Total, Reject, rParam);
		}
		return static_cast<int>(m_ManagmentStore.size());
	}

	int SharedDataContainer::OpenSlotManagment(const MLCpyContainer&  rmlCont)
	{
		m_ManagmentStore.resize(rmlCont.m_PPQInfoMap.size());
		for (auto& PPQinfo : rmlCont.m_PPQInfoMap)
		{
			DWORD Reject = PPQinfo.second->NumRejectSlot;
			DWORD Total = PPQinfo.second->NumRejectSlot + PPQinfo.second->NumLastSlot;
			DWORD index = PPQinfo.second->SlotManagerIndex;
			m_ManagmentStore[index] = RingBufferPointer(new SMRingBuffer);
			m_ManagmentStore[index]->OpenConnection(PPQinfo.first.c_str());
		}
		return static_cast<int>(m_ManagmentStore.size());
	}
	void SharedDataContainer::CloseSlotManagmentConnection()
	{
		m_ManagmentStore.clear();
	}

	int  SharedDataContainer::CreateStores(const MLCpyContainer&  rmlCont, const SMParameterStruct& rParam)
	{
		DWORD ImageStoresCount = static_cast<DWORD>(rmlCont.m_InspectionInfoMap.size());
		m_ImageStore.resize(ImageStoresCount);
		m_DataStore.resize(ImageStoresCount);;
		for (auto& Inspectioninfo : rmlCont.m_InspectionInfoMap)
		{
			DWORD index = Inspectioninfo.second->StoreIndex;
			DWORD Imagesize = Inspectioninfo.second->TotalImageSize;
			DWORD Datasize = Inspectioninfo.second->TotalDataSize;
			SVString StoreNameImage,StoreNameData;
			BuildStoreName(Inspectioninfo.first.c_str(), Image, StoreNameImage); 
			BuildStoreName(Inspectioninfo.first.c_str(), Data, StoreNameData);
			DWORD ppqIndex  = Inspectioninfo.second->PPQIndex;
			DWORD Slotsize = m_ManagmentStore[ppqIndex]->GetTotalSlotCount();
			m_ImageStore[index] = DataStorePointer(new SharedDataStore);
			m_ImageStore[index]->CreateDataStore(StoreNameImage.c_str(), Imagesize, Slotsize, rParam);

			m_DataStore[index] = DataStorePointer(new SharedDataStore);
			m_DataStore[index]->CreateDataStore(StoreNameData.c_str(), Datasize, Slotsize, rParam);
		}
		return ImageStoresCount;
	}

	int  SharedDataContainer::OpenStores( const MLCpyContainer&  rmlCont)
	{
		DWORD ImageStoresCount = static_cast<DWORD>(rmlCont.m_InspectionInfoMap.size());
		m_ImageStore.resize(ImageStoresCount);
		m_DataStore.resize(ImageStoresCount);;
		for (auto& Inspectioninfo : rmlCont.m_InspectionInfoMap)
		{
			DWORD index = Inspectioninfo.second->StoreIndex;
			//DWORD Imagesize = Inspectioninfo.second->TotalImageSize;
			//DWORD Datasize = Inspectioninfo.second->TotalDataSize;
			SVString StoreNameImage, StoreNameData;
			BuildStoreName(Inspectioninfo.first.c_str(), Image, StoreNameImage);
			BuildStoreName(Inspectioninfo.first.c_str(), Data, StoreNameData);
			DWORD ppqIndex = Inspectioninfo.second->PPQIndex;
			DWORD Slotsize = m_ManagmentStore[ppqIndex]->GetTotalSlotCount();
			m_ImageStore[index] = DataStorePointer(new SharedDataStore);
			m_ImageStore[index]->OpenDataStore(StoreNameImage.c_str());

			m_DataStore[index] = DataStorePointer(new SharedDataStore);
			m_DataStore[index]->OpenDataStore(StoreNameData.c_str());
		}
		return ImageStoresCount;
	}
	
	void SharedDataContainer::CloseStoreConnection()
	{
		m_DataStore.clear();
		m_ImageStore.clear();

	}

	BYTE*  SharedDataContainer::GetImageBufferPtr(DWORD  SlotIndex, DWORD storeIndex, DWORD storeoffset)
	{
			if (storeIndex  <= m_ImageStore.size() && m_ImageStore[storeIndex].get())
			{
				return m_ImageStore[storeIndex]->GetPtr(SlotIndex, storeoffset);
			}
			return nullptr;
	}
	BYTE*  SharedDataContainer::GetDataBufferPtr(DWORD  SlotIndex, DWORD storeIndex, DWORD storeoffset)
	{
		if (storeIndex <= m_DataStore.size() && m_DataStore[storeIndex].get())
		{
			return m_DataStore[storeIndex]->GetPtr(SlotIndex, storeoffset);
		}
		return nullptr;
	}
	void SharedDataContainer::CreateSharedMatroxBuffer(const MLCpyContainer&  rmlCont)
	{
		m_ImageBufferStore.resize(m_ImageStore.size());
		MLInspectionInfoMap::const_iterator infoMapIt = rmlCont.m_InspectionInfoMap.begin();
		for (infoMapIt = rmlCont.m_InspectionInfoMap.begin(); infoMapIt != rmlCont.m_InspectionInfoMap.end(); ++infoMapIt)
		{
			if (nullptr == infoMapIt->second.get())
				continue;
			DWORD index = infoMapIt->second->StoreIndex;
			DWORD imageCount = infoMapIt->second->TotalImageCount;
			int Slotcount = GetImageSlotCount(index);
			
			if (-1 == index || Slotcount <0)
			{
				continue;
			}
			m_ImageBufferStore[index] = UP_MatroxBuffer2D(new MatroxBuffer2D);
			m_ImageBufferStore[index]->SetDim(Slotcount, imageCount);
		}
		MonitorListCpyMap::const_iterator it = rmlCont.m_MonitorListCpyMap.begin();
		//loop over Monitorlist 
		for (; it != rmlCont.m_MonitorListCpyMap.end(); ++it)
		{
			const MonitorEntries& monEn = it->second->GetMonitorEntries(ListType::productItemsImage);
			MonitorEntries::const_iterator  monEnIt = monEn.begin();
			for (; monEnIt != monEn.end(); ++monEnIt)
			{
				DWORD ItemId = monEnIt->get()->data.ItemId;
				DWORD StoreId = monEnIt->get()->data.InspectionStoreId;
				DWORD StoreOffset = monEnIt->get()->data.Store_Offset;
				int Slotcount = GetImageSlotCount(StoreId);

				MatroxImageProps ImageProps;
				monEnIt->get()->GetMatroxImageProps(ImageProps);
				for (int Slotindex = 0; Slotindex < Slotcount; Slotindex++)
				{
					BYTE* ptr = GetImageBufferPtr(Slotindex, StoreId, StoreOffset);
					SVMatroxBuffer& MBuffer = m_ImageBufferStore[StoreId]->GetAt(Slotindex, ItemId);
					HRESULT code;
					code = SVMatroxBufferInterface::CreateBuffer(MBuffer, ImageProps, ptr);
				}
				
			}
		}
	}


	SVMatroxBuffer& SharedDataContainer::GetImageBuffer(DWORD  SlotIndex, DWORD storeIndex, DWORD ImageIndex)
	{
		return m_ImageBufferStore[storeIndex]->GetAt(SlotIndex, ImageIndex);
	}

	///return the number of ImageSlot in the Image Store 
	int SharedDataContainer::GetImageSlotCount(DWORD ImageStoreIndex)
	{
		int ret(-1);
		if (ImageStoreIndex < m_ImageStore.size())
		{
			if (m_ImageStore[ImageStoreIndex].get())
			{
				ret = m_ImageStore[ImageStoreIndex]->GetSlotCount();
			}
		}

		return ret;
	}

	RingBufferPointer SharedDataContainer::GetSlotManager(int Slotmanagerindex)
	{
		return m_ManagmentStore[Slotmanagerindex];
	}

	
	void SharedDataContainer::CloseConnection()
	{
		for (auto& isE : m_ImageBufferStore)
		{
			if (isE.get())
			{
				isE->Clear();
			}
		}
		for (auto& isE : m_ImageStore)
		{
			if (isE.get())
			{
				isE->CloseConnection();
			}
			
		}
		for (auto& isE : m_DataStore)
		{
			if (isE.get())
			{
				isE->CloseConnection();
			}
			
		}
		for (auto& isE : m_ManagmentStore)
		{
			if (isE.get())
			{
				isE->CloseConnection();
			}
		}
		m_ImageStore.clear();
		m_ImageBufferStore.clear();
		m_DataStore.clear();
		m_ManagmentStore.clear();
	}

	void SharedDataContainer::BuildStoreName(LPCTSTR ObjectName, StoreType type, SVString& rStoreName)
	{
		rStoreName = ObjectName;
		rStoreName += ShareNames[type];
	}
}