#pragma once
#pragma region Includes
#include "SVShared.h"
#include "SVUtilityLibrary\SVString.h"
//Moved to precompiled header: #include <vector>  
#pragma endregion Includes



namespace Seidenader { namespace SVSharedMemoryLibrary
{
	///Struct holds data for one entry in Monitorlist
	struct MonitorEntry
	{	
		SVString name;
		DWORD type;
		DWORD size;
	};
	typedef std::vector<MonitorEntry>  MonitorEntryVector;
	
	///Struct holds data for one entry in Monitorlist. Uses boost interprocess data type for string.
	struct ShMonitorEntry
	{	
		ShMonitorEntry(const void_allocator &allocator):name(allocator){;}
		bip_string name;
		DWORD type;
		DWORD size;
	};

	typedef bip::allocator<ShMonitorEntry, segment_manager_t>     ShMoListEntry_allocator;
	typedef bip::vector<ShMonitorEntry, ShMoListEntry_allocator> ShMoListEntryVector;
	
	
} } 

namespace SvSml = Seidenader::SVSharedMemoryLibrary;