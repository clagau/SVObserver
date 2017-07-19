//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorEntry.h 
/// All Rights Reserved 
//*****************************************************************************
///classes for holding data for one element in the monitorlist  
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVShared.h"
#include "SVUtilityLibrary\SVString.h"
#include "MLInspectionInfo.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
#pragma endregion Includes

namespace SvSml
{
	struct ShMonitorEntry; 
	///struct holds data element used in MonitorEntry and ShMonitorEntry
	struct MonitorEntryData
	{
		MonitorEntryData();
		DWORD InspectionStoreId; //<Inspection Store Index  
		DWORD ItemId;	//<Index in the Inspection Store
		DWORD ObjectType;		//<SVObjectTypeEnum 
		DWORD variant_type; ///vt value from variant
		DWORD Store_Offset;  ///offset in Inspection Store

		BOOL isArray;
		BOOL wholeArray;
		long arrayIndex;
		DWORD m_MonitorListFlag; // combination of SvSml::ListFlags[]
		
		long long sizeY;		//< MbufInquire  M_SIZE_Y   
		long long sizeX;		//< MbufInquire  M_SIZE_Y   
		long long PitchByte;	//< MbufInquire  M_PITCH_BYTE   
		long long Pitch;		//< MbufInquire  M_PITCH   
		long long Matrox_type;  //< MbufInquire  M_TYPE   
		long long Attrib;		//< MbufInquire  M_EXTENDED_ATTRIBUTE   
		long long BandSize;		//< MbufInquire  M_SIZE_BAND   
		long long ByteSize;		//< MbufInquire  M_SIZE_BYTE   
	};
	
	///Struct holds data for one entry in Monitorlist
	struct MonitorEntry
	{	
	public:
		MonitorEntry();
		MonitorEntry(const SVString &name);
		MonitorEntry(const ShMonitorEntry &rentry );
		
		bool GetMatroxImageProps(MatroxImageProps &ImageProps);
		void SetMatroxImageProps(const MatroxImageProps  &rImageProps);
		bool GetValue(SVString& string, BYTE* offset);
		bool GetValue(_variant_t& val , BYTE* offset);

	public:	
		GUID m_Guid;   //Object Guid 
		SVString name; //<Full name 
		MonitorEntryData data;
	};
	
	typedef std::shared_ptr<MonitorEntry>  MonitorEntryPointer; //< shared_ptr MonitorEntry 
	typedef std::vector<MonitorEntryPointer>  MonitorEntries; //< vector MonitorEntryPointer 
	typedef std::map<SVString, MonitorEntryPointer>  MonitorEntriesMap; //< map objectname MonitorEntryPointer

	///struct holds data for one entry in Monitorlist. Uses boost interprocess data type for string. Can be used in managed shared memory
	struct ShMonitorEntry
	{	
		ShMonitorEntry(const void_allocator &allocator);
		ShMonitorEntry(const void_allocator &allocator, const MonitorEntry &rentry );
		
		bip_string name;
		MonitorEntryData data;
	};

	typedef bip::allocator<ShMonitorEntry, segment_manager_t>     ShMoListEntry_allocator;
	typedef bip::vector<ShMonitorEntry, ShMoListEntry_allocator> ShMoListEntryVector;
	
} //namespace SvSml
