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
	///Struct holds data for one entry in Monitorlist
	struct MonitorEntry
	{	
	public:
		MonitorEntry();
		MonitorEntry(const SVString &name, DWORD type);
		MonitorEntry(const ShMonitorEntry &rentry );
		
		bool GetMatroxImageProps(MatroxImageProps &ImageProps);
		void SetMatroxImageProps(const MatroxImageProps  &rImageProps);
	public:	
		/// fills the inspectioninfomap, with information of the size for Inspection store
		//int InsertToMLInspectionInfo(MLInspectionInfoMap& inspectionInfoMap ) const;
		
		SVString name; //<Full name 
		DWORD InspectionStoreId; //<Inspection Store Index  
		DWORD ItemId;	//<Index in the Inspection Store
		DWORD ObjectType;		//<SVObjectTypeEnum 
		long long  size;		//Size of the object
		DWORD Store_Offset;  ///offset in Inspection Store
		

		long long sizeY;
		long long sizeX;
		long long PitchByte; 
		long long Pitch; 
		long long Matrox_type; 
		long long Attrib;
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
		DWORD InspectionStoreId; //<Inspection Store Index  
		DWORD ItemId;	//<Index in the Inspection Store
		DWORD ObjectType;		//<SVObjectTypeEnum
		long long  size;		//Size of the object
		DWORD Store_Offset;  ///offset in Inspection Store
		

		long long sizeY;
		long long sizeX;
		long long PitchByte; 
		long long Pitch; 
		long long Matrox_type; 
		long long Attrib;
	};

	typedef bip::allocator<ShMonitorEntry, segment_manager_t>     ShMoListEntry_allocator;
	typedef bip::vector<ShMonitorEntry, ShMoListEntry_allocator> ShMoListEntryVector;
	
} //namespace SvSml
