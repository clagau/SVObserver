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
#include "MLInspectionInfo.h"
#include "MonitorListCpy.h"
#include "Definitions\StringTypeDef.h"
#include "SVProtoBuf/envelope.h"
#include "SVProtobuf/SVRC.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes


namespace SvSml
{
	class MesMLCpyContainer;
	
	//!class encapsulate an container wit all active monitorlists 
	class MLCpyContainer
	{
	public:
		MLCpyContainer(void);
		~MLCpyContainer(void);

		///Clear all Entries
		void Clear();

		DWORD GetVersion() const; 
		
		//! Get the names of all active Monitorlist 
		int GetAllActiveMonitorlistName(SvDef::StringVector& monotorlistnames) const;

		DWORD GetActiveMonitorlistCount() const;
		///return true if a active monitorlist with the name exist 
		bool IsActiveMonitorList(const std::string& Monitorlistname ) const;
		///return a pointer to Monitorlistcopy for the Monitorlistname 
		const MonitorListCpy*  GetMonitorListCpyPointer(const std::string& Monitorlistname )  const;
		
		const MonitorListCpy*  GetMonitorListCpyPointerForPPQ(const std::string& PPQName)  const;
	
		DWORD GetInspectionStoreId(const std::string& rInspectionName) const;
	
		void Insert(MonitorListCpyPointer& rMLCpyPtr);
		
		///searches all monitorlist for the full dotet nane and return the pointer if it is not found throw an exception 
		MonitorEntryPointer GetMonitorEntryPointer(const std::string& rname);

		///Calculates the Storindex, offset and itemindex for all images 
		void CalculateStoreIds();

		void setDataTrcPos(const std::string& rPPQName, int inspectionStoreId, int inspectionTrcPos, const SvPb::DataDefinitionList& rDataDefList, const SvPb::ImageList& rImageDefList);

		///Serialize to ProtoBufMessage
		void BuildProtoMessage(SvPml::MesMLCpyContainer& rMesMLCpyContainer) const;

		///Fill the class from protobufmessage
		void BuildFromProtoMessage(const SvPml::MesMLCpyContainer& rMesMLCpyContainer);
		
		bool  QueryListName(const SvPb::QueryListNameRequest& rReq, SvPb::QueryListNameResponse& rResp, SvPenv::Error& err) const;
		bool  QueryListItem(const SvPb::QueryListItemRequest& rReq, SvPb::QueryListItemResponse& resp, SvPenv::Error& err) const;

		bool clearInspectionIdsVector(const std::string& rPPQName);
		bool addInspectionIdEntry(const std::string& rPPQName, int ipMLId, int ipTRCId);
		

	//private:
		DWORD m_Version;							//< VersionsNumber used from RRS
		MonitorListCpyMap m_MonitorListCpyMap;		//<map Monitorlistname  active MonitorListCpyPointer   
		MLInspectionInfoMap	  m_InspectionInfoMap;	//<Map inspection-nname  InspectionInfos
		MLPPQInfoMap   m_PPQInfoMap;				//<Map ppqName PPQInfo 
	};
} //namespace SvSml
