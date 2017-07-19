//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedMemWriter.h
/// All Rights Reserved 
//*****************************************************************************
/// Class encapsulate function to create  and write to  the  shared Memory
//******************************************************************************
#pragma once
#pragma region Includes
//Moved to precompiled header: #include <map>
#include "MLCpyContainer.h"
#include "MonitorListCpy.h"
#include "SVMonitorListWriter.h"
#include "SVSharedMemorySettings.h"
#include "SVProductFilterEnum.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include  "SVMatroxLibrary/MatroxBuffer2D.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "SharedDataContainer.h"

#pragma endregion Includes


namespace SvSml
{
	//!SingeltonClass encapsulate function to create and write to the shared Memory
	///Use this class  only on SVObserverside 
	class SharedMemWriter
	{
	public:
		static SharedMemWriter& Instance();
		~SharedMemWriter();

		int CreateManagmentAndStores(DWORD Productslot);
		
		RingBufferPointer GetSlotManager(LPCTSTR PPQname);

		SVMonitorListWriter& GetMonitorListWriter();
		
		const SVSharedMemorySettings& GetSettings() const;
	
		BYTE*  GetDataBufferPtr(DWORD  SlotIndex,  DWORD storeIndex , DWORD storeoffset); 
	
		///Return the Matrox buffer inthe shared memory;
		SVMatroxBuffer& GetImageBuffer(DWORD  SlotIndex,  DWORD storeIndex, DWORD ImageIndex);
	
		///Creates the Matroxbuffer  for images in the Monitorlist in the Imagestores;
		void CreateSharedMatroxBuffer();

	
		/// Clears the PPQ part of the shared memory
		void CloseDataConnection();
		bool HasShares();
		void Destroy();
	
		/**************function call in m_MLContainer********/
		///Calculates the Storindex, offset and itemindex for all images 
		DWORD GetActiveMonitorListCount() const;
		void CalculateStoreIds();
		void ClearMonitorListCpyVector(); //< clear m_MonitorListCpyVector
		void Insert(MonitorListCpyPointer& MLCpyPtr);
		DWORD GetInspectionImageSize(const SVString& InspName );
		MonitorEntryPointer GetMonitorEntryPointer(const SVString& rname);
		void WriteMonitorList()  ;


		
	//private:
		SharedMemWriter();

		//Do not implement this method
		SharedMemWriter(const SharedMemWriter& p_rObject);

		//Do not implement this method
		const SharedMemWriter& operator=(const SharedMemWriter& p_rObject);

		void ReadSettings();
		void CheckDirectories();

	//private:
		SVMonitorListWriter m_monitorListWriter;
		SharedDataContainer m_DataContainer;
		MLCpyContainer m_MLContainer;  //Container holds MonitorlistInformation  
		SVSharedMemorySettings m_settings;
	};
} //namespace SvSml
