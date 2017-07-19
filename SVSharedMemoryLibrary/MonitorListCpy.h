//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorlistCpy.h 
/// All Rights Reserved 
//*****************************************************************************

///class for holding Monitorlists 
//******************************************************************************


#pragma once
#pragma region Includes
#include "MonitorEntry.h"
#include "SVProductFilterEnum.h"
#include "SVSharedMonitorList.h"
#include "MLInspectionInfo.h"
#include "MLPPQInfo.h"
#pragma endregion Includes

namespace SvSml
{
	
	class SVMonitorListWriter;
	class SVSharedMonitorList;

	///struct holds data for one Monitorlist.
	/// struct is filled in RemoteMonitorListHelper::RemotMonitorNamedList2MonitorListcpy(); 
	struct MonitorListCpy
	{
	public:	
		MonitorListCpy(void);
		MonitorListCpy(const SVSharedMonitorList& sharedMonitorList);
		///Calculates the necessary size of the monitorlist in shared memory
		size_t CalcMemorySize() const;
		/// Insert the totalimage size and data size for this monitorlist to the inspectionInfoMap 
		int InsertToMLInspectionInfoMap(MLInspectionInfoMap& inspectionInfoMap, MLPPQInfoMap& PPQIndexMap) const;
	
		///Write the Monitorlist to the shared Memory 
		void WriteMonitorList(SVMonitorListWriter& rWriter) const;
		
		//Add a new Entry to  the monitorlist
		MonitorEntryPointer  AddEntry(ListType::typ ltype, const SVString& name);

		const MonitorEntries& GetMonitorEntries(ListType::typ type) const; 
		void ClearAll();
		void SetMonitorlistname(const SVString& listname);
		void SetPPQname(const SVString& listname);
		void SetRejectDepth(int RejectDept);
		void SetProductFilter(SVProductFilterEnum fEnum);
		void SetIsActive(bool isactive);
		
		const SVString&  GetMonitorlistname() const;
		const SVString&  GetPPQname() const;
		
		int GetRejectDepth() const;
		SVProductFilterEnum GetProductFilter() const;
		bool GetIsActive() const;
		
		const static DWORD ImageBufferHeaderSize; //<Constant additional size needed fore every imagebuffer

	public:
		
		MonitorEntries m_MonitorEntries[ListType::Count]; //< entrylists  for data,image,reject and failstatus 
		MonitorEntriesMap m_EntriesMap; //< entry map  fullname and all  element from m_MonitorEntries 
		SVString m_MonitorListName;
		SVString m_ppqName;
		int m_rejectDepth;
		bool m_IsActive;
		SVProductFilterEnum m_ProductFilter;
	};
	typedef std::unique_ptr<MonitorListCpy> MonitorListCpyPointer;          /// unique Ptr to MonitorListCpy
	typedef std::map<SVString, MonitorListCpyPointer> MonitorListCpyMap;    ///map Monitorlistname MonitorlistcpyPtr 
	typedef std::map<SVString, DWORD> IndexMap;

} //namespace SvSml
