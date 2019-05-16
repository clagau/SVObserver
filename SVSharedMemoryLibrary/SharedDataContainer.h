//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedDataContainer.h
/// All Rights Reserved 
//*****************************************************************************
///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory
///Data store and slot management  
//******************************************************************************
#pragma once
#pragma region Includes
#include "MLCpyContainer.h"
#include "SharedDataStore.h"
#include "SMRingbuffer.h"
#pragma endregion Includes
		  
namespace SvSml
{

	///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory
	///Data store and slot management  
	class SharedDataContainer
	{
	public:
		SharedDataContainer(void);
		~SharedDataContainer(void);
		
		///Create the slotmanagment for MLCpyContainer
		int CreateSlotManagment( const MLCpyContainer&  rmlCont, const SMParameterStruct& rParam);
		
		///Open the slotmanagment for MLCpyContainer
		int OpenSlotManagment(const MLCpyContainer&  rmlCont);
		
		RingBufferPointer GetSlotManager(int slotmanagerindex);
		void CloseConnection();
	private:
		///Every PPQ has its own slot management 
		std::vector<RingBufferPointer> m_ManagmentStore;
	};
}



