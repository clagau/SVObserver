//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedMemReader.h 
/// All Rights Reserved 
//*****************************************************************************
///// Class encapsulate function to open  and read from the  shared Memory
//******************************************************************************
#pragma once

#include "SharedDataContainer.h"
#include "SVMonitorListReader.h"

namespace SvSml
{

	class SharedMemReader
	{
	public:
		///return values for member functions 
		enum retvalues 
		{
			fail = 0, sucess, last
		};
		SharedMemReader(void);
		~SharedMemReader(void);
		
		///Function call to m_MLContainer
		bool IsActiveMonitorList(const SVString& Monitorlistname ) const;
		DWORD GetVersion() const; 

		///Reload MonitorMap, PPQReader, Open Imagestores and create the imageBuffer 
		///the version number is saved in MLCpyContainer
		void Reload( DWORD version);

		///close Connection to data container clear ML_Container 
		void Clear();
		
		///get SlotmanagerIndex from Monitorlistname 
		int  GetSlotManagerIndexForMonitorList(LPCTSTR Monitorlist);
		
		///get SlotmanagerIndex from PPQName 
		int GetSlotManagerIndex(LPCTSTR PPQname); 
		
		///get Slot manager  from Index  
		RingBufferPointer GetSlotManager(int SlotManagerIndex);
		
		///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns true if successful
		/// the readerslot is released if releaseslot = true;
		BOOL GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, BOOL releaseslot);
		
		///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns true if successful
		/// the readerslot is released if releaseslot = true;
		BOOL GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, BOOL releaseSlot);
		
		///Fills Failstatus  for Monitorlistname  returns true if successful
		BOOL GetFailstatus(LPCTSTR Monitorlist, productPointerVector* pFailstatus);

		
		///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns sucess if successful
		/// the readerslot is released if releaseslot = true;
		// if pLastProduct has the same trigger as rProduct last is returned 
		retvalues GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, BOOL releaseslot);
		///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns sucess if successful
		/// the readerslot is released if releaseslot = true;
		// if pLastProduct has the same trigger as rProduct last is returned 
		retvalues GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, BOOL  releaseslot);
		
		///Fills Failstatus  for Monitorlistname  returns sucess if successful
		///if pLastFailstatus has the same trigger as failstatus would have last is returned
		retvalues GetFailstatus(LPCTSTR Monitorlist, productPointerVector* pFailstatus, productPointerVector*  pLastFailstatus);
		
		BOOL  GetFailStatusData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct);
		SVMatroxBuffer& GetImageBuffer(DWORD  SlotIndex, DWORD storeIndex, DWORD ImageIndex);

	//private:
		SVMonitorListReader m_monitorListReader;
		SharedDataContainer    m_DataContainer;
		MLCpyContainer m_MLContainer;  //Container holds MonitorlistInformation  
		/// map contains monitorlist name Slotmanager index 
		std::map<SVString, int> m_SlotManagerIndexMap; 
	private: 
		//************************************
		// Method:    _GetProduct
		// FullName:  SvSml::SharedMemReader::_GetProduct
		// Access:    private 
		// Returns:   retvalues
		// Qualifier:
		// Parameter: LPCTSTR Monitorlist
		// Parameter: int trigger  -1 for last trigger or reject
		// Parameter: bool failstatus if true only values in Failstatuslist 
		// Parameter: bool reject if true returns last reject for -1 
		// Parameter: MLProduct & rProduct
		// Parameter: BOOL releaseSlot  if true the reader slot is released after this call 
		//************************************
		retvalues _GetProduct(LPCTSTR Monitorlist, int trigger, bool failstatus, bool reject, MLProduct* pProduct, const MLProduct* pLastProduct, BOOL releaseSlot);

	};
} //namespace SvSml
