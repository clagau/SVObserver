//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedMemReader.h 
/// All Rights Reserved 
//*****************************************************************************
///// Class encapsulate function to open  and read from the  shared Memory
//******************************************************************************
#pragma once

#include "SharedDataContainer.h"
#include "MonitorListStore.h"

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
		int GetAllActiveMonitorlistName(SvDef::StringVector& monotorlistnames) const
		{
			return m_MLContainer.GetAllActiveMonitorlistName(monotorlistnames);
		}
		bool IsActiveMonitorList(const std::string& Monitorlistname) const
		{
			return m_MLContainer.IsActiveMonitorList(Monitorlistname);
		}

		void QueryListName(const RRApi::QueryListNameRequest& req, RRApi::QueryListNameResponse& resp) const
		{
			return m_MLContainer.QueryListName(req, resp);
		}
		
		void QueryListItem(const RRApi::QueryListItemRequest& req, RRApi::QueryListItemResponse& resp) const
		{
			return m_MLContainer.QueryListItem(req, resp);
		}

		DWORD GetVersion() const
		{
			return m_MLContainer.GetVersion();
		}

		///Reload MonitorMap, PPQReader, Open Imagestores and create the imageBuffer 
		///the version number is saved in MLCpyContainer
		void Reload( DWORD version);

		///close Connection to data container clear ML_Container 
		void Clear();
		
		///get SlotmanagerIndex from Monitorlistname 
		int  GetSlotManagerIndexForMonitorList(LPCTSTR Monitorlist);
		
		///get SlotmanagerIndex from PPQName 
		int GetSlotManagerIndexForPPQName(LPCTSTR PPQname); 
		
		///get Slot manager  from Index  
		RingBufferPointer GetSlotManager(int SlotManagerIndex);
		
		
		///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns sucess if successful
		/// the readerslot is released if releaseslot = true;
		// if pLastProduct has the same trigger as rProduct last is returned 
		retvalues GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, bool releaseslot);
		///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns sucess if successful
		/// the readerslot is released if releaseslot = true;
		// if pLastProduct has the same trigger as rProduct last is returned 
		retvalues GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, const MLProduct* pLastProduct, bool  releaseslot);
		
		///Fills Failstatus  for Monitorlistname  returns sucess if successful
		///if pLastFailstatus has the same trigger as failstatus would have last is returned
		retvalues GetFailstatus(LPCTSTR Monitorlist, vecpProd* pFailstatus, vecpProd*  pLastFailstatus);
		
		//! Get the data from the failstatuslist for triggernumber
		bool  GetFailStatusData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct);
		SVMatroxBuffer& GetImageBuffer(DWORD  SlotIndex, DWORD storeIndex, DWORD ImageIndex);

	//private:
		MonitorListStore      m_MonitorListStore;
		SharedDataContainer    m_DataContainer;
		MLCpyContainer m_MLContainer;  //Container holds MonitorlistInformation  
		/// map contains monitorlist name Slotmanager index 
		std::map<std::string, int> m_SlotManagerIndexMap; 
	private: 
		
		///parameter for GetProduct
		struct GetProdPar
		{
			bool failstatus{ false }; //<only values from the failstatuslist are retrieved
			bool reject{ false };    //< if trigger == -1 the the latest trigger is assumed.
			bool releaseTrigger{ false };  //< release trigger immediately 

		};
		//************************************
		// Parameter: int trigger  -1 for last trigger or reject
		// return last when recent trigger == PreviousTrigger
		//************************************
		//retvalues GetProduct(const GetProdPar& par,LPCSTR monitorlist, int Trigger, int PreviousTrigger, MLProduct* pProduct);
		retvalues _GetProduct(const GetProdPar& par, LPCTSTR Monitorlist, int trigger,  MLProduct* pProduct, const MLProduct* pLastProduct);

	};
} //namespace SvSml
