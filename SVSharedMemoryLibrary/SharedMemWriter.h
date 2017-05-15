//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedMemorySingleton
//* .File Name       : $Workfile:   SVSharedMemorySingleton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Aug 2014 18:48:26  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <map>
#include "MLCpyContainer.h"
#include "MonitorListCpy.h"
#include "SharedImageContainer.h"
#include "SVSharedPPQWriter.h"
#include "SVMonitorListWriter.h"
#include "SVSharedMemorySettings.h"
#include "SVProductFilterEnum.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include  "SVMatroxLibrary/MatroxBuffer2D.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"

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

		HRESULT InsertPPQSharedMemory(const SVString& rName, const SVGUID& rGuid, const long ProductSlots, const long RejectSlots, const SvSml::InspectionWriterCreationInfos& rCreationInfos);
		HRESULT ErasePPQSharedMemory(const SVGUID& rGuid); //< REALY NECESSARY ??
	
		int CreateImageStores(const SVString& InspName, long ProductSlots, long RejectSlots ); 
		//void EraseImageStore(const SVGUID& rGuid);   //< REALY NECESSARY ??
	
		SVSharedPPQWriter& GetPPQWriter(const SVGUID& rGuid);
		SVSharedInspectionWriter& GetInspectionWriter(const SVGUID& rPPQGuid, const SVGUID& rGuid);
		SVMonitorListWriter& GetMonitorListWriter();
		SVString GetInspectionShareName(const SVGUID& rPPQGuid,const SVGUID& rGuid);
		const SVSharedMemorySettings& GetSettings() const;
	
	
		BYTE*  GetImageBufferPtr(DWORD  SlotIndex, SharedImageStore::StoreType t, DWORD storeIndex , DWORD storeoffset); 
	
		///Return the Matrox buffer inthe shared memory;
		SVMatroxBuffer& GetImageBuffer(DWORD  SlotIndex, SharedImageStore::StoreType t, DWORD storeIndex, DWORD ImageIndex);  
	
		///Creates the Matroxbuffer  for images in the Monitorlist in the Imagestores;
		void CreateSharedMatroxBuffer();

	
		/// Clears the PPQ part of the shared memory
		void ClearPPQSharedMemory();
		bool HasShares();
		void Destroy();
	
		/**************function call in m_MLContainer********/
		///Calculates the Storindex, offset and itemindex for all images 
		void CalculateStoreIds();
		void ClearMonitorListCpyVector(); //< clear m_MonitorListCpyVector
		void Insert(MonitorListCpyPointer& MLCpyPtr);
		DWORD GetInspectionImageSize(const SVString& InspName );
		MonitorEntryPointer GetMonitorEntryPointer(const SVString& rname);
		void WriteMonitorList()  ;

	private:
		SharedMemWriter();

		//Do not implement this method
		SharedMemWriter(const SharedMemWriter& p_rObject);

		//Do not implement this method
		const SharedMemWriter& operator=(const SharedMemWriter& p_rObject);

		void ReadSettings();
		void CheckDirectories();

	private:
		typedef std::map<SVGUID, std::unique_ptr<SVSharedPPQWriter>>  PPQSharedMemoryMap; ///map GUID , ptrToSVSharedPPQWrite
	
		PPQSharedMemoryMap m_PPQSharedMemory; 
		SVMonitorListWriter m_monitorListWriter;
		SharedImageContainer m_ImageContainer;
		MLCpyContainer m_MLContainer;  //Container holds MonitorlistInformation  
		SVSharedMemorySettings m_settings;
	};
} //namespace SvSml
