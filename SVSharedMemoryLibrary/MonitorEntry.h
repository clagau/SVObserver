//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file MonitorEntry.h 
/// All Rights Reserved 
//*****************************************************************************
///classes for holding data for one element in the monitorlist  
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
#include "Definitions/ObjectDefines.h"
#include "SVMatroxLibrary/MatroxImageProps.h"
#include "SVProtoBuf/MonitorListStore.h"
#include "SVProtoBuf/SVRC.h"
#include "SVUtilityLibrary/SVSafeArray.h"
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

		int m_inspectionStoreId{-1}; //<Inspection Store Index  
		int m_inspectionTRCPos = -1;
		SvPb::SVObjectTypeEnum ObjectType{SvPb::SVNotSetObjectType};
		DWORD variant_type{ VT_EMPTY };  ///vt value from variant

		bool isArray{ false };
		bool wholeArray{ false };
		long arrayIndex =-1;
		DWORD m_MonitorListFlag{ 0 };	 // combination of SvSml::ListFlags[]
		
		static const int c_childFlagForTrPos = 0x400000;
		int m_triggerRecordPos = -1;  //if image and c_childFlagForTrPos is set, it is a child image.
				
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
		explicit MonitorEntry(const std::string &name);
		
		bool GetMatroxImageProps(MatroxImageProps &rImageProps) const;
		void SetMatroxImageProps(const MatroxImageProps  &rImageProps);
		///Serialize to ProtoBufMessage
		void BuildProtoMessage(SvPml::MesMonitorEntry &rMesMonitorEntry) const;
		///set values from serializing 
		void BuildFromProtoMessage(const SvPml::MesMonitorEntry &rMesMonitorEntry);
		void AddListItem(SvPb::QueryListItemResponse& rResp) const;
		bool IsImage() const { return (data.ObjectType == SvPb::SVImageObjectType); };

	public:
		uint32_t m_objectId = SvDef::InvalidObjectId;
		std::string name; //<Full name 
		MonitorEntryData data;
	};
	
	typedef std::shared_ptr<MonitorEntry>  MonitorEntryPointer; //< shared_ptr MonitorEntry 
	typedef std::vector<MonitorEntryPointer>  MonitorEntries; //< vector MonitorEntryPointer 
	typedef std::map<std::string, MonitorEntryPointer>  MonitorEntriesMap; //< map objectname MonitorEntryPointer
} //namespace SvSml
