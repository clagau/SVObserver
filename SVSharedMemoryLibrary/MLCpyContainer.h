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
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
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
		
		bool IsActiveMonitorList(const SVString& Monitorlistname ) const;
		const MonitorListCpy*  GetMonitorListCpyPointer(const SVString& Monitorlistname )  const;
		void Insert(MonitorListCpyPointer& MLCpyPtr);
		
		///Return the total size of the all images for this inspection 
		DWORD GetInspectionImageSize(const SVString& inspectionName   );
		
		///searches all monitorlist for the nane and return the pointer if it is not found throw an exception 
		MonitorEntryPointer GetMonitorEntryPointer(const SVString& rname);

		///Writes all Monitorlist to shared memory 
		void WriteMonitorList(SVMonitorListWriter& rWriter) ;

		///Calculates the Storindex, offset and itemindex for all images 
		void CalculateStoreIds();
	
		DWORD m_Version;							//< VersionsNumber used from RRS
		MonitorListCpyMap m_MonitorListCpyMap;		//<map Monitorlistname active MonitorListCpyPointer   
		MLInspectionInfoMap	  m_InspectionInfoMap;	//<Map inspection-nname and InspectionInfos
};

}}
namespace SvSml = Seidenader::SVSharedMemoryLibrary;