//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedDataContainer.cpp
/// All Rights Reserved 
//*****************************************************************************
///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory
///Data store and slot management  
//******************************************************************************
#include "Stdafx.h"

#include "SharedDataContainer.h"
#include "SMRingbuffer.h"

namespace SvSml
{
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
			DWORD index = PPQinfo.second->SlotManagerIndex;
			m_ManagmentStore[index] = RingBufferPointer(new SMRingBuffer);
			m_ManagmentStore[index]->OpenConnection(PPQinfo.first.c_str());
		}
		return static_cast<int>(m_ManagmentStore.size());
	}

	RingBufferPointer SharedDataContainer::GetSlotManager(int Slotmanagerindex)
	{
		return m_ManagmentStore[Slotmanagerindex];
	}

	
	void SharedDataContainer::CloseConnection()
	{
		for (auto& isE : m_ManagmentStore)
		{
			if (isE.get())
			{
				isE->CloseConnection();
			}
		}
		m_ManagmentStore.clear();
	}
}