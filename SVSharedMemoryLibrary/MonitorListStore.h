//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file MonitorListStore.h
/// All Rights Reserved 
//*****************************************************************************
/// class for holding memory for the MonitorList in SharedMemory 
//******************************************************************************
#pragma once
#pragma region Includes
#pragma endregion Includes

namespace SvSml
{
struct  SMParameterStruct;

//!class for holding memory for the MonitorList in SharedMemory
class MonitorListStore
{

public:
	MonitorListStore();
	~MonitorListStore();
	MonitorListStore(const MonitorListStore& datastore) = delete;
	///close connection to datastore
	void  CloseConnection();
	///Destroy the shared memory if it exist
	void CreateMonitorStore(LPCTSTR StoreName, DWORD  size, const SMParameterStruct& rParam);
	bool OpenMonitorStore(LPCTSTR StoreName); //<Connect to existing store 

	///Get Ptr to data in shared memory
	BYTE* GetPtr();
	///Get size 
	DWORD GetSize() const;

private:
	void BuildMapFileName();


private:
	std::string m_MapFileName;
	std::string m_StoreName;
	HANDLE m_hMapFileData = NULL;

	//void* m_pViewHeader = nullptr;
	void* m_pViewData = nullptr;
	DWORD m_AllocationGranularity = 0;
	DWORD m_DataSize = 0;
	DWORD m_SharedSize = 0;

};
typedef  std::unique_ptr<MonitorListStore> MonitorListStorePointer;


}



