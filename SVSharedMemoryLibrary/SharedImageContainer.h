/*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedImageContainer.h 
/// All Rights Reserved 
//*****************************************************************************

///class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory 
******************************************************************************/


#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVSharedPPQWriter.h"
#include "SVMonitorListWriter.h"
#include "SVSharedMemorySettings.h"
#include "SVProductFilterEnum.h"
#include "MonitorListCpy.h"
#include "MLCpyContainer.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include  "SVMatroxLibrary/MatroxBuffer2D.h"
#pragma endregion Includes


namespace SvSml
{
	//!class holds the ImageStores and the ImageBuffer for Handling the Images in Shared Memory 
	class SharedImageContainer
	{
	public:
		SharedImageContainer(void);
		~SharedImageContainer(void);
		
		int CreateImageStores(const SVString& InspName, long ProductSlots, long RejectSlots, const MLCpyContainer&  rmlCont );
		
		int OpenImageStores( const MLCpyContainer&  rmlCont );

		void Clear();
		///clear and  removes the entries in the shared memory 
		void DestroySharedMemory();
		BYTE*  GetImageBufferPtr(DWORD  SlotIndex, SharedImageStore::StoreType Store, DWORD storeIndex , DWORD storeoffset);
		SVMatroxBuffer& GetImageBuffer(DWORD  SlotIndex, SharedImageStore::StoreType t, DWORD storeIndex, DWORD ImageIndex);
		void CreateSharedMatroxBuffer( const MLCpyContainer&  rmlCont );

		///return the number of ImageSlot in the Image Store 
		DWORD GetProductSlotCount(DWORD ImageStoreIndex);
		
		///return the number of RejectSlot  in the Image Store 
		DWORD GetRejectSlotCount(DWORD ImageStoreIndex);
	private:
		///Every inspection has its own imagestore 
		std::vector<ImageStorePointer> m_ProductImageStores;
		std::vector<ImageStorePointer> m_RejectImageStores;

		///Every inspection has its own MatroxBufferVector 
		std::vector<UP_MatroxBuffer2D> m_ProductImageBuffer;
		std::vector<UP_MatroxBuffer2D> m_RejectImageBuffer;
	};
} //namespace SvSml
