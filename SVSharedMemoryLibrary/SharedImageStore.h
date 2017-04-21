//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedImageStore.h 
/// All Rights Reserved 
//*****************************************************************************

///class for holding memory for the imageBufferes in SharedMemory
//******************************************************************************

#pragma once

namespace Seidenader { namespace SVSharedMemoryLibrary{

///class for holding memory for the imageBufferes in SharedMemory
	class SharedImageStore
	{

	public:
		enum StoreType {reject, last};

		SharedImageStore();
		~SharedImageStore();
		bool CreateImageStore(const SVString &InspectionName , StoreType t, DWORD  size , DWORD  slots); //<Destroy the shared memory if it exist
		bool OpenImageStore(const SVString &InspectionName, StoreType t);
		bool Destroy(); //<Destroys the shared Memory 
		void  CloseConnection();

		BYTE* GetPtr(DWORD slot, DWORD offset );	
		static LPCTSTR StoreType2Name(StoreType t );
		LPCTSTR GetShareName() const;
		DWORD GetSlotCount() const;
		DWORD GetSlotSize() const;
		const SVString & GetInspectionName() const
		{
			return m_InspectionName;
		}

	private:
		//Not implemented don't  allow copyconstructor 
		SharedImageStore(const SharedImageStore& imstore); 
		
		void SetShareName(const SVString &InspectionName, StoreType t);
		DWORD m_slotCount; //<number of slots 
		DWORD   m_slotSize; //<size per slot
		SVString m_ShareName;
		SVString m_InspectionName;
		bip::shared_memory_object* m_pSharedMemoryObject;
		bip::mapped_region*  m_pMappedRegion;
	};

typedef  std::unique_ptr<SharedImageStore> ImageStorePointer;
	
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
namespace SvSml = Seidenader::SVSharedMemoryLibrary;
