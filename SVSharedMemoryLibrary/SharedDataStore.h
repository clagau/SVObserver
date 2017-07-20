//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedDataStore.h
/// All Rights Reserved 
//*****************************************************************************
///class for holding memory for the dataBufferes in SharedMemory
//******************************************************************************
#pragma once
#include "SVStatusLibrary\SourceFileParams.h"
namespace SvSml
{
	///class for holding memory for the dataBufferes in SharedMemory
	class SharedDataStore
	{

	public:
		
		struct DataStoreHeader
		{
			DWORD slotCount; //<number of slots 
			DWORD  slotSize; //<size per slot
			BOOL UseOneViewPerSlot;
		};
		
		//! criteria if the memory is mapped to one view or one view for each slot 
		// 0 means one view per slot in every case
		const static int GranularityFactor = 5; 
		const static int ConnectionTimeout = 2000;
		const static int  ConnectionRetryTime = 200;

		SharedDataStore();
		~SharedDataStore();
		void  CloseConnection(); //<close connection to datastore

		void CreateDataStore(LPCTSTR StoreName, DWORD  size, DWORD  slots); //<Destroy the shared memory if it exist
		bool OpenDataStore(LPCTSTR StoreName); //<Connect to existing store 
		///Get Ptr to data in shared memory
		BYTE* GetPtr(DWORD slot, DWORD offset); 
		///Get m_MapFileName 
		LPCTSTR GetMapFileName() const;
		///Get Slotcount
		DWORD GetSlotCount() const;
		///Get Slot size 
		DWORD GetSlotSize() const;
	
	
	
	private:
		//Not implemented don't  allow copyconstructor 
		SharedDataStore(const SharedDataStore& datastore);
		void BuildMapFileName();
		void ThrowCreateFileMappingFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode);
		void ThrowMapViewOfFileFailedFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode);
		void LogUnMapViewOfFileFailedFailed(SvStl::SourceFileParams& FileParams, DWORD Programmcode);
		void MapViewOfFileWriteInfos();

	private:		
		BOOL  m_bUseOneViewPerSlot; //<if this is false if the memory is mapped to one view else  one view for each slot is used
		DWORD m_slotCount; //<number of slots 
		DWORD  m_slotSize; //<size per slot
		DWORD m_DataStoreHeaderSize;
		SVString m_MapFileName;
		SVString m_StoreName;
		HANDLE m_hMapFileImage;

		void* m_pViewHeader;
		void* m_pViewData;
		std::vector<void*> m_pViewDataVector;
		DWORD m_AllocationGranularity;

	};
	typedef  std::unique_ptr<SharedDataStore> DataStorePointer;

} //namespace SvSml



