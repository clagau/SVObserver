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
#include "SVSharedMemorySettings.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"

#include "SharedDataContainer.h"
#include "MonitorListStore.h"

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

		int CreateManagment();

		RingBufferPointer GetSlotManager(LPCTSTR PPQname);
		
		const SVSharedMemorySettings& GetSettings() const;
	
	
		/// Clears the PPQ part of the shared memory
		void CloseDataConnection();
		void Destroy();
	
		/**************function call in m_MLContainer********/
		///Calculates the Storindex, offset and itemindex for all images 
		DWORD GetActiveMonitorListCount() const;
		void CalculateStoreIds();
		void setDataTrcPos(const std::string& rPPQName, int inspectionStoreId, int inspectionTRCPos, const std::unordered_map<uint32_t, int>& rDataDefMap, const std::unordered_map<uint32_t, int>& rImageMap, const std::unordered_map<uint32_t, int>& rChildImageMap);
		void ClearMonitorListCpyVector(); //< clear m_MonitorListCpyVector
		void Insert(MonitorListCpyPointer& MLCpyPtr);
		DWORD GetInspectionStoreId(const std::string& InspectionName);
		const MonitorListCpy*  GetMonitorListCpyPointer(const std::string& Monitorlistname)  const;
		const MonitorListCpy*  GetMonitorListCpyPointerForPPQ(const std::string& PPQNAME)  const;
		MonitorEntryPointer GetMonitorEntryPointer(const std::string& rname);
		void WriteMonitorList();
		bool clearInspectionIdsVector(const std::string& rPPQName);
		bool addInspectionIdEntry(const std::string& rPPQName, int ipMLId, int ipTRCId);
	private:
		SharedMemWriter();
		SharedMemWriter(const SharedMemWriter& p_rObject) =delete;
		const SharedMemWriter& operator=(const SharedMemWriter& p_rObject) =delete;
		void ReadSettings();
	private:
		MonitorListStore	m_monitorListStore;		
		SharedDataContainer m_DataContainer;
		MLCpyContainer m_MLContainer;  //<Container holds MonitorlistInformation  
		SVSharedMemorySettings m_settings;
	};
} 
