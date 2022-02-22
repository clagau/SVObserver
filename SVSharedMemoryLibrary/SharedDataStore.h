//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SharedDataStore.h
/// All Rights Reserved 
//*****************************************************************************
///class for holding memory for the dataBufferes in SharedMemory
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvSml
{
struct SMParameterStruct;

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
	SharedDataStore();
	~SharedDataStore();
	///close connection to datastore
	void  CloseConnection();
	///Destroy the shared memory if it exist
	void CreateDataStore(LPCTSTR StoreName, DWORD  size, DWORD  slots, const SMParameterStruct& rParam);
	bool OpenDataStore(LPCTSTR StoreName); //<Connect to existing store 
	///Get Ptr to data in shared memory
	BYTE* GetPtr(DWORD slot, DWORD offset);
	///Get m_MapFileName 
	LPCTSTR GetMapFileName() const;
	///Get Slot size 
	DWORD GetSlotSize() const;



private:
	SharedDataStore(const SharedDataStore& datastore) = delete;
	void BuildMapFileName();
	void MapViewOfFileWriteInfos();

private:

	bool  m_bUseOneViewPerSlot; //<if this is false if the memory is mapped to one view else  one view for each slot is used
	DWORD m_slotCount; //<number of slots 
	DWORD  m_slotSize; //<size per slot
	DWORD m_DataStoreHeaderSize;
	std::string m_MapFileName;
	std::string m_StoreName;
	HANDLE m_hMapFileImage;

	void* m_pViewHeader;
	void* m_pViewData;
	std::vector<void*> m_pViewDataVector;
	DWORD m_AllocationGranularity;

};
typedef  std::unique_ptr<SharedDataStore> DataStorePointer;

} //namespace SvSml



