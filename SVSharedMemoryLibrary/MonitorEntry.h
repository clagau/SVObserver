//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorEntry.h 
/// All Rights Reserved 
//*****************************************************************************
///classes for holding data for one element in the monitorlist  
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>

#include "SVShared.h"
#include "MLInspectionInfo.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
#include "SVprotobuf/MonitorListStore.h"
#include "SVProtobuf/SVRC.h"
#include "Definitions\SVObjectTypeInfoStruct.h"

#pragma endregion Includes

namespace SvSml
{
	
	///struct holds data element used in MonitorEntry and ShMonitorEntry
	struct MonitorEntryData
	{
		
		bool GetMatroxImageProps(MatroxImageProps& rImageProps) const;
		void SetMatroxImageProps(const MatroxImageProps& rImageProps);
		///Serialize to ProtoBufMessage
		void BuildProtoMessage(SvPml::MesMonitorEntryData& rProtoMessage) const;
		void BuildFromProtoMessage(const SvPml::MesMonitorEntryData& rProtoMessage);

		DWORD InspectionStoreId{ UINT_MAX }; //<Inspection Store Index  
		DWORD ItemId{UINT_MAX };	//<Index in the Inspection Store
		SvPb::SVObjectTypeEnum ObjectType{SvPb::SVNotSetObjectType};
		DWORD variant_type{ VT_EMPTY };  ///vt value from variant
		DWORD Store_Offset{ 0 };  ///offset in Inspection Store

		bool isArray{ false };
		bool wholeArray{ false };
		long arrayIndex =-1;
		DWORD m_MonitorListFlag{ 0 };	 // combination of SvSml::ListFlags[]
		
		long long sizeY{ 0 };		//< MbufInquire  M_SIZE_Y   
		long long sizeX{ 0 };		//< MbufInquire  M_SIZE_Y   
		long long PitchByte{ 0 };	//< MbufInquire  M_PITCH_BYTE   
		long long Pitch{ 0 };		//< MbufInquire  M_PITCH   
		long long Matrox_type{ 0 };  //< MbufInquire  M_TYPE   
		long long Attrib{ 0 };		//< MbufInquire  M_EXTENDED_ATTRIBUTE   
		long long BandSize{ 0};		//< MbufInquire  M_SIZE_BAND   
		long long ByteSize{ 0};		//< MbufInquire  M_SIZE_BYTE   
	};
	
	///Struct holds data for one entry in Monitorlist
	struct MonitorEntry
	{	
	public:
		MonitorEntry();
		MonitorEntry(const std::string &name);
		
		bool GetMatroxImageProps(MatroxImageProps &rImageProps) const;
		void SetMatroxImageProps(const MatroxImageProps  &rImageProps);
		bool GetValue(std::string& rString, BYTE* offset) const;
		///Not used yet 
		bool GetValue(_variant_t& rVal , BYTE* pOffset) const;
		///Serialize to ProtoBufMessage
		void BuildProtoMessage(SvPml::MesMonitorEntry &rMesMonitorEntry) const;
		///set values from serializing 
		void BuildFromProtoMessage(const SvPml::MesMonitorEntry &rMesMonitorEntry);
		void AddListItem(SvPb::QueryListItemResponse& rResp) const;
		bool IsImage() const { return (data.ObjectType == SvPb::SVImageObjectType); };
	public:	
		GUID m_Guid;   //Object Guid 
		std::string name; //<Full name 
		MonitorEntryData data;
	};
	
	typedef std::shared_ptr<MonitorEntry>  MonitorEntryPointer; //< shared_ptr MonitorEntry 
	typedef std::vector<MonitorEntryPointer>  MonitorEntries; //< vector MonitorEntryPointer 
	typedef std::map<std::string, MonitorEntryPointer>  MonitorEntriesMap; //< map objectname MonitorEntryPointer
} //namespace SvSml
