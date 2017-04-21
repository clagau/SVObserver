#include "StdAfx.h"
#include "SharedImageContainer.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"

namespace Seidenader { namespace SVSharedMemoryLibrary
{


SharedImageContainer::SharedImageContainer(void)
{
}


SharedImageContainer::~SharedImageContainer(void)
{
}

int SharedImageContainer::CreateImageStores(const SVString& InspName, long ProductSlots, long RejectSlots, const MLCpyContainer&  rmlCont )
{
	MLInspectionInfoMap::const_iterator infoMapIt = rmlCont.m_InspectionInfoMap.find(InspName);
	if(infoMapIt == rmlCont.m_InspectionInfoMap.end())
	{
		return 0; // No Monitorlistentries for this inspection 
	}

	DWORD ImageStoresCount = 	static_cast<DWORD>(rmlCont.m_InspectionInfoMap.size());
	m_ProductImageStores.resize(ImageStoresCount);
	m_RejectImageStores.resize(ImageStoresCount);


	DWORD index = infoMapIt->second->StoreIndex;
	DWORD Imagesize = infoMapIt->second->TotalImageSize;

	if(Imagesize >0 )
	{
		m_ProductImageStores[index] = ImageStorePointer(new SharedImageStore );
		m_ProductImageStores[index]->CreateImageStore(InspName,SharedImageStore::last ,Imagesize,ProductSlots);
		m_RejectImageStores[index] = (ImageStorePointer(new SharedImageStore ));
		m_RejectImageStores[index]->CreateImageStore(InspName,SharedImageStore::reject ,Imagesize,RejectSlots);
	}
	return static_cast<int>( m_ProductImageStores.size());
}


int SharedImageContainer::OpenImageStores( const MLCpyContainer&  rmlCont )
{
	
	DWORD ImageStoresCount = 	static_cast<DWORD>(rmlCont.m_InspectionInfoMap.size());
	m_ProductImageStores.resize(ImageStoresCount);
	m_RejectImageStores.resize(ImageStoresCount);

	MLInspectionInfoMap::const_iterator infoMapIt = rmlCont.m_InspectionInfoMap.begin();
	for(;infoMapIt != rmlCont.m_InspectionInfoMap.end(); ++infoMapIt )
	{

		DWORD index = infoMapIt->second->StoreIndex;
		DWORD Imagesize = infoMapIt->second->TotalImageSize;
		if(Imagesize >0 )
		{
			m_ProductImageStores[index] = ImageStorePointer(new SharedImageStore );
			m_ProductImageStores[index]->OpenImageStore(infoMapIt->first,SharedImageStore::last);
			m_RejectImageStores[index] = (ImageStorePointer(new SharedImageStore ));
			m_RejectImageStores[index]->OpenImageStore(infoMapIt->first, SharedImageStore::reject);
		}
	}
	return static_cast<int>( m_ProductImageStores.size());
}


void SharedImageContainer::Clear()
{
	m_ProductImageBuffer.clear();
	m_RejectImageBuffer.clear();
	m_ProductImageStores.clear();
	m_RejectImageStores.clear();
}


void SharedImageContainer::DestroySharedMemory()
{
	m_ProductImageBuffer.clear();
	m_RejectImageBuffer.clear();
	std::vector<ImageStorePointer>::iterator it;
	for( it = m_ProductImageStores.begin(); it != m_ProductImageStores.end(); ++it)
	{
		if(it->get())
		{
			it->get()->Destroy();
			it->release();
		}
	}
	for( it = m_RejectImageStores.begin(); it != m_RejectImageStores.end(); ++it)
	{
		if(it->get())
		{
			it->get()->Destroy();
			it->release();
		}
	}
	m_ProductImageStores.clear();
	m_RejectImageStores.clear();
}

BYTE*  SharedImageContainer::GetImageBufferPtr(DWORD  SlotIndex, SharedImageStore::StoreType Store, DWORD storeIndex , DWORD storeoffset)
{
	switch (Store)
	{
	case SharedImageStore::last:
		if(storeIndex <= m_ProductImageStores.size() && m_ProductImageStores[storeIndex].get())
		{
			return m_ProductImageStores[storeIndex]->GetPtr(SlotIndex,storeoffset);
		}
		break;
	case SharedImageStore::reject:
		if(storeIndex <= m_RejectImageStores.size() && m_ProductImageStores[storeIndex].get())
		{
			return m_RejectImageStores[storeIndex]->GetPtr(SlotIndex,storeoffset);
		}
		break;
	}

	return nullptr;
}


///Return the MatroxSharedBuffer;
SVMatroxBuffer& SharedImageContainer::GetImageBuffer(DWORD  SlotIndex, SharedImageStore::StoreType t, DWORD storeIndex, DWORD ImageIndex)
{
	
	if( t == SharedImageStore::reject)
	{
		return m_RejectImageBuffer[storeIndex]->GetAt(SlotIndex,ImageIndex);
	}
	else
	{
		return m_ProductImageBuffer[storeIndex]->GetAt(SlotIndex,ImageIndex);
	}
}

///Creates a MatroxSharedBuffer for all images in the Monitorlist;
void SharedImageContainer::CreateSharedMatroxBuffer( const MLCpyContainer&  rmlCont )
{
	m_ProductImageBuffer.resize(m_ProductImageStores.size());
	m_RejectImageBuffer.resize(m_ProductImageStores.size());

	MLInspectionInfoMap::const_iterator infoMapIt = rmlCont.m_InspectionInfoMap.begin();
	for(infoMapIt = rmlCont.m_InspectionInfoMap.begin(); infoMapIt != rmlCont.m_InspectionInfoMap.end();++infoMapIt)
	{
		if (nullptr == infoMapIt->second.get())
			continue;
		DWORD index = infoMapIt->second->StoreIndex;
		DWORD imageCount = infoMapIt->second->TotalImageCount;
		DWORD Slotcount = GetProductSlotCount(index);
		DWORD Rejectcount = GetRejectSlotCount(index);

		
		
		m_RejectImageBuffer[index] = UP_MatroxBuffer2D(new MatroxBuffer2D);
		m_ProductImageBuffer[index] = UP_MatroxBuffer2D(new MatroxBuffer2D);
		m_ProductImageBuffer[index]->SetDim(Slotcount,imageCount);
		m_RejectImageBuffer[index]->SetDim(Rejectcount,imageCount);
	}

	///Loop over all Buffer

	MonitorListCpyMap::const_iterator it =  rmlCont.m_MonitorListCpyMap.begin();
	//LOOP over Monitorlist 
	for(; it != rmlCont.m_MonitorListCpyMap.end(); ++it)
	{
		const MonitorEntries& monEn	= it->second->GetMonitorEntries(ListType::productItemsImage);
		MonitorEntries::const_iterator  monEnIt =  monEn.begin();
		for(;  monEnIt != monEn.end(); ++monEnIt)
		{
			DWORD ItemId = monEnIt->get()->ItemId;
			DWORD StoreId = monEnIt->get()->InspectionStoreId;
			DWORD StoreOffset = monEnIt->get()->Store_Offset;
			DWORD Slotcount = GetProductSlotCount(StoreId);
			DWORD Rejectcount = GetRejectSlotCount(StoreId);
			SharedImageStore::StoreType Last = SharedImageStore::last;
			SharedImageStore::StoreType Reject = SharedImageStore::reject;

			
			MatroxImageProps ImageProps ; 
			monEnIt->get()->GetMatroxImageProps(ImageProps);
			for(DWORD Slotindex = 0;Slotindex < Slotcount; Slotindex++ )
			{
				BYTE* ptr =  GetImageBufferPtr( Slotindex, Last, StoreId ,StoreOffset); 
				SVMatroxBuffer& MBuffer = m_ProductImageBuffer[StoreId]->GetAt(Slotindex ,ItemId);
				SVMatroxBufferInterface::SVStatusCode code; 
				code = 	SVMatroxBufferInterface::CreateBuffer(MBuffer,ImageProps, ptr);
			}
			for(DWORD RejectSlot = 0;RejectSlot < Rejectcount; RejectSlot++ )
			{
				BYTE* ptr =  GetImageBufferPtr( RejectSlot, Reject, StoreId ,StoreOffset); 
				SVMatroxBuffer& MBuffer = m_RejectImageBuffer[StoreId]->GetAt(RejectSlot ,ItemId);
				SVMatroxBufferInterface::SVStatusCode code; 
				code = 	SVMatroxBufferInterface::CreateBuffer(MBuffer,ImageProps, ptr);
			}
		}
	}


}


DWORD SharedImageContainer::GetProductSlotCount(DWORD StoreIndex)
{
	DWORD Slotcount(0);
	if ( m_ProductImageStores.size() > StoreIndex &&  nullptr != m_ProductImageStores[StoreIndex].get())
	{
		Slotcount = m_ProductImageStores[StoreIndex]->GetSlotCount();
	}
	return Slotcount;


}
DWORD SharedImageContainer::GetRejectSlotCount(DWORD StoreIndex)
{
	DWORD Slotcount(0);
	if (m_RejectImageStores.size() > StoreIndex &&  nullptr != m_RejectImageStores[StoreIndex].get())
	{
		Slotcount = m_RejectImageStores[StoreIndex]->GetSlotCount();
	}
	return Slotcount;


}



}}