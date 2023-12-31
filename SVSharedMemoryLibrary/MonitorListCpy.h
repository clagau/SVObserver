//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file MonitorlistCpy.h 
/// All Rights Reserved 
//*****************************************************************************

///class for holding Monitorlists 
//******************************************************************************


#pragma once
#pragma region Includes
#include "MonitorEntry.h"
#include "SVProductFilterEnum.h"
#include "SVShared.h"
#include "MLInspectionInfo.h"
#include "MLPPQInfo.h"
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

namespace SvSml
{
	class MesMonitorListCpy;
	///struct holds data for one Monitorlist.
	/// struct is filled in RemoteMonitorListHelper::RemotMonitorNamedList2MonitorListcpy(); 
	struct MonitorListCpy
	{
	public:	
		MonitorListCpy(void);
		
		///Calculates the necessary size of the monitorlist in shared memory
		size_t CalcMemorySize() const;
		/// Insert the totalimage size and data size for this monitorlist to the inspectionInfoMap 
		// Insert PPQINdex to  to PPQIndexMap
		int InsertToMLInspectionInfoMap(MLInspectionInfoMap& inspectionInfoMap, MLPPQInfoMap& PPQIndexMap) const;
	
		
		//Add a new Entry to  the monitorlist
		MonitorEntryPointer  AddEntry(ListType::typ ltype, const std::string& name);
		///Add a new Entry to the monitorlist. 
		MonitorEntryPointer  AddMultEntries(DWORD ListFlags, const std::string& name);

		const MonitorEntries& GetMonitorEntries(ListType::typ type) const; 
		void ClearAll();
		void SetMonitorlistname(const std::string& listname);
		void SetPPQname(const std::string& listname);
		void SetRejectDepth(int RejectDept);
		void SetProductDepth(int RejectDept);

		void SetProductFilter(SVProductFilterEnum fEnum);
		void SetIsActive(bool isactive);
		
		const std::string&  GetMonitorlistname() const;
		const std::string&  GetPPQname() const;
		
		int GetRejectDepth() const;
		int GetProductDepth() const;
		SVProductFilterEnum GetProductFilter() const;
		bool GetIsActive() const;
		bool BuildProtoMessage(SvPml::MesMonitorListCpy& rMessage) const;
		void BuildFromProtoMessage(const SvPml::MesMonitorListCpy& rMessage);
		
		void QueryListItem(const SvPb::QueryListItemRequest& rRequest, SvPb::QueryListItemResponse& rResponse) const;

		void clearInspectionIdsVector() { m_InspectionIdsVector.clear(); };
		void addInspectionIdEntry(int ipMLId, int ipTRCId) { m_InspectionIdsVector.emplace_back(ipMLId, ipTRCId); };
		
	public:
		MonitorEntries m_MonitorEntries[ListType::Count]; //< entrylists  for data,image,reject and failstatus 
		MonitorEntriesMap m_EntriesMap; //< entry map  fullname and all  element from m_MonitorEntries 
		std::string m_MonitorListName;
		std::string m_ppqName;
		int m_rejectDepth;
		int m_ProductDepth;
		bool m_IsActive;
		SVProductFilterEnum m_ProductFilter;
		std::vector<std::pair<int, int>> m_InspectionIdsVector; //first is inspection id of monitorSM and second is inspection id of trigger record controller

	};
	typedef std::unique_ptr<MonitorListCpy> MonitorListCpyPointer;          /// unique Ptr to MonitorListCpy
	typedef std::map<std::string, MonitorListCpyPointer> MonitorListCpyMap;    ///map Monitorlistname MonitorlistcpyPtr 
	typedef std::map<std::string, DWORD> IndexMap;

} //namespace SvSml
