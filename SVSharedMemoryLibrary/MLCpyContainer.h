/*****************************************************************************/
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLCpyContainer.h 
/// All Rights Reserved 
//*****************************************************************************

///class encapsulate an container with all active monitorlists 
//******************************************************************************

#pragma once
#pragma region Includes
#include "MonitorEntry.h"
#include "SVProductFilterEnum.h"
#include "SVSharedMonitorList.h"
#include "MLInspectionInfo.h"
#include "MonitorListCpy.h"
#include "Definitions\SVIMCommand.h"
#include "Definitions\StringTypeDef.h"
#pragma endregion Includes


namespace SvSml
{
	class SVMonitorListReader;
	class SVMonitorListWriter;
	
	//!class encapsulate an container wit all active monitorlists 
	class MLCpyContainer
	{
	public:
		MLCpyContainer(void);
		~MLCpyContainer(void);

		///Clear all Entries
		void Clear();
		
		//! Load MonitorLists from Shared  Memory
		void ReloadMonitorMap(SVMonitorListReader& rmlReader, DWORD version);
		
		DWORD GetVersion() const; 
		
		//! Get the names of all active Monitorlist 
		int GetAllActiveMonitorlistName(SvDef::StringVector& monotorlistnames) const;

		DWORD GetActiveMonitorlistCount() const;
		///return true if a active monitorlist with the name exist 
		bool IsActiveMonitorList(const std::string& Monitorlistname ) const;
		///return a pointer to Monitorlistcopy for the Monitorlistname 
		const MonitorListCpy*  GetMonitorListCpyPointer(const std::string& Monitorlistname )  const;
		
		const MonitorListCpy*  GetMonitorListCpyPointerForPPQ(const std::string& PPQName)  const;
	
		DWORD GetInspectionStoreId(const std::string& InspectionName);
	
		void Insert(MonitorListCpyPointer& MLCpyPtr);
		
		///Return the total size of the all images for this inspection 
		DWORD GetInspectionImageSize(const std::string& inspectionName   );
		
		///searches all monitorlist for the full dotet nane and return the pointer if it is not found throw an exception 
		MonitorEntryPointer GetMonitorEntryPointer(const std::string& rname);

		///Writes all Monitorlist to shared memory 
		void WriteMonitorList(SVMonitorListWriter& rWriter) ;

		///Calculates the Storindex, offset and itemindex for all images 
		void CalculateStoreIds();
	
		DWORD m_Version;							//< VersionsNumber used from RRS
		MonitorListCpyMap m_MonitorListCpyMap;		//<map Monitorlistname  active MonitorListCpyPointer   
		MLInspectionInfoMap	  m_InspectionInfoMap;	//<Map inspection-nname  InspectionInfos
		MLPPQInfoMap   m_PPQInfoMap;				//<Map ppqName PPQInfo 
	};
} //namespace SvSml
