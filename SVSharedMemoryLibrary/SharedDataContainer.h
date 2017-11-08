//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedDataContainer.h
/// All Rights Reserved 
//*****************************************************************************
///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory
///Data store and slot management  
//******************************************************************************
#pragma once
#include "MLCpyContainer.h"
#include "SVMatroxLibrary\MatroxBuffer2D.h"
#include "SharedDataStore.h"
#include "SMRingbuffer.h"
#include "MLProduct.h"

		  
namespace SvSml
{

	///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory
	///Data store and slot management  
	class SharedDataContainer
	{
	public:
		enum StoreType
		{
			Slot =0, Image, Data
		};
		static const LPCTSTR ShareNames[];// = { ".SlotManager",".Images", ".Data" };

		SharedDataContainer(void);
		~SharedDataContainer(void);
		
		///Create the slotmanagment for MLCpyContainer
		int CreateSlotManagment( const MLCpyContainer&  rmlCont, const SMParameterStruct& rParam);
		
		///Open the slotmanagment for MLCpyContainer
		int OpenSlotManagment(const MLCpyContainer&  rmlCont);
		
		void CloseSlotManagmentConnection();
		
		//Create the image and datastore with for rmlCont return number of imagestores 
		
		int  CreateStores( const MLCpyContainer&  rmlCont, const SMParameterStruct& rParam);
		//open the image and datastore with for rmlCont return number of imagestores 
		int  OpenStores(const MLCpyContainer&  rmlCont);
		/// clear the image store and the data stores
		void CloseStoreConnection();

		
		BYTE*  GetImageBufferPtr(DWORD  SlotIndex,  DWORD storeIndex, DWORD storeoffset);
		BYTE*  GetDataBufferPtr(DWORD  SlotIndex, DWORD storeIndex, DWORD storeoffset);
		
		void CreateSharedMatroxBuffer(const MLCpyContainer&  rmlCont);
		
		SVMatroxBuffer& GetImageBuffer(DWORD  SlotIndex, DWORD storeIndex, DWORD ImageIndex);

		///return the number of ImageSlot in the Image Store 
		int  GetImageSlotCount(DWORD ImageStoreIndex);

		RingBufferPointer GetSlotManager(int slotmanagerindex);
		void CloseConnection();
	private:
		static void BuildStoreName(LPCTSTR ObjectName, StoreType type, std::string& rStoreName);

		///Every inspection has its own image store 
		std::vector<DataStorePointer> m_ImageStore;
		///Every inspection has its own MatroxBufferVector 
		std::vector<UP_MatroxBuffer2D> m_ImageBufferStore;
		///Every inspection has its own dataStore
		std::vector<DataStorePointer> m_DataStore;
		///Every PPQ has its own slot management 
		std::vector<RingBufferPointer> m_ManagmentStore;
	};
}



