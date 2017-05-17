//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedImageStore.h 
/// All Rights Reserved 
//*****************************************************************************

///class for holding memory for the imageBufferes in SharedMemory
//******************************************************************************

#pragma once

namespace SvSml
{
	///class for holding memory for the imageBufferes in SharedMemory
	class SharedImageStore
	{

	public:
		enum StoreType {reject, last};
		struct ISHeader
		{
			DWORD slotCount; //<number of slots 
			DWORD  slotSize; //<size per slot
		};


		SharedImageStore();
		~SharedImageStore();
		bool CreateImageStore(const SVString &InspectionName , StoreType t, DWORD  size , DWORD  slots); //<Destroy the shared memory if it exist
		bool OpenImageStore(const SVString &InspectionName, StoreType t);
		bool Destroy(); //<Destroys the shared Memory 
		void  CloseConnection();

		BYTE* GetPtr(DWORD slot, DWORD offset );	
		static LPCTSTR StoreType2Name(StoreType t );
		LPCTSTR GetMapFileName() const;
		DWORD GetSlotCount() const;
		DWORD GetSlotSize() const;
		const SVString & GetInspectionName() const
		{
			return m_InspectionName;
		}

	private:
		//Not implemented don't  allow copyconstructor 
		SharedImageStore(const SharedImageStore& imstore); 
		
		void BuildMapFileName();
		DWORD m_slotCount; //<number of slots 
		DWORD  m_slotSize; //<size per slot
		DWORD m_ISHeadersize;
		StoreType m_StoreType;
		SVString m_MapFileName;
		SVString m_InspectionName;
		HANDLE m_hMapFileImage;
		
		void* m_pViewHeader;
		std::vector<void*> m_pViewImagesVector;
		DWORD m_AllocationGranularity;
		
	};
	typedef  std::unique_ptr<SharedImageStore> ImageStorePointer;

} //namespace SvSml
